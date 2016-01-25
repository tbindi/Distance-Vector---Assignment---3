#include <stdio.h>

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>
#include <string>
#include <ctime>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <errno.h>
#include <vector>
#include <sys/time.h>
#include <map>
#include "Main.h"

using namespace std;


// Blind Update Graph with recvd messages
void update_graph(map<byte4,int> update,int row){
	map<byte4,int>::iterator it = update.begin();
	for (it=update.begin(); it!=update.end(); ++it){
		int col = getIndex(it->first);
		graph[row][col] = it->second;
		string add = node_list.at(col)->address;
		cout << add << " - " << it->second << endl;
	}
}

// Check if TTL has expired and update the graph accordingly
void checkTTL(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	for (int i = 0; i < routeTable.size(); ++i)
	{
		if(routeTable.at(i)->time_to_live < (long)tv.tv_sec && i != 0){
			routeTable.at(i)->cost = infinity;
			graph[0][i] = infinity;
		}
	}
}

// Update TTL 
void updateTTL(byte4 address){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	// Used already return function to fetch index & update TTL.
	int i = getIndex(address);
	if(routeTable.at(i)->getAddressInt() == address && i != 0)
	{
		routeTable.at(i)->time_to_live = (long)tv.tv_sec + TTL;
		//to update TTL for neighbouring nodes
		for (int j = 0; j < routeTable.size(); ++j){
			if(routeTable.at(j)->next_hop == routeTable.at(i)->destination){
				routeTable.at(j)->time_to_live = (long)tv.tv_sec + TTL;
			}
		}
	}
}

// Send Updates to neighbours
void send_updates(int sock_fd){
	// send updates every X period of time
	Advertise* ad = new Advertise();
	// Check for default TTL, if TTL is expired -> don't send update to that node. Else send update
	ad->send(sock_fd,routeTable,node_list,splitHorizon);
}

// Recv Updates from neighbours, This is a new Thread. 
// Does not return anything.
void *recv_updates(void *argument){
	int sock_fd;
	sock_fd = * (int *)argument;

	byte* recv_buffer = (byte *) calloc(MAX_SEGMENT_DATA_SIZE,sizeof(byte));
	memset(recv_buffer,0,MAX_SEGMENT_DATA_SIZE);
	struct sockaddr_in adr; // AF_INET
	socklen_t length = sizeof(adr);
	map<byte4,int> resultMap;
	bool init_flag = true;

	while(1) {
		Advertise* ad = new Advertise();
		display_graph();
		int bytes = recvfrom(sock_fd,recv_buffer,MAX_SEGMENT_DATA_SIZE,0,(struct sockaddr *)&adr,&length);
		if(bytes > 0){
	    	Advertise* ad = new Advertise();
	    	resultMap = ad->parse(recv_buffer,bytes/8);

	    	pthread_mutex_lock( &mutex1 );
	    	// Update Cost based on updated TTL
	    	// Check for advertisement/update messages from neighbors. 
	    	checkTTL();
	    	//updateTTL to avoid infinity after stability of network
	    	updateTTL(adr.sin_addr.s_addr);

	    	// Update Graph
	    	// Process received messages by updating edge weights in your graph. 
	    	update_graph(resultMap,getIndex(adr.sin_addr.s_addr));

	    	// Run Bellman Ford Algorithm
	    	// Run Bellman-Ford to recalculate your routing table.
	    	if( bellman_ford(adr.sin_addr.s_addr) ){
	    		// Update TTL
	    		// If any routing table entries change, set the TTL for that entry to the default value.
		    	updateTTL(adr.sin_addr.s_addr);
		    	checkTTL();
	    		cout << "Triggered update" << endl;
	    		logger->printLog(" ---- Triggered update ---- ");
	    		send_updates(sock_fd);
	    	} else {	
	    		// Do this only once.
				gettimeofday(&init_tv,NULL);
	    		if (init_flag){
		    		logger->printLog(" ---- Initialization Time ---- ");
		    		cout << "Initialization Time: " << (long)init_tv.tv_sec - (long)start_tv.tv_sec << endl;
					logger->printLog(SSTR((long)init_tv.tv_sec - (long)start_tv.tv_sec ));	    		
					init_flag = false;
				}
	    	}
	    	pthread_mutex_unlock( &mutex1 );
	    	display_routing_table();

		}else{
	    	cerr << "Error in recving: " << endl;
			cout << "Error Code: " << strerror(errno) << endl;
			exit(1);
	    }
	}
}

int main(int argc, char const *argv[]){
	string config;
	int portNumber;
	int period;
	int socket_fd;
	pthread_t thread[2];
	

	if(argc != 7){
		cout << "Not Enough Arguments, Please enter arguments in format:" << endl;
		cout << "Main <config> <port> <TTL> <infinity> <period> <splitHorizon>" << endl;
		exit(0);
	}else if( argc == 7){
		config = argv[1];
		portNumber = atoi(argv[2]);
		TTL = atoi(argv[3]);
		infinity = atoi(argv[4]);
		period = atoi(argv[5]);
		splitHorizon = atoi(argv[6]);
	}

	Socket* socket = new Socket(AF_INET, SOCK_DGRAM, 0 , portNumber);

	ifstream config_read;
	config_read.open(config.c_str());

	if(!config_read.is_open()){
		cout << "Error in opening file" << endl;
		exit(0);
	}else{
		config_read.close();
		gettimeofday(&start_tv,NULL);
		init(config,portNumber,TTL,infinity,period);
	}
	socket_fd= socket->get_socket();

	int i = pthread_create(&thread[0], NULL, recv_updates, (void*) &socket_fd);
	
	if(i){
		cerr << "Unable to create thread" << endl;
		exit(1);
	}

	while(1) {
		checkTTL();
		//display_routing_table();
		cout << "Periodic Update" << endl;
		logger->printLog(" ---- Periodic Update ---- ");
		pthread_mutex_lock( &mutex1 );
		send_updates(socket->get_socket());
		pthread_mutex_unlock( &mutex1 );
	    sleep(period);
	}

	return 0;
}

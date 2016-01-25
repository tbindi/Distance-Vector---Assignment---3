#ifndef MAIN_H_
#define MAIN_H_

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
#include "RouteEntry.h"
#include "Socket.h"
#include "Advertise.h"
#include "Logger.h"

using namespace std;

typedef unsigned char byte;
typedef unsigned int byte4;
Logger* logger=new Logger();

#define LF '\n'
#define SP ' '

vector<Node*> node_list;
vector<RouteEntry*> routeTable;
vector<vector<int> > graph;
int TTL;
int infinity;
int splitHorizon;
struct timeval convergence_tv;
struct timeval init_tv;
struct timeval start_tv;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

/*
	Utilities:
		Parse Configs, GetAddress in String based on integer,
		get index of the node in our table/graph/nodelist 
		from address in string and byte format
*/
Node* parse_config(string data,int index,int portNumber){
	int pos = data.find(SP);
	string address = data.substr(0,pos);
	bool neighbor = false;
	if( data.substr(pos+1) == "yes" ) neighbor = true;
	return new Node(address,neighbor,portNumber,index);
}

// Get index of a particular address from 
int getIndex(byte4 address){
	int temp = -1;
	for (int i = 0; i < node_list.size(); ++i)
	{
		if(node_list.at(i)->check_address(address)){
			temp = i;
			break;
		}
	}
	return temp;
}
// Get Address in String from Integer
string getAddress(byte4 address){
	int temp = -1;
	for (int i = 0; i < node_list.size(); ++i)
	{
		if(node_list.at(i)->check_address(address)){
			temp = i;
			break;
		}
	}
	return node_list.at(temp)->address;
}

// Get Index of Node from Address
int getIndex(string address){
	int temp = -1;
	for(int i = 0; i < node_list.size(); ++i){
		if(node_list.at(i)->address.compare(address)){
			temp = i;
			break;
		}
	}
	return temp;
}

/*
	display_node_list
		takes	: 	nothing
		returns	:	nothing 
	displays Nodes
*/
void display_node_list(){
	logger->printLog(" ---- Node List ---- ");
	cout << " ---- Node List ---- " << endl;
	for(int i = 0; i < node_list.size(); ++i)
	{
		cout << node_list.at(i)->toString();
		logger->printLog(node_list.at(i)->toString());
	}
	logger->printLog(" ---- **** **** ---- ");
	cout << " ---- **** **** ---- " << endl;
}

/*
	display_distance_vector
		takes	: 	distance_vector
		returns	:	nothing 
	displays Distance Vector Entries
*/
void display_distance_vector(vector<int> distance_vector){
	cout << " ---- Distance Vector ---- " << endl;
	for (int i = 0; i < distance_vector.size(); ++i)
	{
		cout << distance_vector[i] << "  ";
	}
	cout << endl;
	cout << " ---- ******* ***** ---- " << endl;
}

/*
	display_routing_table
		takes	: 	nothing
		returns	:	nothing 
	displays Routing Entries
*/
void display_routing_table(){
	cout << " ---- Routing Table ---- " << endl;
	logger->printLog(" ---- Routing Table ---- ");
	for(int i = 0; i < routeTable.size(); ++i)
	{
		cout << routeTable.at(i)->toString();
		logger->printLog(routeTable.at(i)->toString());
	}
	logger->printLog(" ---- **** **** ---- ");
	cout << " ---- ******* ***** ---- " << endl;
}

/*
	display_graph
		takes	: 	nothing
		returns	:	nothing 
	displays Graph Entries
*/
void display_graph(){
	string stringGraph;
	cout << " ---- Routing Graph ---- " << endl;
	logger->printLog(" ---- Routing Graph ---- ");
	for (int i = 0; i < graph.size(); i++){
	    for (int j = 0; j < graph[i].size(); j++){
	    	logger->printLog(SSTR("Routing Graph.....:" <<graph[i][j] << "  "));
	    	cout << graph[i][j] << "  ";
	    }
	    cout<<endl;
	}
	logger->printLog(" ---- ******* ***** ---- ");
	cout << " ---- ******* ***** ---- " << endl;
}

bool check_graph(int u,int v){
	if(splitHorizon == 1 && graph[u][v] == 1)
		return true;
	else if(splitHorizon != 1 && graph[u][v] != 0)
		return true;
	else
		return false;
}

/*
	relax
		Takes 	: 	u,v, distance vector & Address of the sender
		returns	:	Distance vector
		This is a standard relaxation function by Bellman ford which checks if the recvd update is less than the one that is present.
*/ 
vector<int> relax(int u,int v,vector<int> dist,byte4 address){
	// relax distance vector algorithm
	if(dist[v] > dist[u] + graph[u][v] && dist[u] != infinity){
		dist[v] = dist[u] + graph[u][v];
		string ad;
		if ( u == 0 ){
			ad = node_list.at(v)->address;
		}else{
			ad = node_list.at(u)->address;
		}
		routeTable.at(v)->next_hop = ad;
	}
	return dist;
}

/*
	bellman_ford
		takes	:	Address
		returns	:	Boolean.
		Takes the address of the node which sent the Advertisement. Updates Distance Vector and Routing Table.
*/
bool bellman_ford(byte4 address){
	int adv_index = getIndex(address);
	int count = graph[adv_index].size();
	int src_index = 0;
	bool flag = false;
	vector<int> distance_vector;

	// reinitialize distance vector to 0 everytime you get an update.
	for (int i = 0; i < count; ++i)
	{
		distance_vector.push_back(infinity);
	}
	distance_vector[0] = 0;


	for (int k = 0; k < count-1; ++k)
	{
		for (int i = 0; i < count; ++i)
		{
			for (int j = 0; j < count; ++j)
			{
				// Check for directly connected graph
				if(check_graph(i,j))
					distance_vector = relax(i,j,distance_vector,address);
			}
		}
	}

	display_distance_vector(distance_vector);

	for (int i = 1; i < distance_vector.size(); ++i)
	{
		if(distance_vector[i] != routeTable.at(i)->cost){
			routeTable.at(i)->cost = distance_vector[i];
			if(routeTable.at(i)->cost == infinity){
				gettimeofday(&convergence_tv,NULL);
				logger->printLog(" ---- Convergence Time Based on Last Known Stable Update: ---- ");
		    	cout << "Convergence Time: " << (long)convergence_tv.tv_sec - (long)init_tv.tv_sec << endl;
				logger->printLog(SSTR((long)convergence_tv.tv_sec - (long)init_tv.tv_sec));

				logger->printLog(" ---- Convergence Time Based on Last Known Node Update: ---- ");
		    	cout << "Convergence Time: " << - ((long)convergence_tv.tv_sec - routeTable.at(i)->time_to_live) << endl;
				logger->printLog(SSTR( -((long)convergence_tv.tv_sec - routeTable.at(i)->time_to_live)) );

				logger->printLog(" ---- Lost Node: ---- ");
		    	cout << "Lost Node: " << routeTable.at(i)->destination << endl;
				logger->printLog(routeTable.at(i)->destination);

			}
			flag = true;
			break;
		}
	}

	return flag;
}

/*
	init
		takes	:	config file name, infinity value, TTL, portNumber
		returns	:	Nothing.
		Initializes Routing Table & Node List
*/
void init(string config,int portNumber,int TTL,int infinity, int period){
	ifstream read;
	read.open(config.c_str());
	string data;
	int i = 0;
	char sourceHostName[50];
	gethostname(sourceHostName,sizeof(sourceHostName));

	node_list.push_back(new Node(sourceHostName,false,portNumber,0));
	struct timeval tv;
	gettimeofday(&tv,NULL);
	routeTable.push_back(new RouteEntry(sourceHostName,sourceHostName,0,(long)tv.tv_sec+99999));

	while(getline(read,data)) {

	   	node_list.push_back(parse_config(data,i,portNumber));
	   	Node* temp = node_list.back(); // Create Routing Entry using inserted Node.

	   	if(temp->neighbor){
	   		routeTable.push_back(new RouteEntry(temp->address,temp->address,1,(long)tv.tv_sec+TTL));
	   	}else{
	   		routeTable.push_back(new RouteEntry(temp->address,"",infinity,(long)tv.tv_sec+TTL));
	   	}
	    i++;
	}

	graph.resize(node_list.size(), vector<int>( node_list.size() , infinity ) );
	for (int i = 0; i < node_list.size(); ++i)
	{
		if( node_list.at(i)->neighbor){
			graph[0][i] = 1;
		}
		graph[i][i] = 0;
	}

	read.close();
}

#endif

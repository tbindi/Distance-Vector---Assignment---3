/*
 * Advertise.cpp
 *
 *  Created on: Nov 18, 2015
 *      Author: abhi
 */

#include "Advertise.h"
#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
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
#include "Node.h"
#include <vector>
#include <map>
#include <bitset>

typedef unsigned char byte;
typedef unsigned int byte4;

// Prepares Buffer to be sent to the Destination (dest) depending on SplitHorzon or not.
byte* Advertise::prepare(vector<RouteEntry*> routing_table,string dest,int split){
	int shiftCount=0;
	byte* buffer= (byte*) calloc(MAX_SEGMENT_DATA_SIZE,sizeof(byte*));
	memset(buffer,0,MAX_SEGMENT_DATA_SIZE);
	this->no_sent = 0;
	
	for(int i = 1; i < routing_table.size(); i++)
	{
		if(split == 1){
			if( dest.compare(routing_table.at(i)->next_hop) == 0 
				&& routing_table.at(i)->next_hop.compare(routing_table.at(i)->destination) != 0){
				continue;
			}
		}

		byte4 ip = routing_table.at(i)->getAddressInt();
		memcpy(buffer+shiftCount,&ip,sizeof(byte4));

		shiftCount += sizeof(byte4);

		memcpy(buffer+shiftCount,&routing_table.at(i)->cost,sizeof(int));
		shiftCount += sizeof(int);
		this->no_sent++;
	}
	return buffer;
}

// Sends Advertisement to its neighbours.
bool Advertise::send_advertisement(int sock_fd,Node* node,byte* buffer){
	socklen_t client_length = sizeof(node->node_address);
	if(sendto(sock_fd,buffer,this->no_sent*8,0,(struct sockaddr *)&node->node_address, client_length)){
		return true;
	}
	return false;
}

// Send Advertise will check for neighbour and then sends Advertisement message based on Split Horizon.
void Advertise::send(int sock_fd,vector<RouteEntry*> routeTable,vector<Node*> node_list,int split){
	for (int i = 0; i < node_list.size(); ++i)
	{
		if(node_list.at(i)->neighbor){
			byte* buffer = prepare(routeTable,node_list.at(i)->address,split);
			if(!send_advertisement(sock_fd,node_list.at(i),buffer)){
				cout << "Error in sending update." << endl;
			}
		}
	}
}

// Parses the Recvd Message. Converts the Byte format to Integer. (IP 32 bits, Cost 32 bits) 
map<byte4,int> Advertise::parse(byte* buffer,int count){
	map<byte4,int> result;
	for (int i = 0; i < count; ++i)
	{
		byte4 ip = (buffer[3+(i*8)] << 24) | (buffer[2+(i*8)] << 16) | (buffer[1+(i*8)] << 8) | (buffer[0+(i*8)]);
		int cost = (buffer[7+(i*8)] << 24) | (buffer[6+(i*8)] << 16) | (buffer[5+(i*8)] << 8) | (buffer[4+(i*8)]);
		if(ip == 0)
			continue;
	    result.insert(pair<byte4,int>(ip,cost));
	}
	return result;
}

Advertise::Advertise() {
	// TODO Auto-generated constructor stub

}

Advertise::~Advertise() {
	// TODO Auto-generated destructor stub
}

/* namespace std */

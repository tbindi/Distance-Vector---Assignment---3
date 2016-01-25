/*
 * Advertise.h
 *
 *  Created on: Nov 18, 2015
 *      Author: abhi
 */

#ifndef ADVERTISE_H_
#define ADVERTISE_H_
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
#include <bitset>
#include <map>
#include "RouteEntry.h"

typedef unsigned char byte;
typedef unsigned int byte4;
using namespace std;

/*
	Advertise Class:
		Does not have a member function that is used.
		This is more like an interface which handles Sending & Recving of Advertisement.
		Prepares Message to be sent to a node. 
		Parses Message recvd from a node.
*/
class Advertise {
public:
	Advertise();
	int no_sent;
	
	byte* prepare(vector<RouteEntry*> routing_table,string dest,int split);
	
	void send(int sock_fd,vector<RouteEntry*> routeTable,vector<Node*> node_list,int split);
	
	bool send_advertisement(int sock_fd,Node* node,byte* buffer);

	map<byte4,int> parse(byte* buffer,int count);
	
	virtual ~Advertise();
};

 /* namespace std */
#endif /* ADVERTISE_H_ */


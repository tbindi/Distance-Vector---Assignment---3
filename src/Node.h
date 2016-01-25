#ifndef NODE_H_
#define NODE_H_

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

using namespace std;

typedef unsigned int byte4;

/*
	Node Class:
		Stores information about Router/Node such as Neighbour, Index in the Graph, Address in String & SockAddr_in format.
		Config will be parsed and Node information about the network will be stored in this class.
*/
class Node{
public:
	Node();
	int index;
	bool neighbor; // yes-> neighbour, no-> not a neighbor
	struct sockaddr_in node_address;
	string address;
	bool available; // yes-> neighbour is available(Within TTL)!!! no-> neighbour is dead!!!

	Node(string host_address,bool neighbor,int port,int index);
	
	string toString();
	
	void Setup_Node(vector<Node*> node_list);
	
	byte4 address_to_int();
	
	bool check_address(byte4 addr);
	
	virtual ~Node();
};

#endif

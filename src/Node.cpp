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
#include "Node.h"

using namespace std;

typedef unsigned int byte4;

// Initializes Nodes with Address, Neighbour, Portnumber and Index
Node::Node(string host_address,bool neighbor,int port,int index){
	this->index = index;
	this->address = host_address;
	this->node_address.sin_port = htons(port);
	this->node_address.sin_family = AF_INET;

	if(inet_pton(AF_INET,host_address.c_str(),&this->node_address.sin_addr) < 1){
		struct hostent *host;
		host = gethostbyname(host_address.c_str());
		// hostname to ip address
		if(host){
			struct in_addr **address_list;
			address_list = (struct in_addr **) host->h_addr_list;
			for(int i=0; address_list[i] != NULL; i++){
		 		this->node_address.sin_addr = *address_list[i];
		 		break;
		 	}
		}else{
			cerr << "Error Resolving Host: " << host_address << endl;
			cout << "Error Code" << strerror(errno) << endl;
			exit(0);
		}
	}
	this->neighbor = neighbor;
	
	
	if(this->neighbor)
		this->available = true;
	else
		this->available = false;

}

// Converts Address to Integer or Byte4
byte4 Node::address_to_int(){
	return this->node_address.sin_addr.s_addr;
}

// Checks if the Address is true or not.
bool Node::check_address(byte4 addr){
	if(this->node_address.sin_addr.s_addr == addr)
		return true;
	else
		return false;
}

// Converts the Node members to printable string.
string Node::toString(){
	stringstream data;
	data << "Index: " << this->index << "\taddress: " << this->address << "\t\t\tneighbor: " << this->neighbor << endl; 
	return data.str();
}

Node::Node(){
	// TODO Auto-generated constructor stub

}
Node::~Node() {
	// TODO Auto-generated destructor stub
}

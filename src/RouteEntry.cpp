/*
 * RouteEntry.cpp
 *
 *  Created on: Nov 17, 2015
 *      Author: abhi
 */

#include "RouteEntry.h"
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
#include "Node.h"

#define AD 8

using namespace std;

typedef unsigned char byte;
typedef unsigned int byte4;
typedef unsigned short byte2;

// Constructor for RouteEntry
RouteEntry::RouteEntry(string destination,string next_hop,int cost,long time_to_live){
	this->destination = destination;
	this->next_hop = next_hop;
	this->cost = cost;
	this->time_to_live = time_to_live;
}

// Converts RouteEntry members to printable string
string RouteEntry::toString(){
	stringstream data;
	data << "Destination: " << this->destination;
	data << "\t\tNext hop: " << this->next_hop;
	data << "\t\tCost: " << this->cost;
	data << "\t\tTTL: " << this->time_to_live << endl;
	return data.str();
}

// Returns Integer from String (IP Address)
byte4 RouteEntry::getAddressInt(){
	// Create a dummy node and get the address in integer.
	Node* temp = new Node(this->destination,false,0,0);
	return temp->address_to_int();
}

// Returns the Corresponding Node object.
Node* RouteEntry::getNode(vector<Node*> node_list){
	int index = 0;
	for (int i = 0; i < node_list.size(); ++i)
	{
		if(node_list.at(i)->address.compare(this->destination) == 0){
			index = i;
			break;
		}
	}
	return node_list.at(index);
}

RouteEntry::RouteEntry() {
	// TODO Auto-generated constructor stub
}

RouteEntry::~RouteEntry() {
	// TODO Auto-generated destructor stub
}


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
#include "Socket.h"

using namespace std;

Socket::Socket(int domain,int type,int protocol,int port){
	this->port = port;
	this->sock_fd = -1;
	this->socket_server(domain,type,protocol);
	if(this->bind_server() > 0){
		cout << "Binding successfully done...." << endl;
	}
}

int Socket::get_socket(){
	return this->sock_fd;
}

int Socket::socket_server(int domain, int type, int protocol){

	this->sock_fd = socket(domain, type, protocol);

	if (this->sock_fd < 0) {
		cerr << "Error in opening socket: " << strerror(errno) << endl;
		exit(0);
	}

	return 1;
}

int Socket::bind_server(){
	
	struct sockaddr_in server_address;
	
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY; // gets message from all the interfaces.
	server_address.sin_port = htons(this->port);
	
	if( bind(this->sock_fd, (struct sockaddr *) &server_address, sizeof(server_address)) < 0 ){
		cerr << "Error in binding: " << endl;
		cout << "Error Code: " << strerror(errno) << endl;
		exit(1);
	}

	return 1;
}
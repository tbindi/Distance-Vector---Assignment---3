/*
 * Logger.cpp
 *
 *  Created on: Nov 29, 2015
 *      Author: abhi
 */

#include "Logger.h"
using namespace std;
void Logger::printLog(string data){
	char buffer[1000];
	struct timeval tv;
	struct tm * timeinfo;
	time_t current;
	gettimeofday(&tv, NULL);

	current=tv.tv_sec;
	timeinfo=localtime(&current);

	strftime(buffer,1000,"%m-%d-%Y %T.",timeinfo);
	log_fd << buffer << tv.tv_usec << "[INFO] " << data << endl;

	}

Logger::Logger(){
	log_fd.open( "router.log", ios::out | ios::app );
	// TODO Auto-generated constructor stub

}

Logger::~Logger() {
	// TODO Auto-generated destructor stub
}

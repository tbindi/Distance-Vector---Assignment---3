/*
 * Logger.h
 *
 *  Created on: Nov 29, 2015
 *      Author: abhi
 */
#ifndef LOGGER_H_
#define LOGGER_H_
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <fstream>
#define SSTR( x ) dynamic_cast< std::ostringstream & >(( std::ostringstream() << std::dec << x ) ).str()

using namespace std;
class Logger {
public:
	fstream log_fd;
	Logger();
	void printLog(string data);

	virtual ~Logger();
};

#endif /* LOGGER_H_ */

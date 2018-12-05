
#ifndef _ROBOT_H
#define _ROBOT_H

#include <sys/time.h>
#include "poller.h"
#include "finder.h"

class Robot
{
	struct timeval etv;
	struct timeval ttv;
	unsigned long long tb;
	
	Poller poller;
	int numFinders;
	Finder *finders;
	bool started;
	
public:
	Robot(int nfinders);
	~Robot();

	void addPage(const char *url);
	void printState();
	void printStats();
	void printData();
	
	void init(Pollable *p = NULL);
	void start();
	void stop();
	void load(char **dfiles, int ndfiles);
	void save();	
};

#endif


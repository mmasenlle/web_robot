
#ifndef POLLER_H
#define POLLER_H

#include "pollable.h"

class Poller
{
	int max;
	nfds_t nfds;
	Pollable **pollables;
	struct pollfd *fds;	

public:
	Poller(int m = 256);
	~Poller();	
	void addPollable(Pollable *p);
	void go();
};

#endif


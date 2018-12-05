
#ifndef READSELECT_H
#define READSELECT_H

#include <poll.h>
#include <vector>
#include "selectable.h"

class ReadSelect
{
	std::vector<Selectable*> elements;
	struct pollfd *fds;
	nfds_t nfds;
	void _step();

public:
	ReadSelect();
	~ReadSelect();	
	void addElement(Selectable *e);
	void step();
	void go();
};

#endif


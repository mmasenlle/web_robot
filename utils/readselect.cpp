
#include <fcntl.h>
#include "conf.h"
#include "readselect.h"

ReadSelect::ReadSelect()
 : fds(NULL), nfds(0)
{
}

ReadSelect::~ReadSelect()
{
	if(fds) delete [] fds;
}

void ReadSelect::addElement(Selectable *e)
{
	elements.push_back(e);
}

void ReadSelect::step()
{
	if(nfds != elements.size())
	{
		if(fds) delete [] fds;
		nfds = elements.size();
		fds = new pollfd[nfds];
	}
	_step();
}

void ReadSelect::go()
{
	if(nfds != elements.size())
	{
		if(fds) delete [] fds;
		nfds = elements.size();
		fds = new pollfd[nfds];
	}
	for(;;) _step();
}

void ReadSelect::_step()
{
	for(int i = 0; i < nfds; i++)
	{
		fds[i].fd = elements[i]->getFd();
		int flags = fcntl(fds[i].fd, F_GETFL, 0);
		if(!(flags & O_NONBLOCK))
			fcntl(fds[i].fd, F_SETFL, flags | O_NONBLOCK);
		fds[i].events = elements[i]->getFlags();
	}
	if(poll(fds, nfds, -1) < 1)
	{
		if(Conf::verbose) fprintf(stderr, "Error de poll\n");
	}
	else
	{
		for(int i = 0; i < nfds; i++)
		{
			if(fds[i].fd != -1 && fds[i].revents)
			{
fprintf(stderr, "Evento en %d fd: %d revents: %d\n", i, fds[i].fd, fds[i].revents);
				elements[i]->go();
			}
		}
	}
}

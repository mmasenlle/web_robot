
#include <stdio.h>
#include "conf.h"
#include "poller.h"

Poller::Poller(int m)
 : nfds(0), max(m)
{
	pollables = new Pollable*[max];
	fds = new pollfd[max];
}

Poller::~Poller()
{
	delete [] pollables;
	delete [] fds;
}

void Poller::addPollable(Pollable *p)
{
	fds[nfds].fd = 0;
	fds[nfds].events = POLLIN;
	p->setPFd(&fds[nfds]);
	pollables[nfds] = p;
	nfds++;
}

void Poller::go()
{
	for(;;)
	{
		if(poll(fds, nfds, Conf::timeout) == -1)
		{
			if(Conf::verbose) fprintf(stderr, "poll error\n");
		}
		else
		{
			for(int i = 0; i < nfds; i++)
			{
//fprintf(stderr, "Evento id: %d fd: %d revents: %d\n", i-1, fds[i].fd, fds[i].revents);
				if(fds[i].revents || (fds[i].fd == -1))
					pollables[i]->go();
			}
		}
	}
}

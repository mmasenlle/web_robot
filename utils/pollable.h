#ifndef POLLABLE_H_
#define POLLABLE_H_

#include <poll.h>
#include <fcntl.h>

class Pollable
{
protected:
	struct pollfd *pfd;
public:
	Pollable() : pfd(0) {};
	void setPFd(pollfd *p) { pfd = p; };
	virtual void go() =0;
	
	static void setnblock(int fd)
	{
		int flags = fcntl(fd, F_GETFL, 0);
		if(flags == -1) flags = 0;
		fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	}
};

#endif /*POLLABLE_H_*/

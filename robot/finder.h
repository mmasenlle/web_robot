
#ifndef _FINDER_H
#define _FINDER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "pollable.h"
#include "resolver.h"
#include "pagecontainer.h"
#include "strautom.h"
#include "linkautom.h"


#define BUFF_TAM		4096

class Finder : public Pollable
{
	int id, state;
	int s, len, offset;
	const Page *curPage;
	struct sockaddr_in conndat;
	LinkAutom linkautom;
	StrAutom strautom;
	bool run, running;
	unsigned long long acum;
	static char buffer[BUFF_TAM];

	int find();
	int waitip();
	int Connect();
	int Send();
	int firstRecv();
	int Recv();
	int vacia();
	
public:
	static Resolver resolver;
	static PageContainer pages;
	
	void go();

	Finder();
	void start(int fid);
	void stop();
	void print_state();
	void print_curPages(FILE *f);
	unsigned long long getAcum();
};

#endif


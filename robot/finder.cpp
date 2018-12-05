
#include <string>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tiempos.h"
#include "conf.h"
#include "finder.h"

enum { s_stop = 0, s_waitip, s_send, s_first_recv, s_recv, s_vacia };

char Finder::buffer[BUFF_TAM];
Resolver Finder::resolver;
PageContainer Finder::pages;

Finder::Finder() : linkautom(pages) {}

void Finder::start(int fid)
{
	id = fid;
	s = -1;
	curPage = NULL;
	conndat.sin_family = AF_INET;
	conndat.sin_port = htons(80);
	
	run = true;
	state = find();
}

void Finder::stop()
{
	run = false;
	curPage = NULL;
	state = s_stop;
	pfd->fd = -2;
	pfd->events = 0;
}

void Finder::go()
{
//fprintf(stderr, "[:%d] Finder::go -> enter state: %d\n", id, state);
	switch(state)
	{
	case s_stop:
		state = find();
		break;
	case s_waitip:
		state = waitip();
		break;
	case s_send:
		state = Send();
		break;
	case s_first_recv:
		state = firstRecv();
		break;
	case s_recv:
		state = Recv();
		break;
	case s_vacia:
		state = vacia();
		break;
	}
//fprintf(stderr, "[:%d] Finder::go -> exit state: %d\n", id, state);
}

int Finder::find()
{
	if(s != -1) close(s), s = -1;
	delete curPage;

	if(!(running = run) || !(curPage = pages.getPage(id)))
		return s_stop;

	if(!(conndat.sin_addr.s_addr = curPage->getHost()->getIp()))
	{
		pfd->fd = -1;
		pfd->events = 0;
		resolver.query(id, curPage->getHost()->getName());
		return s_waitip;
	}
	return Connect(); 
}

int Finder::waitip()
{
	unsigned long ip = resolver.getIp(id);
	if(ip == -1)
	{
		if(Conf::verbose) fprintf(stderr, "Finder::waitip -> -error resolving- "
				"host: %s\n", curPage->getHost()->getName());
		return find();
	}
	else if(ip)
	{
		curPage->getHost()->setIp(ip);
		conndat.sin_addr.s_addr = ip;
		return Connect();
	}
	return s_waitip;
}

int Finder::Connect()
{
	s = socket(PF_INET, SOCK_STREAM, 0);
	setnblock(s);
	connect(s, (sockaddr*)&conndat, sizeof(conndat));
	pfd->fd = s;
	pfd->events = POLLOUT;
	return s_send;
}

int Finder::Send()
{
	pfd->events = POLLIN;
	std::string sbuf = "GET ";		sbuf += curPage->getPath();
	sbuf += " HTTP/1.0\r\nHost: ";	sbuf += curPage->getHost()->getName();
	sbuf += "\r\nConnection: Close\r\n\r\n";
	if(write(s, sbuf.c_str(), sbuf.length()) != sbuf.length())
	{
		if(Conf::verbose) fprintf(stderr, "Finder::Send -> -not sent whole buffer- "
				"host: %s path: %s\n", curPage->getHost()->getName(), curPage->getPath());
		return find();
	}
	return s_first_recv;
}

int Finder::firstRecv()
{
	if((len = read(s, buffer, BUFF_TAM)) <= 0)
	{
		if(Conf::verbose) fprintf(stderr, "Finder::firstRecv -> -read error-"
			" len: %d host: %s path: %s\n", len, curPage->getHost()->getName(), curPage->getPath());
		return find();
	}
	bool btext = false;
	offset = 0;
	for(int i = 0; i < len; i++)
	{
		if(!strncmp(buffer + i, "\r\n\r\n", 4))
		{
			offset = i + 4; break;
		}
		else if(!strncmp(buffer + i, "ype: text/", 10))
		{
			btext = true;
		}
	}
	if(!btext)
	{
		if(Conf::verbose) fprintf(stderr, "Finder::firstRecv -> -no text found- "
			"host: %s path: %s\n", curPage->getHost()->getName(), curPage->getPath());
		return find();	
	}
	if(buffer[9] == '4')
	{
		if(Conf::verbose) fprintf(stderr, "Finder::firstRecv -> -http error code- "
				"host: %s path: %s\n", curPage->getHost()->getName(), curPage->getPath());
		return s_vacia;
	}
	
	linkautom.setPage(curPage);
	strautom.setPage(curPage);

	for(int k = offset; k < len; k++)
	{
		LinkAutom_PROCESS(linkautom, buffer[k]);
		StrAutom_PROCESS(strautom, buffer[k]);
	}
	acum += (len - offset);
	return s_recv;	
}

int Finder::Recv()
{
	if((len = read(s, buffer, BUFF_TAM)) > 0)
	{
		for(int k = 0; k < len; k++)
		{
			LinkAutom_PROCESS(linkautom, buffer[k]);
			StrAutom_PROCESS(strautom, buffer[k]);
		}
		acum += len;
		return s_recv;
	}
	return find();
}

int Finder::vacia()
{
	if(read(s, buffer, BUFF_TAM) > 0)
		return s_vacia;
	
	return find();
}

void Finder::print_curPages(FILE *f)
{
	if(curPage) curPage->print(f);
}

void Finder::print_state()
{
	printf("[%d] run:%d%d qsize: %d\ts: %d\ttam: %d\ttotal: %dB\n",
	 id, run?1:0, running?1:0, pages.getAfqSize(id), s, len, acum);
	if(curPage) printf("Page: %s %s\n", curPage->getHost()->getName(), curPage->getPath());
	printf("\n");
}

unsigned long long Finder::getAcum()
{
	unsigned long long aux = acum;
	acum = 0;
	return aux;
}

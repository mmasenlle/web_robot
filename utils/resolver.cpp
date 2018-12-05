
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "resolver.h"

void Resolver::start(int m)
{
	if(max) return;
	max = m;
	rregs = (rreg_t*)calloc(max, sizeof(rreg_t));
	pthread_mutex_init(&mtx, NULL);
	pthread_cond_init(&cond, NULL);
	pthread_t th;
	if(pthread_create(&th, NULL, resthread, this) == -1)
		fprintf(stderr, "Resolver::start -> error creating thread\n");
	else pthread_detach(th);
}

void Resolver::query(int id, const char *hname)
{
	rregs[id].ip = 0;
	rregs[id].host = hname;
	pthread_cond_broadcast(&cond);
}

unsigned long Resolver::getIp(int id)
{
	return rregs[id].ip; 
}

unsigned long Resolver::resolv(const char *hname)
{
	unsigned long ip = 0;
	struct addrinfo *addr_info = NULL;
	struct addrinfo hints = { 0 };
	hints.ai_family = PF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if(!getaddrinfo(hname, NULL, &hints, &addr_info))
	{
		ip = ((struct sockaddr_in *)(addr_info->ai_addr))->sin_addr.s_addr;
		freeaddrinfo(addr_info);
	}
	else if(!inet_aton(hname, (in_addr*)&ip))
	{
		return (unsigned long)(-1);
	}
	return ip;
}

void Resolver::resolver()
{
	for(;;)
	{
		for(int i = 0; i < max; i++)
		{
			if(rregs[i].host)
			{
				rregs[i].ip = resolv(rregs[i].host);
				rregs[i].host = NULL;
			}
		}
		pthread_mutex_lock(&mtx);
		bool go = false;
		for(int i = 0; i < max; i++)
		{
			if(rregs[i].host)
			{
				go = true;
				break;
			}
		}
		if(!go) pthread_cond_wait(&cond, &mtx);
		pthread_mutex_unlock(&mtx);
	}
}

void *Resolver::resthread(void *p)
{
	((Resolver*)p)->resolver();
	return NULL;
}

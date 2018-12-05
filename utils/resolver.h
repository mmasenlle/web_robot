#ifndef RESOLVER_H_
#define RESOLVER_H_

#include <pthread.h>

struct rreg_t
{
	unsigned long ip;
	const char *host;
};

class Resolver
{
	pthread_mutex_t mtx;
	pthread_cond_t cond;
	int max;
	struct rreg_t *rregs;

	unsigned long resolv(const char *hname);
	void resolver();
	static void *resthread(void *p);
public:
	void start(int m);
	void query(int id, const char *hname);
	unsigned long getIp(int id);
};

#endif /*RESOLVER_H_*/


#ifndef _AFQUEUE_H_
#define _AFQUEUE_H_

#define BUF_TAM 256

class AfQueue
{
	int iRead, iWrite;
	const void *data[BUF_TAM];

public:
	AfQueue();
	int size();
	
	bool push(const void *e);
	const void *pop();	
};

#endif /*AFQUEUE_H_*/

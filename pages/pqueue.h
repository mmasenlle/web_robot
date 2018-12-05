
#ifndef _PQUEUE_H_
#define _PQUEUE_H_

#include <vector>

class PQueue
{
	int iRead, jRead;
	int iWrite, jWrite;
	std::vector<const void **> data;
	
public:
	PQueue();
	~PQueue();
	int size();
	
	void push(const void *e);
	const void *pop();	
};

#endif /*PQUEUE_H_*/

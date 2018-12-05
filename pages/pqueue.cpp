
#include "pqueue.h"

#define CHUNK_TAM 1024

PQueue::PQueue()
 : iRead(0), jRead(0), iWrite(0), jWrite(0)
{
	data.clear();
	data.push_back(new const void*[CHUNK_TAM]);
}

PQueue::~PQueue()
{
	for(int i = iRead; i <= iWrite; i++)
		delete [] data[i];
}

int PQueue::size()
{
	return (((iWrite - iRead) * CHUNK_TAM) + jWrite - jRead);
}
	
void PQueue::push(const void *e)
{
	if(jWrite == CHUNK_TAM)
	{
		data.push_back(new const void*[CHUNK_TAM]);
		jWrite = 0;
		iWrite++;
	}
	data[iWrite][jWrite++] = e;
}

const void *PQueue::pop()
{
	if(iWrite > iRead || jWrite > jRead)
	{
		if(jRead == CHUNK_TAM)
		{
			jRead = 0;
			delete data[iRead];
			data[iRead] = NULL;
			iRead++;
		}
		return data[iRead][jRead++];
	}
	return NULL;
}

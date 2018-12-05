
#include "afqueue.h"

AfQueue::AfQueue()
 : iRead(0), iWrite(0)
{
}

int AfQueue::size()
{
	int aux = iWrite - iRead;
	if(aux < 0) aux += BUF_TAM;
	return aux;
}

bool AfQueue::push(const void *e)
{
	int aux = iWrite - iRead;
	if((aux == -1) || (aux == (BUF_TAM - 1)))
	{
		return false;
	}
	data[iWrite] = e;
	iWrite = (iWrite + 1) % BUF_TAM;
	return true;
}

const void *AfQueue::pop()
{
	const void *aux = 0;
	if(iRead != iWrite)
	{
		aux = data[iRead];
		iRead = (iRead + 1) % BUF_TAM;
	}
	return aux;
}

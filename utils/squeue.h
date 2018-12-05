
#ifndef _SQUEUE_H_
#define _SQUEUE_H_

template<int TAM>
class SQueue
{
	int iRead, iWrite;
	const void *data[TAM];
	
public:
	SQueue() : iRead(0), iWrite(0) {};
	int size()
	{
		int aux = iWrite - iRead;
		if(aux < 0) aux += TAM;
		return aux;
	};
	bool push(const void *e)
	{
		int aux = iWrite - iRead;
		if((aux == -1) || (aux == (TAM - 1)))
		{
			return false;
		}
		data[iWrite] = e;
		iWrite = (iWrite + 1) % TAM;
		return true;
	};
	const void *pop()
	{
		const void *aux = 0;
		if(iRead != iWrite)
		{
			aux = data[iRead];
			iRead = (iRead + 1) % TAM;
		}
		return aux;
	};	
};

#endif /*SQUEUE_H_*/

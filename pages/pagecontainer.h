
#ifndef _PAGECONTAINER_H
#define _PAGECONTAINER_H

#include <set>
#include "squeue.h"
#include "page.h"

#define PQUEUE_TAM	(64*1024)
#define AFQUEUE_TAM	256

class PageContainer
{
	std::set<Page> pageTree;
	SQueue<PQUEUE_TAM> pageQueue;
	int qacum;
	
	int nAfinity;
	const Host **afHosts;
	SQueue<AFQUEUE_TAM> *afQueues;
	
	void tmpLoad();
	void tmpSave();
	
public:
	PageContainer();
	~PageContainer();
	void setAfinNum(int n);
	
	void addPage(const char *url, const Page *curp);
	const Page *getPage(int fid);
	
	int getAfqSize(int fid);
	void getStats(int *nq, int *nh, int *np);
	void printData(FILE *f = stdout);
	void loadData(char *fname);
};

#endif

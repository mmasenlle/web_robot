
#include <fstream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "conf.h"
#include "pagecontainer.h"


PageContainer::PageContainer()
 : qacum(0), nAfinity(0), afHosts(NULL), afQueues(NULL)
{
}

PageContainer::~PageContainer()
{
	if(nAfinity)
	{
		delete [] afHosts;
		delete [] afQueues;
	}
}

void PageContainer::setAfinNum(int n)
{
	if(!nAfinity)
	{
		nAfinity = n;
		afHosts = new const Host*[nAfinity];
		afQueues = new SQueue<AFQUEUE_TAM>[nAfinity];
		memset(afHosts, 0, nAfinity * sizeof(*afHosts));
	}
}

void PageContainer::addPage(const char *url, const Page *curp)
{
	std::string spath, shost;
	const Host *phost;
	
	if(Page::strip(url, curp, &spath, &shost))
	{
		Host auxhost = shost.c_str();
		phost = auxhost.getPHost();
	}
	else
	{
		phost = curp->getHost();
	}
	Page *ppag = new Page(spath.c_str(), spath.length(), phost);
	if(pageTree.find(*ppag) == pageTree.end())
	{
		qacum++;
		if(!pageQueue.push(ppag))
		{
			tmpSave();
			pageQueue.push(ppag);
		}
	}
	else
	{
		delete ppag;
	}
}

const Page *PageContainer::getPage(int fid)
{
	const Page *aux = (const Page *)afQueues[fid].pop();
	if(aux) return aux;
	for(;;)
	{
		bool pushed = false;
		if(!(aux = (const Page *)pageQueue.pop()))
		{
			tmpLoad();
			if(!(aux = (const Page *)pageQueue.pop()))
				return NULL;
		}		
		if(pageTree.find(*aux) != pageTree.end())
		{
			delete aux;
			continue;
		}
		pageTree.insert(*aux);
		for(int i = 0; i < nAfinity; i++)
		{
			if((aux->getHost() == afHosts[i]))
			{
				if(i == fid) return aux;
				if(afQueues[i].push(aux))
				{
					pushed = true;
					break;
				}
			}
		}
		if(!pushed)
		{
			afHosts[fid] = aux->getHost();
			return aux;
		}
	}
}

int PageContainer::getAfqSize(int fid)
{
	if(!nAfinity) return 0;
	return afQueues[fid].size();
}

void PageContainer::getStats(int *nq, int *nh, int *np)
{
	*nq = qacum;
	*nh = Host::hosts.size();
	*np = pageTree.size();
}

void PageContainer::tmpSave()
{
	FILE *f = fopen(TMP_FILE, (qacum < (2 * PQUEUE_TAM))?"w":"a");
	if(!f)
	{
		if(Conf::verbose) fprintf(stderr, "PageContainer::tmpSave -> Error "
				"opening tmp file %s\n", TMP_FILE);
		return;
	}
	const Page *page;
	while(page = (const Page *)pageQueue.pop())
	{
		page->print(f);
		delete page;
	}
	fclose(f);
}

void PageContainer::tmpLoad()
{
	if(qacum < PQUEUE_TAM) return;
	std::ifstream ifs(TMP_FILE);
	if(!ifs)
	{
		if(Conf::verbose) fprintf(stderr, "Error openning temporal data file %s\n", TMP_FILE);
		return;
	}
	std::string saux;
	std::getline(ifs, saux);
	while(std::getline(ifs, saux))
	{
		if(!saux.length()) continue;
		std::string shost, spath;
		int i = saux.find_first_of(' ');
		shost = saux.substr(i + 1);
		saux.erase(i);
		i = shost.find_first_of(' ');
		spath = shost.substr(i + 1);
		shost.erase(i);
		Host auxhost = shost.c_str();
		unsigned long crc = strtoul(saux.c_str(), NULL, 16);
		const Page *ppag = new Page(crc, auxhost.getPHost(), spath.c_str());
		if(pageTree.find(*ppag) == pageTree.end())
		{
			if(!pageQueue.push(ppag))
			{
				delete ppag;
				break;
			}
		}
	}
	if(saux.length())
	{
		std::ofstream ofs(TMP_FILE "_tmp");
		if(!ofs)
		{
			if(Conf::verbose) fprintf(stderr, "Error openning temporal data file %s\n", TMP_FILE);
			return;
		}
		ofs << saux;
		char buff[PQUEUE_TAM];
		do
		{
			ifs.read(buff, PQUEUE_TAM);
			ofs.write(buff, ifs.gcount());
		}
		while(ifs.gcount());
		ifs.close();
		ofs.close();
		if(unlink(TMP_FILE) != 0)
			if(Conf::verbose) perror("unlink " TMP_FILE);
		if(link(TMP_FILE "_tmp", TMP_FILE) != 0)
			if(Conf::verbose) perror("link " TMP_FILE);
		if(unlink(TMP_FILE "_tmp") != 0)
			if(Conf::verbose) perror("unlink " TMP_FILE "_tmp");
	}
	else
	{
		ifs.close();
		if(unlink(TMP_FILE) != 0)
			if(Conf::verbose) perror("unlink 2 " TMP_FILE);
	}
}

void PageContainer::printData(FILE *f)
{
	std::set<Page>::iterator it = pageTree.begin();
	for(; it != pageTree.end(); it++) (*it).print(f);
	fprintf(f, "\n");
	if(f != stdout)
	{
		const Page *page;
		for(int i = 0; i < nAfinity; i++)
		{
			while(page = (const Page *)afQueues[i].pop())
				page->print(f);
		}
		while(page = (const Page *)pageQueue.pop())
			page->print(f);

		FILE *f2 = fopen(TMP_FILE, "r");
		if(f2)
		{
			int n; char fbuf[PQUEUE_TAM];
			while((n = fread(fbuf, 1, PQUEUE_TAM, f2)) > 0)
				fwrite(fbuf, 1, n, f);
			fclose(f2);
		}
		fprintf(f, "\n");
	}
}

void PageContainer::loadData(char *fname)
{
	std::ifstream ifs(fname);
	if(!ifs)
	{
		fprintf(stderr, "Error openning data file %s\n", fname);
		return;
	}
	std::string saux;
	std::getline(ifs, saux);
	if(strcmp(saux.c_str(), DATA_FILE_TAG))
	{
		fprintf(stderr, "Data file format error %s\n", fname);
		return;
	}
	bool btree = true;
	while(std::getline(ifs, saux))
	{
		if(!saux.length())
		{
			btree = false;
			continue;
		}
		std::string shost, spath;
		int i = saux.find_first_of(' ');
		shost = saux.substr(i + 1);
		saux.erase(i);
		i = shost.find_first_of(' ');
		spath = shost.substr(i + 1);
		shost.erase(i);
		Host auxhost = shost.c_str();
		unsigned long crc = strtoul(saux.c_str(), NULL, 16);
		if(btree)
		{
			Page page(crc, auxhost.getPHost());
			pageTree.insert(page);
		}
		else
		{
			const Page *ppag = new Page(crc, auxhost.getPHost(), spath.c_str());
			if(pageTree.find(*ppag) == pageTree.end())
			{
				qacum++;
				if(!pageQueue.push(ppag))
				{
					tmpSave();
					pageQueue.push(ppag);
				}
			}
			else
			{
				delete ppag;
			}
		}
	}
}


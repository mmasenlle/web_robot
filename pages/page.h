
#ifndef _PAGE_H
#define _PAGE_H

#include <string>
#include "host.h"

class Page
{
	const Host *host;
	const char *path;
	unsigned long crc;
	
public:
	Page(const char *p, int l, const Host *h);
	Page(unsigned long c, const Host *h, const char *p = NULL);
	Page(const Page &page);
	~Page();
	bool operator<(const Page &page) const;
	static bool valid(const char *name, int len);
	static bool strip(const char *name, const Page *curp,
				std::string *spath, std::string *shost);
	
	const Host *getHost() const { return host; };
	const char *getPath() const { return path; };
	
	void print(FILE *f) const;
};

#endif


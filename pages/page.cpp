#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "conf.h"
#include "crc32.h"
#include "page.h"

Page::Page(const char *p, int l, const Host *h)
 : host(h)
{
	path = strdup(p);
	crc = crc32(p, l);
}

Page::Page(unsigned long c, const Host *h, const char *p)
 : crc(c), host(h), path(NULL)
{
	if(p) path = strdup(p);
}

Page::Page(const Page &page)
 : path(NULL), host(page.host), crc(page.crc)
{
}

Page::~Page()
{
	if(path) free(const_cast<char*>(path));
}

bool Page::operator<(const Page &page) const
{
	return ((crc == page.crc) ?
		(host < page.host) : (crc < page.crc));
}

bool Page::valid(const char *name, int len)
{
	if(len < 5) return true;
	
	const char *pos = name + len - 3;
	return (strncasecmp(pos, "css", 3) && strncasecmp(pos, "jpg", 3) &&
			strncasecmp(pos, "pdf", 3) && strncasecmp(pos, "ico", 3) &&
			strncasecmp(pos, "exe", 3) &&
			//...
	   strncasecmp(pos, "png", 3) && strncasecmp(pos, "gif", 3)	);
}

bool Page::strip(const char *name, const Page *curp, std::string *spath, std::string *shost)
{
	if(!strncmp(name, "http://", 7))
	{
		*shost = name + 7;
		*spath = "/";
		int i = shost->find_first_of("/?;:");
		if(i < shost->npos)
		{
			*spath = shost->substr(i);
			shost->erase(i);
		}
		return true;
	}
	else if(!curp)
	{
		*shost = name;
		*spath = "/";
		if(Conf::verbose) fprintf(stderr, "Page::strip -> url incorrecta %s\n", name);
		return true;
	}
	else
	{
		if(name[0] == '/')
		{
			*spath = name;
		}
		else
		{
			*spath = curp->getPath();
			spath->erase(spath->find_last_of('/') + 1);
			if(!strncmp(name, "./", 2))
				name += 2;
			while(!strncmp(name, "../", 3))
			{
				name += 3;
				spath->erase(spath->find_last_not_of('/') + 1);
				spath->erase(spath->find_last_of('/') + 1);				
			}
			if(!strcmp(name, ".")) name = "";
			else if(!strcmp(name, ".."))
			{
				name = "";
				spath->erase(spath->find_last_not_of('/') + 1);
				spath->erase(spath->find_last_of('/') + 1);	
			}
			*spath += name;
		}
		return false;
	}
}

void Page::print(FILE *f) const
{
	fprintf(f, "%x %s %s\n", crc, host->getName(), path? path:"<>");
}


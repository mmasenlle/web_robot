#include <string.h>
#include <stdlib.h>
#include "host.h"

enum { 
	WWW_PREFIX = 1,
	
	MAX_PREFIX
};
/*
static const char *pref_val[MAX_PREFIX] = {
	"",		//0
	"www.",	//1
};
*/
std::set<Host> Host::hosts;

Host::Host(const char *path)
 : name(path), prefix(0), ip(0), dup(false)
{
	if(*((unsigned long*)name) == '.www')
	{
		prefix = WWW_PREFIX;
	}
}

Host::Host(const Host &host)
 : dup(true), prefix(host.prefix), ip(host.ip)
{
	name = strdup(host.name);
}

Host::~Host()
{
	if(dup && name)
		free(const_cast<char*>(name));
}

bool Host::operator<(const Host &host) const
{
	return ((prefix == host.prefix) ?
		(strcmp(name + 4, host.name + 4) < 0) : (prefix < host.prefix));
}

const Host *Host::getPHost()
{
	std::set<Host>::iterator it = hosts.find(*this);
	if(it == hosts.end())
	{
		it = hosts.insert(hosts.begin(), *this);
	}
	return (*it).getThis();
}

void Host::setIp(unsigned long nip) const
{
	if(!ip)
		ip = nip;
}

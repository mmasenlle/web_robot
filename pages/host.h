
#ifndef _HOST_H
#define _HOST_H

#include <set>

class Host
{
	const char *name;
	int prefix;
	mutable unsigned long ip;
	bool dup;
	
public:
	static std::set<Host> hosts;
	
	Host(const char *path);
	Host(const Host &host);
	~Host();
	bool operator<(const Host &host) const;
	const Host *getPHost();
	const Host *getThis() const { return this; };
	const char *getName() const { return name; };
	unsigned long getIp() const { return ip; };
	void setIp(unsigned long nip) const;
};

#endif



#ifndef SELECTABLE_H
#define SELECTABLE_H

class Selectable
{
public:
	virtual int getFd() =0;
	virtual int getFlags() { return 1; };
	virtual void go() =0;
};

#endif


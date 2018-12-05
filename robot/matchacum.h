
#ifndef MATCHACUM_H_
#define MATCHACUM_H_

#include "page.h"

#define MATCHES_BUF_TAM 510

struct match_t
{
	const char *word;
	char *pageBuf;
};

class MatchAcum
{
	int idx;
	int acum;
	match_t matches[MATCHES_BUF_TAM];

public:
	MatchAcum();
	void put(const char *w, const Page *p);
	void release();
	int getAcum();
};

#endif /*MATCHACUM_H_*/

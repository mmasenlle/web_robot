
#ifndef _AUTOMATA_H
#define _AUTOMATA_H

#include "pagecontainer.h"

#define AUTOM_BUF_LEN	512

class LinkAutom
{
public:
	int i, state;
	const Page *curPage;
	PageContainer &pages;
	char buf[AUTOM_BUF_LEN];
	
	LinkAutom(PageContainer &pc) : state(0), i(0), pages(pc) {};
	void reset() { state = 0; i = 0; };
	void setPage(const Page *p) { curPage = p; reset(); };
};

#define LOWER(c) (c | 0x020)

#define LinkAutom_PROCESS(la, c) do { \
	switch(la.state) { \
	case 0: /* inicial */ \
		if(LOWER(c) == 'h') la.state++; break; \
	case 1: /* h */ \
		la.state = (LOWER(c) == 'r') ? 2 : 0; break; \
	case 2: /* hr */ \
		la.state = (LOWER(c) == 'e') ? 3 : 0; break; \
	case 3: /* hre */ \
		la.state = (LOWER(c) == 'f') ? 4 : 0; break; \
	case 4: /* href */ \
		la.state = (c == '=') ? 5 : 0; break; \
	case 5: /* href= */ \
		la.state = (c == '\"') ? 6 : 0; break; \
	case 6: /* href=" */ \
		if(c == '\"' || c == '#') \
		{ \
			la.buf[la.i] = '\0'; \
			if(Page::valid(la.buf, la.i)) \
				la.pages.addPage(la.buf, la.curPage); \
			la.reset(); \
		} \
		else if(c == '<' || c == '\n' || (c == ':' && la.i != 4)) \
		{ \
			la.reset(); \
		} \
		else \
		{ \
			la.buf[la.i++] = c; \
			if(la.i == AUTOM_BUF_LEN - 1) \
				la.reset(); \
		} \
	} \
}while(0)

#endif

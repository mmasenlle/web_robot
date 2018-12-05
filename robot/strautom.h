#ifndef STRAUTOM_H_
#define STRAUTOM_H_

#include "conf.h"
#include "matchacum.h"
#include "page.h"

#define MAX_WORDS 512
#define MAX_STATES 64
struct state_t
{
	int state;
	int word;	
};

struct firstch_t
{
	char ch;
	short jump;
};

class StrAutom
{
public:
	static int nWords;
	static firstch_t firstChar[MAX_WORDS];
	static char *words[MAX_WORDS];
	const Page *curPage;
	int lastv;
	state_t states[MAX_STATES];
	
	static int compWords(const void* a, const void* b);

	static MatchAcum matchAcum;
	static void loadStrs(char *fname);
	static void setWords(char **w, int n);
	static int dicoSort(char **first, char **last, int pos);
	StrAutom();
	void setPage(const Page *p) { curPage = p; lastv = 0; };
};

#define StrAutom_PROCESS(sa, c)	do { \
	int newlastv = 0; \
	for(int i = 0; i < sa.lastv; i++) \
	{ \
		if(sa.states[i].state) \
		{ \
			if(sa.words[sa.states[i].word][sa.states[i].state] == c) \
			{ \
				sa.states[i].state++; \
				newlastv = i + 1; \
				continue; \
			} \
			if(sa.words[sa.states[i].word][sa.states[i].state] == '\0') \
			{ \
				sa.matchAcum.put(sa.words[sa.states[i].word], sa.curPage); \
			} \
			sa.states[i].state = 0; \
		} \
	} \
	int j = 0; \
	char lc = 126; \
	for(int i = 0; i < sa.nWords; ) \
	{ \
		if(c > sa.firstChar[i].ch) \
		{ \
			if(sa.firstChar[i].jump == sa.nWords) break; \
			i += sa.firstChar[i].jump; \
			continue; \
		} \
		else if(c == sa.firstChar[i].ch) \
		{ \
			while(sa.states[j].state && (j < MAX_STATES)) j++; \
			if(j == MAX_STATES) break; \
			sa.states[j].word = i; \
			sa.states[j].state = 1; \
			if(newlastv == j) newlastv++; \
		} \
		else if(lc < sa.firstChar[i].ch) \
			break; \
		lc = sa.firstChar[i].ch; \
		i++; \
	} \
	sa.lastv = newlastv; \
} while(0)

#endif /*STRAUTOM_H_*/

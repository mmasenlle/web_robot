#include <string.h>
#include <stdlib.h>
#include <fstream>
#include "strautom.h"

int StrAutom::nWords = 0;
firstch_t StrAutom::firstChar[MAX_WORDS];
char *StrAutom::words[MAX_WORDS];
MatchAcum StrAutom::matchAcum;

void StrAutom::loadStrs(char *fname)
{
	std::ifstream ifs(fname);
	if(!ifs)
	{
		fprintf(stderr, "Error openning input file '%s'\n", fname);
		return;
	}
	std::string aux;
	while(std::getline(ifs, aux))
	{
		if(nWords == MAX_WORDS)
		{
			fprintf(stderr, "Maximun number of strings reached\n");
			return;
		}
		words[nWords] = strdup(aux.c_str());
		nWords++;
	}
}

void StrAutom::setWords(char **w, int n)
{
	for(int i = 0; i < n; i++)
	{
		if(nWords == MAX_WORDS)
		{
			fprintf(stderr, "Maximun number of strings reached\n");
			return;
		}
		words[nWords] = strdup(w[i]);
		nWords++;
	}
	qsort(words, nWords, sizeof(char*), StrAutom::compWords);
	char *auxWords[nWords];
	memcpy(auxWords, words, sizeof(char*) * nWords);
	dicoSort(auxWords, auxWords + nWords, 0);
	for(int i = 0; i < nWords; i++)
	{
		firstChar[i].ch = words[i][0];
	}
}

int StrAutom::dicoSort(char **first, char **last, int pos)
{
	if(first == last) return pos;
	int middle = (last - first) / 2;
	char c = **(first + middle);
	char **lfirst, **flast;
	while(middle && (**(first + middle - 1) == c)) middle--;
	lfirst = first + middle;
	int auxpos = pos;
	while((middle + first < last) && (**(first + middle) == c))
		words[pos++] = *(first + middle++);
	flast = first + middle;
	int nextpos = dicoSort(first, lfirst, pos);
	while(auxpos < pos)
		firstChar[auxpos++].jump = nextpos;
	return dicoSort(flast, last, nextpos);
}

int StrAutom::compWords(const void* a, const void* b)
{
   const char **aa = (const char**)a;
   const char **bb = (const char**)b;
   return ((**aa < **bb)? (-1) : ((**aa > **bb)? 1 : 0));
}

StrAutom::StrAutom()
: curPage(NULL), lastv(0)
{
	memset(states, 0, MAX_STATES * sizeof(state_t));
}

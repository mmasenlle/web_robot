
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include "conf.h"
#include "matchacum.h"

MatchAcum::MatchAcum()
: idx(0), acum(0)
{
}

void MatchAcum::put(const char *w, const Page *p)
{
	if(idx == MATCHES_BUF_TAM)
	{
		release();
	}
	std::string strBuf = p->getHost()->getName();
	strBuf += p->getPath();
	matches[idx].word = w;
	matches[idx].pageBuf = strdup(strBuf.c_str());
	idx++;
	acum++;
	
	if(Conf::verbose) fprintf(stderr, "Match -> %s host: %s path: %s\n",
		w, p->getHost()->getName(), p->getPath());
}

void MatchAcum::release()
{
	if(Conf::resFile)
	{
		FILE *f;
		if((f = fopen(Conf::resFile, "a")))
		{
			for(int i = 0; i < idx; i++)
			{
				fprintf(f, "<b>%s</b>&nbsp;&nbsp;<a href=\"http://%s\">%s</a><br />\n",
					matches[i].word, matches[i].pageBuf, matches[i].pageBuf);
				free(matches[i].pageBuf);
			}
			fclose(f);
		}
	}
	idx = 0;
}

int MatchAcum::getAcum()
{
	return acum;	
}

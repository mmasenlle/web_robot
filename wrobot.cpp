 
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "pollable.h"
#include "strautom.h"
#include "robot.h"
#include "conf.h"

int Conf::timeout = 500;
int Conf::verbose = 0;
char *Conf::resFile = NULL;

void usage()
{
	printf("\nwrobot:\tSearch strings in the web\n");
	printf("\nUsage:\twrobot [-v] [-n <num>] [-t <timeout>] [-d <file>] [-i <file>] [-o <file>] "
			"\n\t[<word 1> <word 2> ...] [-p <url 1> [<url 2> ...]]\n");
	printf("\t-h\tbrief help\n");
	printf("\t-v\tactive verbose\n");
	printf("\t-n\tnum finder threads\n");
	printf("\t-t\tconnect timeout (ms)\n");
	printf("\t-d\tload data from file\n");
	printf("\t-i\tinput string file\n");
	printf("\t-o\toutput file name\n");
	printf("\t-p\tseed pages\n");
	
	printf("\n");
	exit(-1);
}

void printHelp()
{
	printf("\nOPTION MENU:\n");
	printf("< h >\tthis help\n");
	printf("< p >\tpause\n");
	printf("< c >\tcontinue\n");
	printf("< f >\tprint finders state\n");
	printf("< s >\tsave state\n");
	printf("< d >\tdump tree data\n");
	printf("< q >\texit\n");
	printf("<other>\tprint stats\n\n");
}

void sighandler(int i)
{
	fprintf(stderr, "wrobot -> %d signal received\n", i);
	if(i != SIGPIPE) exit(-1);
}

class WRobot : public Pollable
{
	Robot &robot;
public:
	WRobot(Robot &r) : robot(r) {};
	void go()
	{
		switch(getchar())
		{
		case 'h':	getchar(); printHelp(); break;
		case 'p':	getchar(); robot.stop(); break;
		case 'c':	getchar(); robot.start(); break;
		case 'f':	getchar(); robot.printState(); break;
		case 's':	getchar(); robot.save(); break;
		case 'd':	getchar(); robot.printData(); break;
		case 'q':	exit(0);
			
		case '\n':	robot.printStats(); break;
		default:	getchar(); robot.printStats();
		}		
		fprintf(stdout, "Option: "); fflush(stdout);
	};
};

int main(int argc, char *argv[])
{
	char **urls = NULL, **words = NULL, **dfiles = NULL;
	int nwords = 0, nurls = 0, ndfiles = 0, nfinders = 8;
	
	for(int i = 1; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			switch(argv[i][1])
			{
			case 'v': Conf::verbose = 1; continue;
			case 'n': if(++i == argc) usage();
				nfinders = atoi(argv[i]); continue;
			case 't': if(++i == argc) usage();
				Conf::timeout = atoi(argv[i]); continue;
			case 'd': if(++i == argc) usage();
				dfiles = argv + i; ndfiles = 1; continue;
			case 'i': if(++i == argc) usage();
				StrAutom::loadStrs(argv[i]); continue;
			case 'o': if(++i == argc) usage();
				Conf::resFile = argv[i]; continue;
			case 'p': if(++i == argc) usage();
				urls = argv + i; nurls = 1; continue;
			default: usage();
			}
		}
		else
		{
			if(urls)
			{
				nurls++;
			}
			else if(dfiles)
			{
				ndfiles++;
			}
			else
			{
				if(!words) words = argv + i;
				nwords++;
			}
		}
	}
	
	for(int i = 0; i < 16; i++)
		signal(i, (sighandler_t)sighandler);
	
	StrAutom::setWords(words, nwords);
	Robot robot(nfinders);
	
	if(!nurls && !dfiles) robot.addPage("http://www.google.es");
	else for(int i = 0; i < nurls; i++)
	{
		robot.addPage(urls[i]);
	}
	if(Conf::resFile)
	{
		FILE *f;
		if((f = fopen(Conf::resFile, "a")))
		{
			time_t tt; time(&tt);
			fprintf(f, "<h3>Results: ( %s)</h3>\n", asctime(localtime(&tt)));
			fclose(f);
		}
		else
		{
			fprintf(stderr, "Error abriendo fichero salida %s\n", Conf::resFile);
			Conf::resFile = NULL;
		}
	}
	if(dfiles) robot.load(dfiles, ndfiles);

	fprintf(stdout, "Option: "); fflush(stdout);
	WRobot wrobot(robot);
	wrobot.setnblock(0);
	robot.init(&wrobot);
}

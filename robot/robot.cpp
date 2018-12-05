#include <stdio.h>
#include "conf.h"
#include "strautom.h"
#include "robot.h"


Robot::Robot(int nfinders)
 : tb(0), numFinders(nfinders), started(false), poller(nfinders + 1)
{
	gettimeofday(&ttv, NULL);
	finders = new Finder[numFinders];
	Finder::pages.setAfinNum(numFinders);
}

Robot::~Robot()
{
	if(started) stop();
	delete [] finders;
}

void Robot::init(Pollable *p)
{
	gettimeofday(&etv, NULL);
	Finder::resolver.start(numFinders);
	if(p) poller.addPollable(p);
	for(int i = 0; i < numFinders; i++)
	{
		poller.addPollable(&finders[i]);
		finders[i].start(i);
	}
	started = true;
	poller.go();
}

void Robot::start()
{
	if(started) return;
	for(int i = 0; i < numFinders; i++)
	{
		finders[i].start(i);
	}
	started = true;
}

void Robot::stop()
{
	if(!started) return;
	for(int i = 0; i < numFinders; i++)
	{
		finders[i].stop();
	}
	started = false;
	StrAutom::matchAcum.release();
}

void Robot::load(char **dfiles, int ndfiles)
{
	for(int i = 0; i < ndfiles; i++)
		Finder::pages.loadData(dfiles[i]);
}

void Robot::save()
{
	stop();
	FILE *f = fopen(SAVE_FILE, "w");
	if(f)
	{
		printf("\nSaving data to %s\n", SAVE_FILE);
		fprintf(f, "%s\n", DATA_FILE_TAG);
		for(int i = 0; i < numFinders; i++)
		{
			finders[i].print_curPages(f);
		}
		Finder::pages.printData(f);
		fclose(f);
		printf("Data saved successfully\n");
	}
	else
	{
		printf("\nError opening %s\n", SAVE_FILE);
	}		
}

void Robot::addPage(const char *url)
{
	Finder::pages.addPage(url, NULL);
}

void Robot::printStats()
{
	int nq, nh, np;
	printf("\nWord matches: %d\n", StrAutom::matchAcum.getAcum());
	Finder::pages.getStats(&nq, &nh, &np);
	printf("Processed pages: %d\n", np);
	printf("Queued pages: %d\n", nq);
	printf("Total hosts: %d\n", nh);
	unsigned long long var = 0;
	double et = (etv.tv_sec * 1000.0) + (etv.tv_usec / 1000.0);
	gettimeofday(&etv, NULL);
	et = ((etv.tv_sec * 1000.0) + (etv.tv_usec / 1000.0)) - et;
	for(int i = 0; i < numFinders; i++)
		var += finders[i].getAcum();
	tb += var;
	int ss = etv.tv_sec - ttv.tv_sec;
	printf("Elapsed time: %dh%dm%ds\n", ss/3600, (ss%3600)/60, ss%60);
	printf("Total processed: %0.2fMB\n", tb / (1024.0*1024.0));
	printf("Download rate: %0.2fkB/s\n\n", var / et);
}

void Robot::printState()
{
	printf("\n%s timeout: %d\n", started?"STARTED":"STOPED", Conf::timeout);
	for(int i = 0; i < numFinders; i++)
	{
		finders[i].print_state();
	}
	printf("\n\n");
}

void Robot::printData()
{
	Finder::pages.printData();
}

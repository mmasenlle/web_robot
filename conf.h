#ifndef CONF_H_
#define CONF_H_

#define	TMP_FILE	".tmp_robot.dat"
#define SAVE_FILE "robot.dat"
#define DATA_FILE_TAG "WROBOT DATA FILE v0.0"

struct Conf
{
	static int timeout;
	static int verbose;
	static char *resFile;
};

#endif /*CONF_H_*/

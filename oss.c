#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int errno;
char errmsg[100];
int shmidTime;
int shmidMsg;
/* Insert other shmid values here */

void sigIntHandler(int signum)
{
	snprintf(errmsg, sizeof(errmsg), "MASTER: Caught SIGINT! Killing all child processes.");
	perror(errmsg);	
	
	/* errno = shmdt(shmTime);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "MASTER: shmdt(shmTime)");
		perror(errmsg);	
	}
	
	errno = shmctl(shmidTime, IPC_RMID, NULL);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "MASTER: shmctl(shmidTime)");
		perror(errmsg);	
	}

	errno = shmdt(shmMsg);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "MASTER: shmdt(shmMsg)");
		perror(errmsg);	
	}
	
	errno = shmctl(shmidMsg, IPC_RMID, NULL);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "MASTER: shmctl(shmidMsg)");
		perror(errmsg);	
	}*/
	exit(signum);
}

int main (int argc, char *argv[]) {
int o;
char *sParam = NULL;
char *lParam = NULL;
char *tParam = NULL;
int i;
pid_t pid[20] = {getpid()};
char timeArg[33];
char msgArg[33];
key_t keyTime = 8675;
key_t keyMsg = 1138;
FILE *fp;
signal(SIGINT, sigIntHandler);


/* Options */
while ((o = getopt (argc, argv, "hs:l:t:")) != -1)
{
    switch (o)
    {
		case 'h':
			perror("HELP MESSAGE\n");
			exit(1);
			break;
		case 's':
			sParam = optarg;
			break;
		case 'l':
			lParam = optarg;
			break;
		case 't':
			tParam = optarg;
		case '?':
			if (optopt == 's' || optopt == 'l' || optopt == 't')
			{
				snprintf(errmsg, sizeof(errmsg), "OSS: Option -%c requires an argument.", optopt);
				perror(errmsg);
			}
			return 1;
		default:
			break;
    }
}
printf("sParam = %s, lParam = %s, tParam = %s\n", sParam, lParam, tParam);
	
return 0;
}

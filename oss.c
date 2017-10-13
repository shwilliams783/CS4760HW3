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
int i;
int maxSlaves = 5;
int numSlaves = 0;
int maxTime = 20;
char *sParam = NULL;
char *lParam = NULL;
char *tParam = NULL;
char timeArg[33];
char msgArg[33];
pid_t pid[20] = {getpid()};
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
			break;
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
if(sParam != NULL)
{
	maxSlaves = atoi(sParam);
}
if(lParam != NULL)
{
	/* File OPEN */
}
if(tParam != NULL)
{
	maxTime = atoi(tParam);
}
printf("sParam = %d, tParam = %d\n", maxSlaves, maxTime);

/* Fork off child processes */
for(i = 0; i < maxSlaves+1; i++)
{
	if(pid[i] != 0)
	{
		pid[i+1] = fork();
	}
	else if(pid[i] == 0)
	{
		/* snprintf(idArg, 10, "%d", index); */
		/* snprintf(indexArg, 10, "%d", ((index-1)*5)); */
		execl("./user", "user", (char*)0);
	}
}
	
return 0;
}
























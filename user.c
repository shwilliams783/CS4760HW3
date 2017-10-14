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

struct timer
{
	int seconds;
	int ns;
};

struct message
{
	pid_t pid;
	int seconds;
	int ns;
};

int errno;
pid_t pid;
char errmsg[100];
struct timer *shmTime;
struct message *shmMsg;
/* Insert other shmid values here */

void sigIntHandler(int signum)
{
	
	snprintf(errmsg, sizeof(errmsg), "USER %d: Caught SIGINT! Killing all child processes.", pid);
	perror(errmsg);	
	
	errno = shmdt(shmTime);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "MASTER: shmdt(shmTime)");
		perror(errmsg);	
	}

	errno = shmdt(shmMsg);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "MASTER: shmdt(shmMsg)");
		perror(errmsg);	
	}
	exit(signum);
}

int main (int argc, char *argv[]) {
int o;
int i;
int timeKey = atoi(argv[1]);
int msgKey = atoi(argv[2]);
key_t keyTime = 8675;
key_t keyMsg = 1138;
signal(SIGINT, sigIntHandler);
pid = getpid();

snprintf(errmsg, sizeof(errmsg), "USER %d: Slave process started!", pid);
perror(errmsg);

/********************MEMORY ATTACHMENT********************/
/* Point shmTime to shared memory */
shmTime = shmat(timeKey, NULL, 0);
if ((void *)shmTime == (void *)-1)
{
	snprintf(errmsg, sizeof(errmsg), "USER: shmat(shmidTime)");
	perror(errmsg);
    exit(1);
}

/* Point shmTime to shared memory */
shmMsg = shmat(msgKey, NULL, 0);
if ((void *)shmMsg == (void *)-1)
{
	snprintf(errmsg, sizeof(errmsg), "USER: shmat(shmidMsg)");
	perror(errmsg);
    exit(1);
}
/********************END ATTACHMENT********************/

/* snprintf(errmsg, sizeof(errmsg), "USER %d: shmTime->seconds = %d shmTime->ns = %d\n", pid, shmTime->seconds, shmTime->ns);
perror(errmsg);
snprintf(errmsg, sizeof(errmsg), "USER %d: shmMsg->pid = %d shmMsg->seconds = %d shmMsg->ns = %d\n", pid, shmMsg->pid, shmMsg->seconds, shmMsg->ns);
perror(errmsg); */

/********************MEMORY DETACHMENT********************/
errno = shmdt(shmTime);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "MASTER: shmdt(shmTime)");
		perror(errmsg);	
	}

	errno = shmdt(shmMsg);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "MASTER: shmdt(shmMsg)");
		perror(errmsg);	
	}
/********************END DETACHMENT********************/

return 0;
}

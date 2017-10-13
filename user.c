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
pid_t pid;
char errmsg[100];
int shmidTime;
int shmidMsg;
/* Insert other shmid values here */

void sigIntHandler(int signum)
{
	
	snprintf(errmsg, sizeof(errmsg), "USER %d: Caught SIGINT! Killing all child processes.", pid);
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
key_t keyTime = 8675;
key_t keyMsg = 1138;
signal(SIGINT, sigIntHandler);
pid = getpid();

snprintf(errmsg, sizeof(errmsg), "USER %d: Slave process started!.", pid);
perror(errmsg);

return 0;
}

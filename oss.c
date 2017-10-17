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
#include <semaphore.h>
#include <fcntl.h>

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
char errmsg[100];
int shmidTime;
int shmidMsg;
struct timer *shmTime;
struct message *shmMsg;
sem_t * sem;
/* Insert other shmid values here */


void sigIntHandler(int signum)
{
	/* Send a message to stderr */
	snprintf(errmsg, sizeof(errmsg), "OSS: Caught SIGINT! Killing all child processes.");
	perror(errmsg);	
	
	/* Deallocate shared memory */
	errno = shmdt(shmTime);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "OSS: shmdt(shmTime)");
		perror(errmsg);	
	}
	
	errno = shmctl(shmidTime, IPC_RMID, NULL);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "OSS: shmctl(shmidTime)");
		perror(errmsg);	
	}

	errno = shmdt(shmMsg);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "OSS: shmdt(shmMsg)");
		perror(errmsg);	
	}
	
	errno = shmctl(shmidMsg, IPC_RMID, NULL);
	if(errno == -1)
	{
		snprintf(errmsg, sizeof(errmsg), "OSS: shmctl(shmidMsg)");
		perror(errmsg);	
	}
	
	/* Close Semaphore */
	sem_unlink("pSem");   
    sem_close(sem);  

	/* Exit program */
	exit(signum);
}

int main (int argc, char *argv[]) {
int o;
int i;
int maxSlaves = 5;
int numSlaves = 0;
int numProc = 0;
int maxTime = 20;
char *sParam = NULL;
char *lParam = NULL;
char *tParam = NULL;
char timeArg[33];
char msgArg[33];
pid_t pid[20] = {getpid()};
pid_t myPid;
key_t keyTime = 8675;
key_t keyMsg = 1138;
FILE *fp;
char *fileName = "./msglog.out";
signal(SIGINT, sigIntHandler);
time_t start;
time_t stop;



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

/* Set maximum number of slave processes */
if(sParam != NULL)
{
	maxSlaves = atoi(sParam);
}

/* Set name of log file */
if(lParam != NULL)
{
	fp = fopen(lParam, "a");
	if(fp == NULL)
	{
		snprintf(errmsg, sizeof(errmsg), "OSS: fopen(lParam).");
		perror(errmsg);
	}
}
else
{
	fp = fopen(fileName, "a");
	if(fp == NULL)
	{
		snprintf(errmsg, sizeof(errmsg), "OSS: fopen(fileName).");
		perror(errmsg);
	}
}

/* Set maximum alloted run time in real time seconds */
if(tParam != NULL)
{
	maxTime = atoi(tParam);
}

/********************MEMORY ALLOCATION********************/
/* Create shared memory segment for a struct timer */
shmidTime = shmget(keyTime, sizeof(struct timer), IPC_CREAT | 0666);
if (shmidTime < 0)
{
	snprintf(errmsg, sizeof(errmsg), "OSS: shmget(keyTime...)");
	perror(errmsg);
	exit(1);
}

/* Point shmTime to shared memory */
shmTime = shmat(shmidTime, NULL, 0);
if ((void *)shmTime == (void *)-1)
{
	snprintf(errmsg, sizeof(errmsg), "OSS: shmat(shmidTime)");
	perror(errmsg);
    exit(1);
}

/* Create shared memory segment for a struct message */
shmidMsg = shmget(keyMsg, sizeof(struct message), IPC_CREAT | 0666);
if (shmidMsg < 0)
{
	snprintf(errmsg, sizeof(errmsg), "OSS: shmget(keyMsg...)");
	perror(errmsg);
	exit(1);
}

/* Point shmTime to shared memory */
shmMsg = shmat(shmidMsg, NULL, 0);
if ((void *)shmMsg == (void *)-1)
{
	snprintf(errmsg, sizeof(errmsg), "OSS: shmat(shmidMsg)");
	perror(errmsg);
    exit(1);
}
/********************END ALLOCATION********************/

/********************INITIALIZATION********************/
/* Convert shmTime and shmMsg keys into strings for EXEC parameters */
sprintf(timeArg, "%d", shmidTime);
sprintf(msgArg, "%d", shmidMsg);


/* Set the time to 00.00 */
shmTime->seconds = 0;
shmTime->ns = 0;
/* printf("shmTime->seconds = %d shmTime->ns = %d\n", shmTime->seconds, shmTime->ns); */

/* Set the message to 0, 00.00 */
shmMsg->pid = 0;
shmMsg->seconds = 0;
shmMsg->ns = 0;
/* printf("shmMsg->pid = %d shmMsg->seconds = %d shmMsg->ns = %d\n", shmMsg->pid, shmMsg->seconds, shmMsg->ns); */
/********************END INITIALIZATION********************/

/********************SEMAPHORE CREATION********************/
/* Open Semaphore */
sem=sem_open("pSem", O_CREAT | O_EXCL, 0644, 1);
if(sem == SEM_FAILED) {
	snprintf(errmsg, sizeof(errmsg), "OSS: sem_open(pSem)...");
	perror(errmsg);
    exit(1);
}
/********************END SEMAPHORE CREATION********************/

/* Fork off child processes */
for(i = 0; i < maxSlaves+1; i++)
{
	if(pid[i] != 0 && i < maxSlaves)
	{
		pid[i+1] = fork();
		numSlaves++;
		numProc++;
		continue;
	}
	else if(pid[i] == 0)
	{
		execl("./user", "user", timeArg, msgArg, (char*)0);
	}
}

/* Start the timer */
start = time(NULL);
do
{
	if(shmMsg->pid != 0) /* Do we need a semaphore for this? */
	{
		snprintf(errmsg, sizeof(errmsg), "OSS: Child %d is terminating at my time %02d.%d because it reached %02d.%d in slave\n", shmMsg->pid, shmTime->seconds, shmTime->ns, shmMsg->seconds, shmMsg->ns);
		fprintf(fp, errmsg);
		shmMsg->ns = 0;
		shmMsg->seconds = 0;
		shmMsg->pid = 0;
		/* increment total processes completed */
		/* spawn new process */
	}
	shmTime->ns += rand()%1000;
	if(shmTime->ns >= 1000000000)
	{
		shmTime->ns -= 1000000000;
		shmTime->seconds += 1;
	}
	/* snprintf(errmsg, sizeof(errmsg), "OSS: SS = %02d NS = %d", shmTime->seconds, shmTime->ns);
	perror(errmsg); */
	stop = time(NULL);
}while(stop-start < maxTime && shmTime->seconds < 2);

/* Kill all slave processes */
for(i = 1; i <= maxSlaves; i++)
{
	/* printf("Killing process #%d\n", pid[i]); */
	kill(pid[i], SIGINT);
}

/********************DEALLOCATE MEMORY********************/
errno = shmdt(shmTime);
if(errno == -1)
{
	snprintf(errmsg, sizeof(errmsg), "OSS: shmdt(shmTime)");
	perror(errmsg);	
}

errno = shmctl(shmidTime, IPC_RMID, NULL);
if(errno == -1)
{
	snprintf(errmsg, sizeof(errmsg), "OSS: shmctl(shmidTime)");
	perror(errmsg);	
}

errno = shmdt(shmMsg);
if(errno == -1)
{
	snprintf(errmsg, sizeof(errmsg), "OSS: shmdt(shmMsg)");
	perror(errmsg);	
}

errno = shmctl(shmidMsg, IPC_RMID, NULL);
if(errno == -1)
{
	snprintf(errmsg, sizeof(errmsg), "OSS: shmctl(shmidMsg)");
	perror(errmsg);	
}
/********************END DEALLOCATION********************/

/* Close Semaphore */
sem_unlink("pSem");   
sem_close(sem);  

return 0;
}

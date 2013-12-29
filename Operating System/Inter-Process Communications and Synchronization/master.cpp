//============================================================================
// Author      : Shubham Jain
// Email       : sjain39@uic.edu
// NetID       : sjain39
// Course      : CS385
// Description : HW2 Assignment
//============================================================================

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define STDIN 0
#define STDOUT 1

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
/* union semun is defined by including <sys/sem.h> */
#else
/* according to X/OPEN we have to define it ourselves */
union semun {
    int val;                /* value for SETVAL */
    struct semid_ds *buf;   /* buffer for IPC_STATE, IPC_SET */
    unsigned short *array;  /* array for GETALL, SETALL */
    //Linux specific part:
    struct seminfo *__buf;  /* buffer for IPC_INFO */
};
#endif

struct msgbuffer{
	long int mtype;
	char mtext[100];
} msg;

int countSetBits(int);
void checkInput(int, int, float, float);
int checkPrime(int);

int main(int argc, char** argv)
{
	printf("\n[HW2] IPC and Synchronization\nName: Shubham Jain\nEmail: sjain39@uic.edu\nNetID: sjain39\n-------------------------------\n");
	if(argc < 5 || argc > 7)
	{
		printf("Usage:\nmaster nBuffers nWorkers sleepMin sleepMax [ randSeed ] [ -lock | -nolock ]\n");
		exit(-1);
	}
	int nBuffers=0, nWorkers=0, randseed=0, i=0, j=0, childStatus=0, workerStatus=0, semID=-1;
	nBuffers = atoi(argv[1]);
	nWorkers = atoi(argv[2]);
	float sleepMin, sleepMax;
	sleepMin = atof(argv[3]);
	sleepMax = atof(argv[4]);
	
	checkInput(nBuffers, nWorkers, sleepMin, sleepMax);
	if(argc==6)
	{
		if(!strcmp(argv[5],"-lock") || !strcmp(argv[5],"lock"))
		{
			printf("Implementing semaphores...\n");
			if((semID = semget(IPC_PRIVATE, nBuffers, IPC_CREAT | 0600)) < 0)
			{
				perror("semget() failed in master");
				exit(-1);
			}
		}
		else
			randseed = atoi(argv[5]);
	}
	if(argc==7)
	{
		randseed = atoi(argv[5]);
		if(!strcmp(argv[6],"-lock") || !strcmp(argv[6],"lock"))
		{
			 printf("Implementing semaphores...\n");
			 if((semID = semget(IPC_PRIVATE, nBuffers, IPC_CREAT | 0600)) < 0)
			 {
				 perror("semget() failed in master");
				 exit(-1);
			 }
		}
	}
		

	int shmid=0, msgQue=0, *shm=NULL;

	if ((shmid = shmget(IPC_PRIVATE, nBuffers*sizeof(int), IPC_CREAT | 0600)) < 0)
	{
		perror("shmget() error");
		exit(-1);
	}

	if((msgQue = msgget(IPC_PRIVATE, IPC_CREAT | 0600)) < 0)
	{
		perror("msgget failed");
		exit(-1);
	}

	char *sort_args[] = {"sort", "-nr", NULL};

	int pipe1[2]; //toChild
	int pipe2[2]; //toParent

	if(pipe(pipe1) || pipe(pipe2))
	{
		perror("pipe() failed");
		exit(-1);
	}

	pid_t pid1 = fork();

	if(pid1 < 0)
	{
		perror("fork() failed for sort");
		exit(-1);
	}
	if(pid1 == 0)
	{ 
		close(pipe1[1]);
		close(pipe2[0]);

		dup2(pipe1[0], STDIN);
		dup2(pipe2[1], STDOUT);

		close(pipe1[0]);
		close(pipe2[1]);

		execvp("sort", sort_args);
   
		perror("execvp() failed for sort");
		exit(-1);
	}
	else
	{
		close(pipe1[0]);
		close(pipe2[1]);

		if(randseed == 0)
			srand(time(NULL));
		else
			srand(randseed);

		char writeBuffer[256] = {'\0'};

		printf("Random sleepTimes sent to child:\n================================\n");
		for(i=0; i<nWorkers; i++)
		{
			float random = (rand()/(float) RAND_MAX) * (sleepMax-sleepMin) + sleepMin;
			sprintf(writeBuffer+j,"%f\n", random);
			printf("%s", writeBuffer+j);
			j=strlen(writeBuffer);
		}

		write(pipe1[1], writeBuffer, strlen(writeBuffer));

		close(pipe1[1]);

		if(wait(&childStatus) < 0 )
		{
			perror("wait() failed for sort");
			exit(-1);
		}
		
		float sleepTimes[nWorkers];
		char *readBuffer;
		readBuffer = (char*)malloc((strlen(writeBuffer)+1)*sizeof(char));
		j=0;
		printf("\nSorted sleepTimes recieved from child:\n======================================\n");
		while(j<nWorkers)
		{
			i=0;
			char c;
			while(read(pipe2[0],&c,1)==1 && c!='\n'){
			readBuffer[i++]=c;
			}
			readBuffer[i]='\0';
			printf("%s\n", readBuffer);
			sleepTimes[j] = atof(readBuffer);
			j++;
		}
		close(pipe2[0]);
		printf("\n");

		if(semID != -1)
		{
			for (i=0; i<nBuffers; i++)
			{
				union semun sem_union;
				sem_union.val = 1;
				if(semctl(semID, i, SETVAL, sem_union) < 0)
				{
					perror("semctl() failed");
					exit(-1);
				}
			}
		}

		for(i=0; i<nWorkers; i++)
		{ //fork workers
			int pid2 = fork();
			if(pid2 < 0)
			{
				perror("fork() failed");
				exit(-1);
			}
			if(pid2 == 0)
			{
				char workerID[10];
				sprintf(workerID, "%d", i+1);

				char numBuffers[10];
				sprintf(numBuffers, "%d", nBuffers);

				char sleepTime[10];
				sprintf(sleepTime, "%f", sleepTimes[i]);

				char msgQueID[10];
				sprintf(msgQueID, "%d", msgQue);

				char shmID[10];
				sprintf(shmID, "%i", shmid);

				if(semID!=-1)
				{
					char semId[10];
					sprintf(semId, "%d", semID);
					char *worker_args[] = {"worker",  workerID, numBuffers, sleepTime, msgQueID, shmID, semId, NULL};
					execv("worker", worker_args);
				}
				else
				{
					 char *worker_args[] = {"worker",  workerID, numBuffers, sleepTime, msgQueID, shmID, NULL};
					 execv("worker", worker_args);
				}
				perror("execv() failed for worker");
				exit(-1);
			}
			else
				continue;
		}
		int nleft = nWorkers;
		int readErrors=0, writeErrors=0;
		while(nleft)
		{ //rcv worker messages
			struct msgbuffer rcvd;
			if(msgrcv(msgQue, &rcvd, sizeof(rcvd), 0, 0) < 0)
			{
				perror("msgrcv() failed");
				exit(-1);
			}
			printf("%s\n",rcvd.mtext);
			switch(rcvd.mtype)
			{
				case 2: readErrors++;
					break;
				case 3: if(wait(&workerStatus) <0)
					{
						perror("wait() failed for worker");
						exit(-1);
					}
					nleft--;
					break;
				default: break;
			}
		}
		printf("\nAll workers have finished execution.\n");

		int expectedVal = (1<<nWorkers)-1;
		printf("\nExpected value in all buffers = %d\n", expectedVal);
		printf("Actual value in all buffers is as follows:\n");
		if ((shm = (int*)shmat(shmid, NULL, 0)) == (int*) -1)
		{
			perror("shmat() error");
			exit(-1);
		}
		for(i=0; i<nBuffers; i++)
			printf("Buffer[%d] = %d\n", i, shm[i]);
		
		//check for bad bits
		for(i=0; i<nBuffers; i++)
		{
			int diffbits = shm[i] ^ expectedVal;
			int setBits = countSetBits(diffbits);
			if(setBits)
			{
				writeErrors++;
				printf("=========================================\nError in Buffer[%d]. No. of Bad Bits = %d\n", i, setBits);
			}
			int iter;
			for (j=1, iter=1; j<256; j<<=1, iter++)
				if (diffbits & j)
					printf("Bad Bit set at bit_index[%d] in Buffer[%d]\n", iter, i);
		}

		printf("\n<=====================>\nTotal Read Errors = %d\nTotal Write Errors = %d\n<=====================>\n", readErrors, writeErrors);
		if(msgctl(msgQue, IPC_RMID, NULL)){

			perror("msgctl() failed");
			exit(-1);
		}

		if(shmdt(shm) < 0)
		{
			perror("shmdt() failed");
			exit(-1);
		}
		
		if(shmctl(shmid, IPC_RMID, NULL) < 0)
		{
			perror("shmctl() failed");
			exit(-1);
		}
		if(semID!=-1)
		{
			if(semctl(semID, 0, IPC_RMID) == -1)
			{
				perror("semctl() failed");
				exit(-1);
			}
		}
	}
	return 0;
}

int countSetBits(int i)
{
	i = i - ((i >> 1) & 0x55555555);
	i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

void checkInput(int nBuffers, int nWorkers, float sleepMin, float 
sleepMax)
{
	if(!checkPrime(nBuffers) || nBuffers > 32)
	{
		printf("nBuffers should be prime and less than 32\n");
		exit(-1);
	}

	if(nWorkers > nBuffers)
	{
		printf("nWorkers should be less than nBuffers\n");
		exit(-1);
	}

	if(sleepMin<0 || sleepMax<0 || sleepMin > sleepMax)
	{
		printf("sleepMin and sleepMax should be greater than 0 ; sleepMax should be greater than sleepMin\n");
		exit(-1);
	}
}

int checkPrime(int number)
{
    if (number <= 1) return 0;
    int i;
    for (i=2; i*i<=number; i++) {
        if (number % i == 0) return 0;
    }
    return 1;
}

//============================================================================
// Author      : Shubham Jain
// Email       : sjain39@uic.edu
// NetID       : sjain39
// Course      : CS385
// Description : HW2 Assignment
//============================================================================

#include <errno.h>
#include <sys/msg.h>
#include <string.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>

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

void wait(int);
void signal(int);

int main(int argc, char **argv)
{
	int workerID = atoi(argv[1]);
	int nBuffers = atoi(argv[2]);
	float sleepTime = atof(argv[3]);
	int msgID = atoi(argv[4]);
	int shmID = atoi(argv[5]);
	int semID = -1;
	if(argc==7)
	{
		printf("Semaphore running in Worker[%d]...\n", workerID);
		semID = atoi(argv[6]);
	}
	
	sprintf(msg.mtext,"Worker[%d] has started with sleepTime = %f", workerID, sleepTime);
	msg.mtype = 1;
	if(msgsnd(msgID, &msg, sizeof(msg)-sizeof(msg.mtype),0) < 0)
	{
		printf("for Worker[%d]: ", workerID);
		perror("msgsnd() failed for starting worker");
		exit(-1);
	}

	int *shm = (int*)shmat(shmID, NULL, 0);
	if(shm == (int *) -1)
	{
		perror("shmat() failed");
		exit(-1);
	}

	int i,prevVal=0,currVal=0;
	int currentIndex = workerID;
	for(i=1; i<=3*nBuffers; i++)
	{
        	currentIndex = (i*workerID) % nBuffers; //the buffer being accessed
        	if(i%3==0)
		{
            		if(semID!=-1)
				wait(semID);
            
			//write operation
			prevVal = shm[currentIndex];

			if(usleep(sleepTime*100000) < 0)
			{
				perror("Error sleeping ");
				exit(-1);
			}
			int initVal = prevVal | (1<<(workerID -1));
			shm[currentIndex] = initVal;
            
            		if(semID!=-1)
				signal(semID);
            
        	}
		else
		{
			if(semID!=-1)
				wait(semID);
            
			//read operation
			prevVal = shm[currentIndex];

			if(usleep(sleepTime*100000) < 0)
			{
				perror("Error sleeping ");
				exit(-1);
			}

			currVal = shm[currentIndex];
			if(currVal != prevVal)
			{
				msg.mtype = 2;
				sprintf(msg.mtext, "Worker[%d] reported change from %d to %d in buffer[%d].", workerID, prevVal, currVal, currentIndex);
				if(msgsnd(msgID, &msg, sizeof(msg)-sizeof(msg.mtype),0) < 0)
				{
					printf("for Worker[%d]: ", workerID);
					perror("msgsnd() failed for error reporting");
					exit(-1);
				}
			}
			if(semID!=-1)
				signal(semID);
		}
	}

	msg.mtype = 3;
	sprintf(msg.mtext, "Worker[%d] has finished execution.", workerID);
	if(msgsnd(msgID, &msg, sizeof(msg)-sizeof(msg.mtype),0) < 0)
	{
		printf("for Worker[%d]: ", workerID);
		perror("msgsnd() failed for finished execution");
		exit(-1);
	}
	return 0;
}

void wait(int semID)
{
	struct sembuf semBuffer;
	semBuffer.sem_num = 0;
	semBuffer.sem_op = -1;
	semBuffer.sem_flg = SEM_UNDO;
	if(semop(semID, &semBuffer, 1) == -1)
	{
		perror("semop() failed in wait");
		exit(-1);
	}
}

void signal(int semID)
{
	struct sembuf semBuffer;
	semBuffer.sem_num = 0;
	semBuffer.sem_op = 1;
	semBuffer.sem_flg = SEM_UNDO;
	if(semop(semID, &semBuffer, 1) == -1)
	{
		perror("semop() failed in signal");
		exit(-1);
	}
}

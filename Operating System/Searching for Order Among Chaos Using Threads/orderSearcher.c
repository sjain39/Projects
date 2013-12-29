//============================================================================
// Author      : Shubham Jain
// Email       : sjain39@uic.edu
// NetID       : sjain39
// Course      : CS385
// Description : HW3 Assignment
//============================================================================

#include <sys/stat.h>
#include <sys/mman.h> 
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <time.h>

#define BLOCKSIZE 80
#define MAX_THREADS 5000
#define TRUE 1
#define FALSE 0

struct inputParams{
	long start;
	long end;
	int blocksize;
	int id;
};

float sd(int, int);
float sd_change(int, int);
unsigned char range(int, int);
unsigned char max_abs_change(int, int);
unsigned char sum_abs_change(int, int);
void plotData();
void searchFunctionNoThreads(int);
void *searchFunction(void *);
void *plotFunction(void *);
double criteria(int);
void finalStatistics(int, double);

static void check (int test, const char * message, ...)
{
	if (test)
	{
		va_list args;
		va_start (args, message);
		vfprintf (stderr, message, args);
		va_end (args);
		fprintf (stderr, "\n");
		exit (EXIT_FAILURE);
	}
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
unsigned char * data = NULL;
long bestLocation = -1;
double bestValue = 255;
long bestFound = FALSE, plotting = TRUE, bestPlotted = FALSE;
int countCandidate = 0;

int main (int argc, char** argv)
{
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n[HW3] Searching for Order Among Chaos Using Threads \nName: Shubham Jain\nEmail: sjain39@uic.edu\nNetID: sjain39\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	if(argc != 3)
	{
		printf("\nUsage: PROGRAM <filename> <nThreads>\n\n");
		exit(-1);
	}

	int fd;
	struct stat s;
	int status;
	size_t size;
	const char * file_name = argv[1];
	unsigned char * mapped;
	int nThreads = atoi(argv[2]);
	
	if(nThreads > 5000)
	{
		printf("\nNo. of Threads cannot be greater than 5000\n\n");
		exit(-1);
	}
	long chunkSize = 0;

	//timer start
	clock_t begin, end;
	double time_spent;

	begin = clock();

	/* Open the file for reading. */
	fd = open (argv[1], O_RDONLY);
	check (fd < 0, "open %s failed: %s\n", file_name, strerror (errno));

	/* Get the size of the file. */
	status = fstat (fd, & s);
	check (status < 0, "stat %s failed: %s\n", file_name, strerror (errno));
	
	size = s.st_size;

	printf("\nOpening file '%s'\n", argv[1]);
	printf("Size of the file = %ld\n", (long)size);

	if(nThreads > 0)
		chunkSize = size/nThreads;

	/* Memory-map the file. */
	mapped = mmap (0, size, PROT_READ, MAP_PRIVATE, fd, 0);
	check (mapped == MAP_FAILED, "mmap %s failed: %s", file_name, strerror (errno));

	data = mapped;

	pthread_t thread_id[nThreads+1];
	int i,j,start=0;

	pthread_create(&thread_id[0], NULL, plotFunction, NULL);
	for(i=1; i < nThreads+1; i++)
	{
		struct inputParams *args = malloc(sizeof(struct inputParams));
		args->start = start;
		args->end = start+chunkSize;
		args->blocksize = BLOCKSIZE;
		args->id = i;
		pthread_create(&thread_id[i], NULL, searchFunction, args);
		start+=chunkSize;
	}
	
	for(j=1; j < nThreads+1; j++)
	{
		pthread_join(thread_id[j], NULL);
	}

	if(nThreads == 0)
	{
		searchFunctionNoThreads(size);
	}
	plotting = FALSE;
	pthread_join(thread_id[0], NULL);

	end = clock();
	time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	//timer end
	finalStatistics(nThreads, time_spent);

	return 0;
}

void finalStatistics(int nThreads, double timeSpent)
{
	printf("\nNo. of Threads used = %d\n", nThreads);
	printf("No. of candidates found = %d\n", countCandidate);
        printf("Total Time for Execution = %.2f\n", timeSpent);
	printf("\nBest candidate found is as follows:\n");
	printf("Best Location = %ld\n", bestLocation);
	printf("Criteria Value = %.2f\n", bestValue);
	plotData();
}

void searchFunctionNoThreads(int size)
{
	long s;
	double value;
	for(s = 0; s < size - BLOCKSIZE; s++)
	{
		value = criteria(s);
		if(!bestFound || value < bestValue)
		{
			bestFound = TRUE;
			bestValue = value;
			bestLocation = s;
			bestPlotted = FALSE;
		}
	}
}

void *searchFunction(void *input)
{
	struct inputParams *params = (struct inputParams*)input;
	long start = params->start;
	long end = params->end;
	int blocksize = params->blocksize;
	int id = params->id;

	printf("\nThread[%d] started execution on data from start_index = %ld and end_index = %ld\n", id, start, end);
	long s;
	double value;
	for(s = start; s < end - blocksize; s++)
	//for(s = start; s < end; s++)
	{
		value = criteria(s);
		pthread_mutex_lock(&mutex);
		if(!bestFound || value < bestValue)
		{
			bestFound = TRUE;
			bestValue = value;
			bestLocation = s;
			bestPlotted = FALSE;
		}
		pthread_mutex_unlock(&mutex);
	}
	printf("Thread[%d] finished execution..\n", id);
	pthread_exit(0);
}

void *plotFunction(void *input)
{
	while(plotting)
	{
		pthread_mutex_lock(&mutex);
		if(bestFound && !bestPlotted)
		{
			countCandidate++;
			printf("\n--------UPDATED-------");
			printf("\nCANDIDATE #%d :",countCandidate); 
			printf("\nBEST LOCATION = %ld", bestLocation);
			printf("\nBEST CRITERIA VALUE = %.2f\n", bestValue);
			plotData();
			bestPlotted = TRUE;
		}
		pthread_mutex_unlock(&mutex);
		//else
		//	sleep(1);
	}//while
	pthread_exit(0);
}

double criteria(int s)
{
	return 0.25*sd(s, BLOCKSIZE) + 0.30*sd_change(s, BLOCKSIZE) + 0.1*range(s, BLOCKSIZE) + 0.15*max_abs_change(s, BLOCKSIZE) + 0.20*sum_abs_change(s, BLOCKSIZE);
}

void plotData()
{
	unsigned char * a = data+bestLocation;
	int i,j;
	int n=BLOCKSIZE;
	for(i=10; i>=0; i--)
        {
                for(j=-1; j<n; j++)
                {
			if(j==-1)
				printf("|");
			else
			{
				if(i == (a[j]*10/255))
					printf("x");
				else
					printf(" ");
			}
                }
                printf("\n");
        }
	printf("+");
	for(j=0; j<n; j++)
		printf("-");
	printf("\n\n");
}

float sd(int s, int n)
{
	unsigned char * a = data+s;
	if(n == 0)
		return 0.0;
	int sum = 0, i = 0;
	for(i = 0; i < n; ++i)
		sum += a[i];
	float mean = sum / n;
	float sq_diff_sum = 0.0;
	float diff = 0.0;
	for(i = 0; i < n; ++i) 
	{
		diff = a[i] - mean;
		sq_diff_sum += diff * diff;
	}
	float variance = sq_diff_sum / n;
	return sqrt(variance);
}

float sd_change(int s, int n)
{
	unsigned char * a = data+s;
	int i=0, sum=0;
	unsigned char delta[n-1];
	for(i=0; i<n-1; i++)
	{
		delta[i] = a[i+1]-a[i];
	}
	for(i=0; i<n-1; i++)
		sum+=delta[i];
	float mean = sum/(n-1);
	float sq_diff_sum = 0.0;
	float diff = 0.0;
	for(i = 0; i < n-1; i++)
        {
                diff = delta[i] - mean;
                sq_diff_sum += diff * diff;
        }
        float variance = sq_diff_sum / (n-1);
        return sqrt(variance);
}

unsigned char range(int s, int num_elements)
{
	unsigned char * a = data+s;
	int i, max=a[0], min=a[0];
	for (i=1; i<num_elements; i++)
	{
		if(a[i]>max)
		{
			max=a[i];
		}
		else if(a[i]<min)
		{
			min=a[i];
		}
	}
	return(max-min);
}

unsigned char max_abs_change(int s, int n)
{
	unsigned char * a = data+s;
	int i=0;
	unsigned char diff[n-1];
	for(i=0; i<n-1; i++)
	{
		diff[i] = abs(a[i+1]-a[i]);
	}
	int max = diff[0];
	for(i=1; i<n-1; i++)
	{
		if(diff[i]>max)
			max = diff[i];
	}
	return max;
}

unsigned char sum_abs_change(int s, int n)
{
	unsigned char * a = data+s;
        int i=0;
        unsigned char diff[n-1];
        for(i=0; i<n-1; i++)
        {
                diff[i] = abs(a[i+1]-a[i]);
        }
        int sum = 0;
        for(i=0; i<n-1; i++)
		sum += diff[i];
        return sum;
}

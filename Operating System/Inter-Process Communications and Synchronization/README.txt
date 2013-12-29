***************************************************************************************************
* Author: Shubham Jain 
* NetID: sjain39
* Email: sjain39@uic.edu 
* Couse: CS385 - Operating Systems  
* University: University of Illinois at Chicago
***************************************************************************************************

Completed all the parts of HW2 Assignment [IPC and Synchronization]

Usage:
master nBuffers nWorkers sleepMin sleepMax [ randSeed ] [ -lock | -nolock ]

Makefile:
make all -> makes master and worker
make master -> makes master
make worker -> makes worker
make clean -> deletes master and worker binary

How it works?:
In this assignment, a number of worker processes access a group of shared buffers for both reading and writing purposes. Interprocess communications is used 
by worker processes to notify the parent about the results of their work.
The master program generates randomly generated sleeptimes after forking sort in child. These sleeptimes alongwith other arguements are then used to exec worker programs. The worker programs read and write the shared memory buffers and communicate the status to master program using queues. Without semaphores, the worker program can overwrite shared memory buffers. However, on using semaphores this problem is resolved since each worker waits for the other worker process to finish and then execute. At last, the queue, shared memory, and all semaphores are removed in the master program.

Optional Enhancement:
Made a shell script to clean all the unused queues, shared memories and semaphores manually. 
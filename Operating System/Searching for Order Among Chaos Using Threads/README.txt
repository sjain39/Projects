***************************************************************************************************
* Author: Shubham Jain 
* NetID: sjain39
* Email: sjain39@uic.edu 
* Couse: CS385 - Operating Systems  
* University: University of Illinois at Chicago
***************************************************************************************************

Completed HW3 Assignment [Searching for Order among Chaos Using Threads]

Usage:
orderSearcher [inputDataFile] [nThreads]

	inputDataFile -> file to be used for analyzing the order in data
	nThreads -> number of threads to be used to analyze the file in chunks

Makefile:
make all -> makes orderSearcher
make clean -> deletes orderSearcher

How it works?:
Tested this program on a single core machine.
The program reads the file using mmap and splits it evenly among number of threads. I have used POSIX threads to implement multithreading and synchronization is achieved among threads using mutex locks. (n+1) threads are used, where n threads are used for searching the order in data parallely and the remaining thread is responsible for ploting the best results on screen as they arrive.
A well-formulated criteria is used to find the best ordered pattern. The criteria is based on a combined value of different statistical measures: Range, Maximum Absolute Change, Sum Absolute Change, Standard Deviation and Standard Deviation Change.
Evaluation Criteria = (10% of Range) + (15% of Max Absolute Change) + (20% of Sum Absolute Change) + (25% of Standard Deviation) + (30% of Standard Deviation Change)
The analysis of data discretely shows the effect on multi-threading was better than not using any threads in the program. However, after certian number of threads there was only slight improvement in the performance with each added thread. I have attached some results in the report to support my arguement.
Please go through the report for a detailed analysis.
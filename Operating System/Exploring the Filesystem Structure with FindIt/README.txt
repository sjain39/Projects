***************************************************************************************************
* Author: Shubham Jain 
* NetID: sjain39
* Email: sjain39@uic.edu 
* Couse: CS385 - Operating Systems  
* University: University of Illinois at Chicago
***************************************************************************************************

Completed HW4 Assignment [Exploring the FileSystem Structure with FindIt]

Usage: findit [path] [-expression]
Supported expressions:
        -print
        -size [bytes]
        -atime [day]
        -largest [f/d]
        -smallest [f/d]
        -treedir
        -summarize
        -access [r/w/x]
        -help
Example commands:
        findit [path] -print -> prints list of files and sub-directories
        findit [path] -size +2000 -> prints list of files with size greater than 2000 bytes
        findit [path] -atime +1 -> prints list of files with access time greater than 1 day
        findit [path] -size +2000 -atime -1 -> prints list of files with size greater than 2000 bytes and access time less than 1 day
        findit [path] -largest [f/d] -> f: finds largest file
                                        d: finds largest directory
        findit [path] -smallest [f/d] -> f: finds smallest file
                                d: finds smallest directory
        findit [path] -treedir -> prints out a directory tree listing all of the directories, subdirectories, sub-subdirectories with details
        findit [path] -summarize -> prints all the inode info on the given path, with total no. of files, sub-directories, size etc
        findit [path] -access [r/w/x] -> prints list of files with given [r: read/w: write /x: execute] right
        findit -help -> displays help options


Makefile:
make all -> makes findit
make clean -> deletes findit

Features:
- Successfully implemented recursive directory traverse functionality
- Supported various in-built expressions of unix find program (-print, -size, -atime)
- Supported custome expressions as given in the assignment (-treedir, -summarize, -largest, -smallest)
- Supports more than one expression (-size +2000 -atime +1)

How it works?
It can search the entire system for files meeting certain criteria, and once it finds them, it can take a variety of different actions, including printing out the file or executing any other UNIX command with the found file(s) as input arguments.
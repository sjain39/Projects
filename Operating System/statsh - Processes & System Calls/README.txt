***************************************************************************************************
* Author: Shubham Jain 
* NetID: sjain39
* Email: sjain39@uic.edu 
* Couse: CS385 - Operating Systems  
* University: University of Illinois at Chicago
***************************************************************************************************

Implemented all the parts of the HW1 assignment.
This is a custom shell "statsh" which is expected to behave similar to unix command line interface. It comprises of running multiple processes using pipes, file I/O redirection, running processes in background and giving statistics of all the commands on typing "stats".

Files Included:-
statsh.cpp
timeKiller.cpp
makefile
README.txt
statsh

Compile Instructions:
$ make
(creates binary named statsh)

Run Instruction:
$ ./statsh
(replaces the linux original shell with the statsh shell created by us)

Single Commands:
$ date
Fri Sep 27 17:26:37 UTC 2013

Multiple Commands using pipes:
$ ls | grep statsh | wc
      2       2      43

File I/O redirect:
$ sort < README.txt
(displays README.txt contents in linewise alphabetical order)
$ ls > list.txt
(directs output of ls to list.txt)
$ cat < README.txt | wc > wordcount_readme.txt
(takes input from README.txt, and writes the word count of file README.txt in wordcount_readme.txt)

Running processes in background:
$ date &
(continuously wait for children which have not exited yet until invalid PID is returned from wait4. Used WNOHANG to get wait4 return immediately if no child has exited at this point)

Show history of commands including user time and system time:
$ stats

Exit program:
$ exit
(exits the program showing history of all the commands executed, including complete summary statistics displaying user time and system time of shell itself)

COMMENTS:
I tried to complete the assignment in a limited time constraints. So inspite of intensive testing, the program might still have bugs, otherwise the program runs really well comprising most of the features of a normal shell. The program could have been coded in a more structured way, but since there was so much time constraint, my major focus was to complete all the parts of the assignment rather than making the code look more nicer and structured. At last, I am happy that I am able to implement all the parts of the assignment and have learned a lot about how the original linux shell works (pipes, file I/O, background processes).
The guidelines by Professor Bell and TA Siming Chen helped a lot in completing this assignment. A special thank of note to them.

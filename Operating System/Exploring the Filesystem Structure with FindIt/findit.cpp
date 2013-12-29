#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <cstring>
#include <dirent.h>
#include <algorithm>
#include "dirReadLin.h"

using namespace std;

void displayHelp();
void displaySummarize(char*);

template <typename T>
std::vector<T> inter(const std::vector<T> & v1, const std::vector<T> & v2)
{
    std::vector<T> v3;   
    std::set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), std::back_inserter(v3));
    return v3;
}

int main(int argc, char** argv)
{
	printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n[HW4] Exploring the FileSystem Structure with FindIt\nName: Shubham Jain\nEmail: sjain39@uic.edu\nNetID: sjain39\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n\n");
	int i=0, start=1;
	if (argc < 2)
	{
        	//check to see if an argument was passed.
        	printf("Usage: %s [path] [-expression]\n",argv[0]);
        	exit(-1);
    	}
        //create the basic path to the directory using command-line argument
        string dir = ".";
        //create the list of strings
        vector<string> files = vector<string>();
        vector<string> filessize = vector<string>();
        vector<string> filesatime = vector<string>();

	
	if(argv[1][0] != '-')
	{
		//printf("Path specified = %s\n\n", argv[1]);
		dir = argv[1];
		start = 2;
	}

	bool print = false, size = false, atime = false;
	for(i=start; i<argc; i++)
	{
		if(strcmp(argv[i], "-print") == 0)
		{
			//printf("print\n");
			printdir(dir,files,true);
			print = true;
		}
		else if(strcmp(argv[i], "-size") == 0)
		{
			//printf("size\n");
			if(argc < 5)
				sizedir(dir,files,true, argv[i+1]);
			else
			{
				size = true;
				sizedir(dir,filessize,true,argv[i+1]);
			}
		}
		else if(strcmp(argv[i], "-atime") == 0)
                {
                        //printf("atime\n");
			if(argc < 5)
                        	atimedir(dir,files,true,argv[i+1]);
			else
			{
				atime = true;
				atimedir(dir,filesatime,true,argv[i+1]);
			}
                }
		else if(strcmp(argv[i], "-access") == 0)
		{
			//printf("access\n");
			accessdir(dir,files,true,*argv[i+1]);
		}
		else if(strcmp(argv[i], "-largest") == 0)
		{
			//printf("largest\n");
			if(*argv[i+1] == 'f')
				printf("Largest file = %s\n",largeSizeFile(dir, true).c_str());
			else if(*argv[i+1] == 'd')
				printf("Largest directory = %s\n",largeSizeDir(dir, true).c_str());

		}
		else if(strcmp(argv[i], "-smallest") == 0)
                {
                        //printf("smallest\n");
			if(*argv[i+1] == 'f')
                                printf("Smallest file = %s\n",smallSizeFile(dir, true).c_str());
                        else if(*argv[i+1] == 'd')
                                printf("Smallest directory = %s\n",smallSizeDir(dir, true).c_str());

                }
		else if(strcmp(argv[i], "-summarize") == 0)
                {
                        //printf("summarize\n");
			char *dirpath1 = new char[dir.length() + 1];
                        strcpy(dirpath1, dir.c_str());
			displaySummarize(dirpath1);
                }
		else if(strcmp(argv[i], "-treedir") == 0)
                {
                        //printf("treedir\n");
			char *dirpath = new char[dir.length() + 1];
			strcpy(dirpath, dir.c_str());
			treedir(dirpath, 1);
                }
		else if(strcmp(argv[i], "-help") == 0)
                {
                        //printf("help\n");
			displayHelp();
                }
		else if(strcmp(argv[i], "-not") == 0 || strcmp(argv[i], "-or") == 0)
                {
                        printf("[%s] expression is not supported\n", argv[i]);
                }
	}

	if(argc<5)
	{
		for(unsigned int i = 0;i<files.size();i++){
                    cout << files[i] << endl;
                }
	}
	else
	{
		if(print!=true)
		{
			std::sort(filessize.begin(), filessize.end());   // sort vec1
			std::sort(filesatime.begin(), filesatime.end());   // sort vec2
			std::vector<std::string> v3 = inter(filessize, filesatime);
			for(unsigned int i = 0;i<v3.size();i++){
			    cout << v3[i] << endl;
			}
		}
		else if(size==true)
		{
			std::sort(filessize.begin(), filessize.end());   // sort vec1
                        std::sort(files.begin(), files.end());   // sort vec2
                        std::vector<std::string> v3 = inter(filessize, files);
                        for(unsigned int i = 0;i<v3.size();i++){
                            cout << v3[i] << endl;
                        }
		}
		else if(atime==true)
		{
                        std::sort(files.begin(), files.end());   // sort vec1
                        std::sort(filesatime.begin(), filesatime.end());   // sort vec2
                        std::vector<std::string> v3 = inter(files, filesatime);
                        for(unsigned int i = 0;i<v3.size();i++){
                            cout << v3[i] << endl;
                        }
                }
        }

}

void displayHelp()
{
	printf("Usage: findit [path] [-expression]\n");
	printf("Supported expressions:\n\t-print\n\t-size [bytes]\n\t-atime [day]\n\t-largest [f/d]\n\t-smallest [f/d]\n\t-treedir\n\t-summarize\n\t-access [r/w/x]\n\t-help\n");
	printf("Example commands:\n\t");
	printf("findit [path] -print -> prints list of files and sub-directories\n\t");
	printf("findit [path] -size +2000 -> prints list of files with size greater than 2000 bytes\n\t");
	printf("findit [path] -atime +1 -> prints list of files with access time greater than 1 day\n\t");
	printf("findit [path] -size +2000 -atime -1 -> prints list of files with size greater than 2000 bytes and access time less than 1 day\n\t");
	printf("findit [path] -largest [f/d] -> f: finds largest file\n\t\t\t\t\td: finds largest directory\n\t");
	printf("findit [path] -smallest [f/d] -> f: finds smallest file\n\t\t\t\td: finds smallest directory\n\t");
	printf("findit [path] -treedir -> prints out a directory tree listing all of the directories, subdirectories, sub-subdirectories with details\n\t");
	printf("findit [path] -summarize -> prints all the inode info on the given path, with total no. of files, sub-directories, size etc\n\t");
	printf("findit [path] -access [r/w/x] -> prints list of files with given [r: read/w: write /x: execute] right\n\t");
	printf("findit -help -> displays help options\n\n");
}

void displaySummarize(char* dir) {
 struct stat file_stats;

 if((stat(dir, &file_stats)) == -1) {
  perror("stat()..");
 }
 
 printf(" device: %lld\n",                       file_stats.st_dev);
 printf(" inode: %ld\n",                         file_stats.st_ino);
 printf(" protection: %o\n",                     file_stats.st_mode);
 printf(" number of hard links: %d\n",           file_stats.st_nlink);
 printf(" user ID of owner: %d\n",               file_stats.st_uid);
 printf(" group ID of owner: %d\n",              file_stats.st_gid);
 printf(" device type (if inode device): %lld\n",file_stats.st_rdev);
 printf(" total size, in bytes: %ld\n",          file_stats.st_size);
 printf(" blocksize for filesystem I/O: %ld\n",  file_stats.st_blksize);
 printf(" number of blocks allocated: %ld\n",    file_stats.st_blocks);
 printf(" time of last access: %ld : %s",        file_stats.st_atime, ctime(&file_stats.st_atime));
 printf(" time of last modification: %ld : %s",  file_stats.st_mtime, ctime(&file_stats.st_mtime));
 printf(" time of last change: %ld : %s\n",        file_stats.st_ctime, ctime(&file_stats.st_ctime));
char* accesses[] = {"...", "..x", ".w.", ".wx", "r..", "r.x", "rw.", "rwx"};
int i;
ushort mode = file_stats.st_mode;
printf("access rights on directory[%s] = ", dir);
for(i = 6; i >= 0; i -=3)
      printf("%s", accesses[(mode >> i) & 7]);
   printf("\n");
 printf("No. of files in the directory = %d\n", dirNumFiles(dir, true));
 printf("No. of sub-directories in the directory = %d\n", dirNumDirs(dir, true));
}

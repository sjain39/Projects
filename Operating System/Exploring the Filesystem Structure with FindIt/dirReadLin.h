#include<cstdio>
#include<iostream>
#include<cstdlib>
#include<string.h>
#include<stddef.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<dirent.h>
#include<vector>
#include<time.h>

using namespace std;

struct data
{
    int numFiles;
    int numDirs;
    long size;
};
typedef struct data* pack;

int dirNumDirs(string, bool);
int dirNumFiles(string, bool);
long dirSize(string, bool);

pack countDirs(char* dir)
{
    DIR* dp;
    struct dirent *entry;
    struct stat statbuf;
    pack p = (pack) malloc(sizeof(pack*));
    p -> numFiles = 0;
    p -> numDirs = 0;
    p -> size = 0;
    if ( (dp = opendir(dir)) == NULL)
    {
        fprintf(stderr, "Cannot open directory %s\n", dir);
        return p;
    }
    chdir(dir);
    while( (entry = readdir(dp)) != NULL)   //while there are still links
    {
        lstat(entry -> d_name, &statbuf);
        if(S_ISDIR(statbuf.st_mode))
        {
            if( strcmp(".", entry -> d_name) == 0 ||
                strcmp("..", entry -> d_name) == 0)
            continue;
            pack pp = countDirs(entry -> d_name);
            p->numFiles += pp->numFiles;
            p->numDirs++;
            p->numDirs += pp->numDirs;
            p->size += pp->size;
        }
        else
        {
            p -> numFiles ++;
            p -> size += statbuf.st_size;
        }
    }
    chdir("..");   ///Line I forgot
    closedir(dp);
    return p;
}

void treedir(char *dir, int depth)
{
    //printf("%*s%s/",spaces,"",dir);
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    int spaces = depth*4;

    //printf("%*s%s/",spaces,"",dir);

    if((dp = opendir(dir)) == NULL) {
        fprintf(stderr,"cannot open directory: %s\n", dir);
        return;
    }
    chdir(dir);
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".",entry->d_name) == 0 || strcmp("..",entry->d_name) == 0)
                continue;
            printf("%*s%s/",spaces,"",entry->d_name);
                pack s = countDirs(entry->d_name);
                printf("( %d dirs, %d files, %ld bytes )\n",s->numDirs,s->numFiles,s->size);
            /* Recurse at a new indent level */
            treedir(entry->d_name,depth+1);
        }
        //else
        //printf("%*s%s\n",spaces,"",entry->d_name);
    }
    chdir("..");
    closedir(dp);
}

bool isDir(string dir){
    struct stat fileInfo;
    stat(dir.c_str(), &fileInfo);
    if (S_ISDIR(fileInfo.st_mode)){
        return true;
    }else{
        return false;
    }
}

long fileSize(string filename){
    struct stat fileInfo;
    stat(filename.c_str(), &fileInfo);
    return fileInfo.st_size;
}

long fileAccessTime(string filename){
    struct stat fileInfo;
    stat(filename.c_str(), &fileInfo);
    return fileInfo.st_atime;
}

string to_string(long number){
    string number_string = "";
    char ones_char;
    int ones = 0;
    while(true){
        ones = number % 10;
        switch(ones){
            case 0: ones_char = '0'; break;
            case 1: ones_char = '1'; break;
            case 2: ones_char = '2'; break;
            case 3: ones_char = '3'; break;
            case 4: ones_char = '4'; break;
            case 5: ones_char = '5'; break;
            case 6: ones_char = '6'; break;
            case 7: ones_char = '7'; break;
            case 8: ones_char = '8'; break;
            case 9: ones_char = '9'; break;
            //default : ErrorHandling("Trouble converting number to string.");
        }
        number -= ones;
        number_string = ones_char + number_string;
        if(number == 0){
            break;
        }
        number = number/10;
    }
    return number_string;
}

void printdir(string dir, vector<string> &files, bool recursive){
    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                     //files.push_back(string(entry->d_name)); //add entry to the list of files
                     files.push_back(string(dir+entry->d_name)); //add entry to the list of files
                     printdir(dir + entry->d_name, files, true); //recurse
                 } else{
                      //files.push_back(string(entry->d_name));//add the entry to the list of files
			files.push_back(string(dir+entry->d_name));//add the entry to the list of files
                 } 
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
}

void accessdir(string dir, vector<string> &files, bool recursive, char perm){
    int perm_mode=-1;
    if(perm == 'r')
	perm_mode = R_OK;
    else if(perm == 'w')
	perm_mode = W_OK;
    else if(perm == 'x');
	perm_mode = X_OK;

    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
		if(access(entry->d_name, perm_mode)==0)
		{
                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                     files.push_back(string(dir+entry->d_name)); //add entry to the list of files
                     accessdir(dir + entry->d_name, files, true, perm); //recurse
                 } else{
                        files.push_back(string(dir+entry->d_name));//add the entry to the list of files
                 }
		}
		//else
		 //perror("access() failed..");
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
}


void atimedir(string dir, vector<string> &files, bool recursive, char* atime){
    unsigned long int curr_time = time(NULL);
    unsigned long int acc_time;
    if(atime[0]=='+' || atime[0]!='-')
	acc_time = curr_time - (atoi(atime)+1)*86400;
    if(atime[0]=='-')
	acc_time = curr_time + (atoi(atime))*86400;
	//acc_time = curr_time + atoi(atime)*86400;
    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
		long dir_atime = fileAccessTime(dir+entry->d_name);
                if(atime[0]=='+' || atime[0]!='-')
		{
			if(dir_atime < acc_time)
			{
				files.push_back(string(dir+entry->d_name));
			}
			else if(dir_atime == acc_time)
			{
				files.push_back(string(dir+entry->d_name));
			}
		}
                else if(atime[0]=='-')
		{
			if(dir_atime > acc_time)
			{
				files.push_back(string(dir+entry->d_name));
			}
		}

                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                     atimedir(dir + entry->d_name, files, true, atime); //recurse
                 } 
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
}

void sizedir(string dir, vector<string> &files, bool recursive, char* size){
    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                     sizedir(dir + entry->d_name, files, true, size); //recurse
                 } else{
			long file_size = fileSize(dir+entry->d_name);
                        if(size[0]=='+' || size[0]!='-')
                        {
                                if(file_size > atoi(size))
                                {
                                        files.push_back(string(dir+entry->d_name));
                                }
				if(file_size == atoi(size))
				{
					files.push_back(string(dir+entry->d_name));
				}
                        }
                        else if(size[0]=='-')
                        {
                                if(file_size < -atoi(size))
                                {
                                        files.push_back(string(dir+entry->d_name));
                                }
                        }
                 }
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
}

/*void treedir(string dir, vector<string> &files, bool recursive){
    string dash = "";
    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                     dash+="--";
		     files.push_back(string(dash+dir+entry->d_name)+" ( "+to_string(dirNumDirs(dir+entry->d_name, true))+" dirs, "+to_string(dirNumFiles(dir+entry->d_name, true))+" files, "+to_string(dirSize(dir+entry->d_name, true))+" bytes )");//add entry to the list of files
                     treedir(dir + entry->d_name, files, true); //recurse
                 } else{
                        //files.push_back(string(dir+entry->d_name));//add the entry to the list of files
                 }
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
}*/

string largeSizeFile(string dir, bool recursive){
    long current_file_size=0, max_size=0;
    int flag = 0;
    string large_file_path;
    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                     largeSizeFile(dir + entry->d_name, true); //recurse
                 } else{
			if(flag == 0)
			{
				max_size = fileSize(dir+entry->d_name);
				large_file_path = dir+entry->d_name;
				flag = 1;
			}
			else
			{
				current_file_size = fileSize(dir+entry->d_name);
				if(max_size < current_file_size)
				{
					max_size = current_file_size;
					large_file_path = dir+entry->d_name;
				}
			}	
                 }
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
    return large_file_path;
}

string largeSizeDir(string dir, bool recursive){
    long current_dir_size=0, max_size=0;
    int flag = 0;
    string large_dir_path;
    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                     	if(flag == 0)
                        {
                                max_size = dirSize(dir+entry->d_name, true);
                                large_dir_path = dir+entry->d_name;
                                flag = 1;
                        }
                        else
                        {
                                current_dir_size = dirSize(dir+entry->d_name, true);
                                if(max_size < current_dir_size)
                                {
                                        max_size = current_dir_size;
                                        large_dir_path = dir+entry->d_name;
                                }
                        }

			largeSizeDir(dir + entry->d_name, true); //recurse
                 }
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
    return large_dir_path;
}


string smallSizeFile(string dir, bool recursive){
    long current_file_size=0, min_size=0;
    int flag = 0;
    string small_file_path;
    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                     smallSizeFile(dir + entry->d_name, true); //recurse
                 } else{
                        if(flag == 0)
                        {
                                min_size = fileSize(dir+entry->d_name);
                                small_file_path = dir+entry->d_name;
                                flag = 1;
                        }
                        else
                        {
                                current_file_size = fileSize(dir+entry->d_name);
                                if(min_size > current_file_size)
                                {
                                        min_size = current_file_size;
                                        small_file_path = dir+entry->d_name;
                                }
                        }
                 }
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
    return small_file_path;
}

string smallSizeDir(string dir, bool recursive){
    long current_dir_size=0, min_size=0;
    int flag = 0;
    string small_dir_path;
    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                        if(flag == 0)
                        {
                                min_size = dirSize(dir+entry->d_name, true);
                                small_dir_path = dir+entry->d_name;
                                flag = 1;
                        }
                        else
                        {
                                current_dir_size = dirSize(dir+entry->d_name, true);
                                if(min_size > current_dir_size)
                                {
                                        min_size = current_dir_size;
                                        small_dir_path = dir+entry->d_name;
                                }
                        }

                        smallSizeDir(dir + entry->d_name, true); //recurse
                 }
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
    return small_dir_path;
}


long dirSize(string dir, bool recursive){
    long file_size=0;
    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                     dirSize(dir + entry->d_name, true); //recurse
                 } else{
                        file_size+=fileSize(dir+entry->d_name);
                 }
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
    return file_size;
}

int dirNumFiles(string dir, bool recursive){
    int file_num=0;
    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                     dirNumFiles(dir + entry->d_name, true); //recurse
                 } else{
			file_num++;
                 }
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
    return file_num;
}

int dirNumDirs(string dir, bool recursive){
    int dir_num=0;
    DIR *dp; //create the directory object
    struct dirent *entry; //create the entry structure
    dp=opendir(dir.c_str()); //open directory by converting the string to const char*
    if(dir.at(dir.length()-1)!='/'){
        dir=dir+"/";
    }
    if(dp!=NULL){ //if the directory isn't empty
         while( entry=readdir(dp) ){ //while there is something in the directory
             if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0){ //and if the entry isn't "." or ".."
                 if(isDir(dir + entry->d_name) == true && recursive == true){//check if the new path is a directory, and if it is (and recursion is specified as true), recurse.
                     dir_num++; //recurse
                     dirNumDirs(dir + entry->d_name, true); //recurse
                 } else{
                        //le_num++;
                 }
             }
         }
         (void) closedir(dp); //close directory
    }
    else{
        perror ("Couldn't open the directory.");
    }
    return dir_num;
}



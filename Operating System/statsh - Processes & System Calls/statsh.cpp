//============================================================================
// Name        : statsh.cpp
// Author      : Shubham Jain
// Email       : sjain39@uic.edu
// NetID       : sjain39
// Course      : CS385
// Description : HW1 Assignment
//============================================================================
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

#define BUF_SIZE 100

struct rusage usg;
typedef struct history
{
	char* command;
	double userTime;
	double systemTime;
	history* next;
}history;

void loop_pipe(char ***, history *);
history *list_create(char *);
history *list_insert(history *, char *);
void list_delete(history**);
void list_print(history*);
void printAllUsage();
char * deblank(char *);
int background=0;

int main()
{
	char* input_string;
	char* history_string;
	char* pipeParse;
	char* spaceParse;
	char** pipeCmd;
	char*** spaceCmd;
	size_t n;
	int i = 0, j = 0, num_cmd = 1;
	int* num_space;
	history* head = NULL;
	
	printf("\nStatsh Shell by:\nName: Shubham Jain\nEmail: sjain39@uic.edu\nNetID: sjain39\n---------------------------\n");
	char* user = (char *)getenv("USER");
	if(user == NULL)
	{
		user == "anonymous";
	}
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL)
        	fprintf(stdout, "Current working dir: %s\n\n", cwd);
	else
		perror("getcwd() error");

	input_string = (char*)malloc(BUF_SIZE*sizeof(char));

	while(1)
	{
		printf("%s@statsh:%s$ ", user, cwd);
		getline(&input_string, &n, stdin);
		input_string[strlen(input_string)-1]='\0';

		history_string = (char*)malloc(strlen(input_string)+1);
		memcpy(history_string, input_string, strlen(input_string)+1);
		
		//checks if the process needs to be run in background
		if(strchr(input_string, '&')!=NULL)
		{
			background=1;
			printf("Process running in background...\n");
			printf("background\n");
			input_string[strlen(input_string)-1]='\0';
			printf("Input String: %s\n", input_string);
		}

		if(strcmp(input_string, "exit") == 0)
		{
			list_print(head);
			printAllUsage();
			list_delete(&head);
			exit(0);
		}

		else if(strcmp(input_string, "") == 0)
			continue;

		else if(strcmp(input_string, "stats") == 0)
		{
			list_print(head);
			continue;
		}	
	
		while(input_string[i] != '\0')
		{
			if(input_string[i] == '|')
				num_cmd++;
			i++;
		}

		//parses input string based on the pipe '|' delimeter
		pipeCmd = (char**)malloc(num_cmd*sizeof(char*));
		i = 0;
		pipeParse = strtok(input_string, "|");
		while(pipeParse != NULL)
		{
			if(strchr(pipeParse, '<')!=NULL || strchr(pipeParse, '>')!=NULL)
				pipeCmd[i] = deblank(pipeParse);
			else
				pipeCmd[i] = pipeParse;
			pipeParse = strtok(NULL, "|");
			i++;
		}

		num_space = (int*)malloc(num_cmd*sizeof(int));
		for(i=0;i<num_cmd;i++)
		{
			for(j=0;j<strlen(pipeCmd[i]);j++)
			{
				if(pipeCmd[i][j] == ' ')
					(num_space[i])++;
			}
		}
		
		//parses individual commands based on the space ' ' delimeter
		spaceCmd = (char***)malloc((num_cmd+1)*sizeof(char**));
		for(i=0;i<num_cmd;i++)
		{
			spaceCmd[i] = (char**)malloc((num_space[i]+1+1)*sizeof(char*));
			spaceParse = strtok(pipeCmd[i]," ");
			for(j=0;spaceParse!=NULL;j++)
			{
				spaceCmd[i][j] = spaceParse;
				spaceParse = strtok(NULL, " ");
			}
			spaceCmd[i][num_space[i]+1]=NULL;
		}
		spaceCmd[i]=NULL;

		if(head == NULL)
		{
			head = list_create(history_string);	//creates first node to history struct
		}
		else
		{
			head = list_insert(head, history_string);	//adds nodes to history struct
		}

		loop_pipe(spaceCmd, head);
		num_cmd=1;
		background=0;
	}
	return 0;
}

//executes the commands called in multiple pipes by forking child processes in while loop, while the parent process waits for the child processes to finish using wait4
void loop_pipe(char ***cmd, history *head) 
{
	int   p[2];
	pid_t pid;
	int   fd_in = 0, childStatus = -1;
	int in, out;
	double totalUser=0, totalSystem=0;

	while (*cmd != NULL)
	{
		pipe(p);
		if ((pid = fork()) == -1)
		{
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)
		{
			if(strchr(*cmd[0], '<')!=NULL)
			{
				char * line = strdup(**cmd);
				char * token;
				token = deblank(strtok(line,"<"));
				char * s[]={token, NULL};
				*cmd = s;
				token = deblank(strtok(NULL, "<"));
				in = open(token, O_RDONLY);
				fd_in = in;
			}
			dup2(fd_in, 0); //change the input according to the old one
			close(fd_in);
			if (*(cmd + 1) != NULL)
				dup2(p[1], 1);
			else
				if(strchr(**cmd, '>')!=NULL)
				{
					char * line = strdup(**cmd);
					char * token;
					token = deblank(strtok(line,">"));
					char * s[]={token, NULL};
					*cmd = s;
					token = deblank(strtok(NULL, ">"));
					out = open(token, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
					dup2(out, 1);
					close(out);
				}
			close(p[0]);
			execvp((*cmd)[0], *cmd);
			printf("ERROR: Command not found\n");
			exit(EXIT_FAILURE);
		}
		else
		{
			if(background==1)
			{
				while ((pid = wait4(-1, &childStatus, WNOHANG, &usg)) > 0) {
				printf("child %d terminated\n", pid);}
			}
			else
				wait4(pid, &childStatus, 0, &usg);

			getrusage(pid, &usg);
			totalUser += (double)usg.ru_utime.tv_sec*(1000000) + (double)usg.ru_utime.tv_usec / 1000000;
			totalSystem += (double)usg.ru_stime.tv_sec*(1000000) + (double)usg.ru_stime.tv_usec / 1000000;
			close(p[1]);
			fd_in = p[0]; //save the input for the next command
			cmd++;
		}
	}
	if(NULL != head)
	{
		head->userTime = totalUser;
		head->systemTime = totalSystem;
	}
}

history *list_create(char *history_input)
{
	history *node;
	if(!(node=(history *)malloc(sizeof(history)))) return NULL;
	node->command = (char *)malloc(strlen(history_input));
	memcpy(node->command, history_input, strlen(history_input));
	node->userTime = 0;
	node->systemTime = 0;
	node->next=NULL;
	return node;
}

history *list_insert(history *list, char *data)
{
	history *newnode;
        newnode=list_create(data);
        newnode->next = list;
	return newnode;
}

//deletes all history nodes
void list_delete(history **head)
{
    history *current = *head;
    history *temp;
    if(current == NULL)
	 return;
    while(current != NULL)
    {
         temp = current;
         current = current->next;
         free(temp);
    }
    *head = NULL;
}

//print user time and system of all history nodes recursively
void list_print(history *head)
{
	history *current = head;
	history *temp;
	
	if(current == NULL)
		return;
	if((current->next) != NULL)
                list_print(current->next);

        printf(":> %s\n-------------------\n", current->command);
        printf("User Time = %.3f\n", current->userTime);
        printf("System Time = %.3f\n", current->systemTime);
        printf("-------------------\n\n");
}

//print complete summary statitics on program exit
void printAllUsage()
{
	if(getrusage(RUSAGE_CHILDREN, &usg) < 0)
		printf("NULL!\n");
	else
	{
		double user_time = (double)usg.ru_utime.tv_sec*(1000000) + (double)usg.ru_utime.tv_usec / 1000000;
		double system_time = (double)usg.ru_stime.tv_sec*(1000000) + (double)usg.ru_stime.tv_usec / 1000000;

		printf("\n<<Statsh>>\n------------------------\nShell User Time: %.3f\nShell System Time: %.3f\n------------------------\n\n", user_time, system_time);
	}
}

//removes unnecessary spaces from the string
char * deblank(char *str)
{
  char *out = str, *put = str;

  for(; *str != '\0'; ++str)
  {
    if(*str != ' ')
      *put++ = *str;
  }
  *put = '\0';

  return out;
}

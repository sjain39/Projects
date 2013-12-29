#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>
#include <dirent.h>

#define BACKLOG 20   

void sigchld_handler(int s)
{
  while(waitpid(-1, NULL, WNOHANG) > 0);
}

void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

pthread_mutex_t mutex_pagefd = PTHREAD_MUTEX_INITIALIZER;
int listenfd, pagefd;
char *ROOT;

struct {
	char *ext;
	char *filetype;
} extensions [] = {  
	{".htm", "text/html" },  
	{".html","text/html" },
	{".txt", "text/plain"},
	{".ico", "image/x-icon"  },
	{".gif", "image/gif"  },
    {".jpg", "image/jpeg"  },
    {".png", "image/png"  },
    {".pdf", "application/pdf"  },
	{0,0} };

void *response( void* argv)
{
    int connectionfd = (int)(intptr_t)argv;
	char mesg[99999], *reqline[3], data_to_send[1024], path[99999];
    int rcvd, fd, bytes_read, i;
	char *mimetype,buffer[1023];
    memset( (void*)mesg, (int)'\0', 99999 );

    rcvd=recv(connectionfd, mesg, 99999, 0);

    if (rcvd<0)  
        fprintf(stderr,("recv() error\n"));
    else if (rcvd==0)    
        fprintf(stderr,"Client disconnected upexpectedly.\n");
    else    
    {
        printf("%s", mesg);
        reqline[0] = strtok (mesg, " \t\n");
        if ( strncmp(reqline[0], "GET\0", 4)==0 )
        {
            reqline[1] = strtok (NULL, " \t");
            reqline[2] = strtok (NULL, " \t\n");
            if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
            {
                send(connectionfd, "HTTP/1.0 400 Bad Request\n", 25, 0);
            }
            else
            {
				if ( strncmp( strrchr(reqline[1],'/'), "/\0", 2)==0 )
                    reqline[1] = "/index.html";        
				
                strcpy(path, ROOT);
                strcpy(&path[strlen(ROOT)], reqline[1]);
                printf("file: %s\n", path);
				printf("file extension: %s\n", strstr(path,"."));
				for(i=0;extensions[i].ext != 0;i++)
				{
					if(strstr(path, ".")!=NULL)
					{
						if( !strcmp(strstr(path,"."), extensions[i].ext))
						{
							mimetype = extensions[i].filetype;
							break;
						}
					}
					else
						mimetype = 0;
				}
				sprintf(buffer,"HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", mimetype);	

                if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
                {
                    send(connectionfd, buffer, strlen(buffer), 0);
                    while ( (bytes_read=read(fd, data_to_send, 1024))>0 )
                        write (connectionfd, data_to_send, bytes_read);
                }
                else    write(connectionfd, "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
            }
        }
    }

    shutdown (connectionfd, SHUT_RDWR);         
    close(connectionfd);
    connectionfd=-1;
}
int main(int argc, char* argv[])
{
  int sockfd, new_fd, tempfd;  
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr; 
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1, count;
  char s[INET6_ADDRSTRLEN];
  int rv;
  pthread_t tid[1000];

  ROOT = argv[2];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE; 

  if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for(p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype,
            p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
          sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  if (p == NULL)  {
    fprintf(stderr, "server: failed to bind\n");
    return 2;
  }

  freeaddrinfo(servinfo); 

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  sa.sa_handler = sigchld_handler; 
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  count=0;
  while(1) {  
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    inet_ntop(their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s);
    printf("server: got connection from %s\n", s);
	
	tempfd = new_fd;
	if(pthread_create(&tid[count],NULL,response,(void *)(intptr_t)tempfd) < 0){
	    perror("error creating thread");
	    exit(1);
	}
	else{
	    count++;
	}
	
    /*if (!fork()) { // this is the child process
      close(sockfd); // child doesn't need the listener
      if (send(new_fd, "Hello, world!", 13, 0) == -1)
        perror("send");
      close(new_fd);
      exit(0);
    }*/
    //close(new_fd);  // parent doesn't need this
  }

  return 0;
}

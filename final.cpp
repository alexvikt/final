#define DEBUG
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#ifdef DEBUG
char logbuf[256] = "echo \0";
#endif

struct inthr {
int s;
struct sockaddr_in a;
char *d;
};

void *connect(void *arg) {
#ifdef DEBUG
system("echo Connect... >>  debug.log");
#endif
  inthr sock = *(inthr *) arg;
  free ((inthr *) arg);
  bool notfound;
  char buf[512];
  char method[32];
  char uri[128];
  char *ptr;
  struct stat file_stat;

    memset(buf,0, sizeof(char)*512);
    memset(method,0, sizeof(char)*32);
    memset(uri,0, sizeof(char)*128);

    int n = recv(sock.s, buf, 511, 0);
    if (n < 0) {
        if (errno != EINTR) {
        exit(EXIT_FAILURE);
      } 
    }
#ifdef DEBUG
sprintf(&logbuf[5]," %d bytes received. >> debug.log", n);
system(logbuf);
{
int logfd;
logfd = open("log.txt", O_RDWR | O_APPEND);
if (logfd != -1) {
  write(logfd,buf,n);
  close(logfd);
}
}
#endif
    if (n > 0) {
      sscanf(buf,"%s %s %*s",method,uri);
#ifdef DEBUG
sprintf(&logbuf[5],"Method:  %s. URI: %s  >> debug.log", method, uri);
system(logbuf);
#endif
      if (strlen(method)==3 && strcmp(method,"GET")==0) {
#ifdef DEBUG
system("echo Method GET - OK. >>  debug.log");
#endif
      ptr = strchr(uri,'?');
      if (ptr != NULL) *ptr = '\0';
      strcpy(method,uri);
      strcpy(uri,sock.d);
      strcat(uri,method);
      notfound = true;
      if (stat(uri, &file_stat) == 0) {
#ifdef DEBUG
sprintf(&logbuf[5],"File %s exist. %lu byte.  >> debug.log", uri, file_stat.st_size);
system(logbuf);
#endif
      int fd;
      int nb, lh;
      fd = open(uri, O_RDONLY);
      if (fd != -1) {
        notfound = false;
        char *buffer = (char *) malloc(file_stat.st_size + 100); 
        memset(buffer,0, sizeof(char) * (file_stat.st_size + 100));
        sprintf(buffer,"HTTP/1.0 200 OK\r\nContent-length: %lu\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n", file_stat.st_size);
        lh = strlen(buffer);
        nb = read (fd,&buffer[lh],file_stat.st_size);
#ifdef DEBUG
sprintf(&logbuf[5],"FD %d . Read %d byte.  >> debug.log", fd, nb);
system(logbuf);
int logfd;
logfd = open("log.txt", O_RDWR | O_APPEND);
if (logfd != -1) {
  write(logfd,buffer,lh+nb);
  close(logfd);
}
#endif
        close (fd);
        sendto(sock.s, buffer, strlen(buffer), 0, (struct sockaddr *)& sock.a, sizeof(sock.a));
        free(buffer);
      }
      } 
      if (notfound) {
#ifdef DEBUG
sprintf(&logbuf[5],"File %s not exist.  >> debug.log", uri);
system(logbuf);
#endif
      memset(buf,0, sizeof(char)*512);
      sprintf(buf,"HTTP/1.0 404 NOT FOUND\r\nContent-Type: text/html\r\n\r\n");
      sendto(sock.s, buf, strlen(buf), 0, (struct sockaddr *)& sock.a, sizeof(sock.a));
      }
      
      }
    }
  close (sock.s);
#ifdef DEBUG
system("echo Disconnect. Close socket. >>  debug.log");
#endif
}


int main(int argc, char **argv) {

#ifdef DEBUG
system("echo Start... > debug.log");
#endif
  pid_t pid;
  int socketfd;
  inthr *pinthr;
  struct sockaddr_in addr;
  int opt = 0;
  char *ip;
  int port;
  char *dir;
  int thrres;
  pthread_t a_thread;

  opt = getopt( argc, argv, "h:p:d:");
  while(opt != -1) {
    switch(opt) {
      case 'h':
        ip = optarg;
        break;

      case 'p':
        port = atoi(optarg);
        break;

      case 'd':
        dir = optarg;
        break;

      default:
        break;
    }
    opt = getopt( argc, argv, "h:p:d:");
  }
#ifdef DEBUG
system("echo Init... >> debug.log");
sprintf(&logbuf[5],"IP = %s Port = %d Dir = %s >> debug.log", ip, port, dir);
system(logbuf);
#endif
  pid = fork();
  if (pid < 0) {
    printf("Error demonization!");  
    exit(EXIT_FAILURE);
  }
  if (pid > 0) {
#ifdef DEBUG
system("echo End parent. >> debug.log");
#endif
    exit(EXIT_SUCCESS);
  }

#ifdef DEBUG
system("echo In child. Close std. >> debug.log");
#endif
  close(STDIN_FILENO);
  close(STDOUT_FILENO);
  close(STDERR_FILENO);
  
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = inet_addr(ip);
  addr.sin_port = htons(port);
  if (bind(socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
#ifdef DEBUG
system("echo Error calling bind. >>  debug.log");
#endif
    exit(EXIT_FAILURE);
  }
  if (listen(socketfd, 5)) {
#ifdef DEBUG
system("echo Error calling listen. >>  debug.log");
#endif
    exit(EXIT_FAILURE);
  }
  while (1) {
#ifdef DEBUG
system("echo Wait accept... >>  debug.log");
#endif
    int s = accept(socketfd, NULL, NULL);
    if (s<0) {
#ifdef DEBUG
system("echo Error calling accept. >>  debug.log");
#endif
      exit(EXIT_FAILURE);
    }
#ifdef DEBUG
system("echo Accept OK. Call connect. >>  debug.log");
#endif
    pinthr = (inthr *) malloc (sizeof(inthr));
    pinthr->s = s;
    pinthr->a = addr;
    pinthr->d = dir;
    thrres = pthread_create(&a_thread, NULL, connect,(void *)pinthr);
    if (thrres != 0) {
      exit(EXIT_FAILURE);
    }
  }

return 0;
}


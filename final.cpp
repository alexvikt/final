#define DEBUG
#include <sys/socket.h>
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
};

void *connect(void *arg) {
#ifdef DEBUG
system("echo Connect... >>  debug.log");
#endif
  inthr sock = *(inthr *) arg;
  free ((inthr *) arg);
  char buf[512];

  for (;;) {
    memset(buf,0, sizeof(char)*512);
    int n = recv(sock.s, buf, 511, 0);
    if (n < 0) {
        if (errno != EINTR) {
        exit(EXIT_FAILURE);
      } 
    }
#ifdef DEBUG
sprintf(&logbuf[5]," %d bytes received. >> debug.log", n);
system(logbuf);
#endif
    sendto(sock.s, buf, strlen(buf), 0, (struct sockaddr *)& sock.a, sizeof(sock.a));
  if (buf[0] == '!') break;
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
    thrres = pthread_create(&a_thread, NULL, connect,(void *)pinthr);
    if (thrres != 0) {
      exit(EXIT_FAILURE);
    }
  }

return 0;
}


#define DEBUG
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>


int main(int argc, char **argv) {

#ifdef DEBUG
char logbuf[256] = "echo \0";
system("echo Start... > debug.log");
#endif
  pid_t pid;
  int opt = 0;
  char *ip;
  int port;
  char *dir;

  printf("Start final\n");
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

  while (1);

return 0;
}

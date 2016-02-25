#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char **argv) {
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
  printf("IP = %s  %d\n", ip, strlen(ip));
  printf("Port = %d\n", port);
  printf("Dir = %s\n", dir);
return 0;
}

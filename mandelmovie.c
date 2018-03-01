// elayne & gking5
// Project 3

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define MAXSVAL 2
#define MINSVAL .00001

int startedImages = 0; 
double sIncrement = (MAXSVAL - MINSVAL) / 50;
double sVal = 2;
char *mandelCommand[] = {"mandel",
"-W", "500",
"-H",  "500",
"-x", "0.35",
"-y",  "0.1",
"-m", "2000",
"-s", "000000000",
"-o", "mandel00.bmp",
NULL};

void usage() {
  printf("usage: ./mandelmovie \t<MAXPROCESSES>\n");
  return;
}

int forkMandel() {

  sVal -= sIncrement;
  pid_t res = fork();

  if (res == -1) {
    printf("error: fork failed\n");
    return -1;
  } else if (res == 0) { // child process
    asprintf(&mandelCommand[12], "%lf", sVal); // load -s flag value into command string
    asprintf(&mandelCommand[14], "mandel%02d.bmp", startedImages); // load -o flag value into command sting
    execvp(mandelCommand[0], mandelCommand); // run mandel
    printf("error: exec failed\n");
    return -1;
  } else {
    startedImages++;
  }

  return 0;
}

int main(int argc, char *argv[]) {

  if (argc != 2) { // check for valid argument count
    usage();
    return 1;
  }

  int maxProcesses = atoi(argv[1]);
  int waitStat = 0;

  if (maxProcesses < 1) { // validate max processes
    printf("error: MAXPROCESSES must be greater than 0\n");
    return -1;
  } else if (maxProcesses > 50) {
    
  }

  for (int i = 0; i < maxProcesses; i++) { // launch starter batch of maxProcesses processes
    forkMandel();
  }

  while (startedImages < 50) { // every time a process finishes, another one will be forked
    wait(&waitStat);
    forkMandel();
  }

  wait(NULL); // let any remaining processes finish

  return 0;
}

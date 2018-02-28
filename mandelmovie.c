// elayne & gking5
// Project 3

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/wait.h>
//#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MAXSVAL 2
#define MINSVAL .00001

int startedImages = 0; 
float sIncrement = (MAXSVAL - MINSVAL) / 50;
float sVal = 2;
char *mandelCommand[] = {"mandel", "-W",  "500",  "-H",  "500", "-x", "0.35", "-y",  "0.1", "-m", "2000", "-s", "000000000", "-o", "mandel00.bmp", NULL};// TODO add variable s value to this command!!!

void usage() {
  printf("usage: ./mandelmovie \t<MAXPROCESSES>\n");
  return;
}

int forkMandel() {
  int res = fork();

  if (res == -1) {
    printf("error: fork failed\n");
    return -1;
  } else if (res == 0) { // child process
    if (execvp(mandelCommand[0], mandelCommand) < 0) { // TODO fill out execvp, make sure having this in a function works OK
      printf("error: exec failed\n");
      return -1;
    }
  } else {
    sVal -= sIncrement;
    char bmpName[12];
    strcpy(bmpName, "mandel");
    char numbers[2] = "\0\0"; 
    sprintf(numbers, "%02d", startedImages);
    strcat(bmpName, numbers);
    strcat(bmpName, ".bmp");
    printf(bmpName);
    //sprintf(mandelCommand[14], "mandel%d")
    //printf("%.2f", sVal); // TODO weird seg fault from next line
    //sprintf(mandelCommand[2], "test");
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

  //for (int i = 0; i < 50; i++) {
  //  sVal -= sIncrement;     
  //  printf("%.6f", sVal);
  //}

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

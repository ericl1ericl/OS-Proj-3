#include "bitmap.h"
#include "mandel.h"
#include<iostream>

void usage() {
  std::cout << "usage: ./mandel [MAXPROCESSES]" << std::endl;
  return;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    usage();   
  }	
  return 0;
}


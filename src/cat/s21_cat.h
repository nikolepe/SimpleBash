#ifndef SRC_CAT_S21_CAT_H_
#define SRC_CAT_S21_CAT_H_

#include <getopt.h>
#include <stdio.h>
#include <string.h>

typedef struct {
  int flag_n;
  int flag_b;
  int flag_s;
  int flag_t;
  int flag_e;
  int flag_v;
  int countLines;
  int countEmptyLines;

} Flag;

int WorkWithFile(const char *filename, Flag options);
Flag parseFlag(int argc, char *argv[]);

#endif

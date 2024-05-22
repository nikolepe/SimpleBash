#ifndef C3_SIMPLEBASHUTILS_1_S21_GREP_H
#define C3_SIMPLEBASHUTILS_1_S21_GREP_H

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  int flag_e;
  int flag_i;
  int flag_v;
  int flag_c;
  int flag_l;
  int flag_n;
  int flag_h;
  int flag_s;
  int flag_f;
  int flag_o;
  int countFiles;
  int numberLine;
  int countMatches;
} Flags;

int ScanFlag(int argc, char **argv, Flags *options, char **pattern);
void WorkWithLines(char *str, Flags *Flags, char *pattern, char *name);
void ReadFile(FILE *file, Flags *options, char *pattern, char *path);
void Option_f(Flags *options, char **pattern, char *optarg);
void Option_e(Flags *options, char **pattern, char *optarg);
void Option_o(regex_t my_regex, char *str, Flags options);
void auxProcess(Flags options, char *path);
void *reallocString(void *str, int size);
int SetupPattern(char **str, char *optarg);
void WorkWithFlags(Flags *options, int argc);
void AddPattern(char **str, char *optarg);

#endif  // C3_SIMPLEBASHUTILS_1_S21_GREP_H
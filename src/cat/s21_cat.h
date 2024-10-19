#ifndef S21_CAT_H
#define S21_CAT_H
#define _GNU_SOURCE
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Structure to store flags
typedef struct {
  int b; // Number non-blank lines
  int e; // Show end-of-line characters as $
  int n; // Number all lines
  int s; // Squeeze multiple blank lines
  int T; // Show tabs as ^I
  int t; // Show tabs and non-printable characters
  int v; // Show non-printable characters
} flag;

// Functions for cat implementation
void init_flags(flag *flag);
void check_flags(int argc, char **argv, flag *flag);
void cat_no_arg();
void print(int argc, char *argv[], const flag *flag);

#endif
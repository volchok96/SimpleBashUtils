#ifndef S21_GREP_H
#define S21_GREP_H
#define _GNU_SOURCE
#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFSIZE 4096 // block size in file systems (4-8 KB)

typedef struct {
  int e; // For regex
  int i; // Ignores case
  int v; // Inverts the search result
  int c; // Number of lines in which the search string is found
  int l; // Number of files in which the search string is found
  int n; // Line number in the output of the search string
  int h; // Does not display files where the search string was found
  int s; // Does not display messages about files that were not found or cannot be read
  int f; // Gets regular expressions from a file (arguments are needed)
  int o; // Displays only the matched string without the whole line

  long long int string_count;
  int match_lines_count;
  int match_files_count;
  int empty_lines;
  int file_s;
  int e_count;

  char search_express[BUFFSIZE];
} flags_grep;

// void init_options(flags_grep * flag);
void check_flags_grep(int argc, char **argv, flags_grep *flag);
void open_file(int argc, char **argv, flags_grep *flag);
void flag_grep_e(flags_grep *flag);
void flag_grep_f(char *argv[], flags_grep *flag); // changed const char *argv[] to char *argv[]
void search_and_print(char *argv[], FILE *file, flags_grep *flag, // changed char *argv[] to const char *argv[]
                      int file_index);
void is_it_files(int argc, char **argv, flags_grep *flag);
extern int optind, opterr, optopt;

#endif

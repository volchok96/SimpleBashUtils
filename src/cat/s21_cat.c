#include "s21_cat.h"
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
  static flag flag; // Static structure for storing flags
  if (argc == 1) {
    cat_no_arg(); // If no arguments, read from stdin
  } else {
    init_flags(&flag);              // Initialize flags
    check_flags(argc, argv, &flag); // Check flags
    print(argc, argv, &flag);       // Print file content considering flags
  }
  return 0;
}

// Function to read from stdin if no arguments are passed
void cat_no_arg() {
  char buf[4096];                       // Input buffer
  int fd = 0;                           // Standard input (file descriptor 0)
  int bytes_read = read(fd, buf, 4096); // Read from stdin
  while (bytes_read > 0) {
    printf("%.*s", bytes_read, buf);  // Output buffer content
    bytes_read = read(fd, buf, 4096); // Continue reading until end of input
  }
}

// Initializing the flag structure (set all flags to 0)
void init_flags(flag *flag) {
  flag->b = 0;
  flag->e = 0;
  flag->n = 0;
  flag->s = 0;
  flag->T = 0;
  flag->t = 0;
  flag->v = 0;
}

// Structure for handling long and short flags
struct option flags[] = {{"number-nonblank", no_argument, 0, 'b'},
                         {"E", no_argument, 0, 'E'},
                         {"e", no_argument, 0, 'e'},
                         {"number", no_argument, 0, 'n'},
                         {"squeeze-blank", no_argument, 0, 's'},
                         {"T", no_argument, 0, 'T'},
                         {"t", no_argument, 0, 't'},
                         {"v", no_argument, 0, 'v'},
                         {0, 0, 0, 0}};

// Function to handle command line flags using getopt_long()
void check_flags(int argc, char **argv, flag *flag) {
  int res = 0;
  opterr = 0; // Disable default error messages from getopt
  while ((res = getopt_long(argc, argv, "+beEnstTv", flags, 0)) != -1) {
    switch (res) {
    case 'b':
      flag->b = 1;
      flag->n = 0; // Disable -n if -b is set
      break;
    case 'n':
      if (!flag->b)
        flag->n = 1; // Enable -n only if -b is not set
      break;
    case 'E':
      flag->e = 1;
      break;
    case 'e':
      flag->e = 1;
      flag->v = 1;
      break;
    case 's':
      flag->s = 1;
      break;
    case 't':
      flag->t = 1;
      flag->v = 1;
      break;
    case 'T':
      flag->t = 1;
      break;
    case 'v':
      flag->v = 1;
      break;
    case '?':
      dprintf(2, "%s: illegal option -- %c\n", argv[0], res);
      dprintf(2, "usage: %s [-belnstuv] [file ...]\n", argv[0]);
      exit(1);
    }
  }
}

void print(int argc, char *argv[], const flag *flag) {
  int c = 0;

  for (int i = optind; i < argc;
       i++) { // optind is the index of the next element to be processed in argv
    int line_number = 1; // Counts the number of lines
    int is_new_line = 1; // Indicates the start of a new line
    int empty_lines = 0; // Tracks empty lines
    FILE *file = fopen(argv[i], "r");
    if (file == NULL) {
      dprintf(2, "s21_cat: %s: No such file or directory\n", argv[i]);
      continue;
    }
    while ((c = fgetc(file)) != EOF) {
      // Handles squeezing multiple adjacent blank lines
      if (flag->s) { // -s option
        if (c == '\n') {
          ++empty_lines;
          if (empty_lines > 2) {
            continue;
          }
        } else {
          empty_lines = 0;
        }
      }

      // Handle -n and -b flags (number all lines or non-blank lines)
      if (is_new_line) {
        if (flag->n && !flag->b) {
          printf("%6d\t", line_number++);
        } else if (flag->b && c != '\n') {
          printf("%6d\t", line_number++);
        }
        is_new_line = 0;
      }

      // Handle -t flag (replace tabs with ^I)
      if (flag->t && c == '\t') {
        printf("^I");
        continue;
      }

      // Handle -v flag (show non-printable characters, except \n and \t)
      if (flag->v && (c < 32 || c == 127) && c != '\n' && c != '\t') {
        if (c == 127) {
          printf("^?");
        } else {
          printf("^%c", c + 64);
        }
        continue;
      }

      // Handle -e flag (show $ at the end of each line)
      if (flag->e && c == '\n') {
        printf("$");
      }

      // Print the character
      printf("%c", c);

      // Check for new line
      if (c == '\n') {
        is_new_line = 1; // Set the flag for a new line
      }
    }

    fclose(file);
  }
}

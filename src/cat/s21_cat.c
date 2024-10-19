#include "s21_cat.h"

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
      break;
    case 'n':
      flag->n = 1;
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
    case 'v':
      flag->v = 1;
      break;
    case 't':
      flag->t = 1;
      flag->v = 1;
      break;
    case 'T':
      flag->t = 1;
      break;
    case '?':
      dprintf(2, "%s: illegal option -- %c\n", argv[0], res);
      dprintf(2, "usage: %s [-belnstuv] [file ...]\n", argv[0]);
      exit(1);
    }
  }
}

// Function to print file content considering flags
void print(int argc, char *argv[], const flag *flag) {
  int c = 0;

  for (int i = optind; i < argc; i++) {
    int empty_lines = 0;
    int lines = 1;
    int is_new_line = 1;
    FILE *file = fopen(argv[i], "r");
    if (file == NULL) {
      dprintf(2, "s21_cat: %s: No such file or directory\n", argv[i]);
      continue;
    }
    while ((c = fgetc(file)) != EOF) {
      if (flag->s) {
        if (c == '\n') {
          ++empty_lines;
          if (empty_lines > 2)
            continue;
        } else {
          empty_lines = 0;
        }
      }

      if (flag->n && is_new_line && !flag->b) {
        printf("%6d\t", lines++);
        is_new_line = 0;
      }

      if (flag->b && is_new_line && c != '\n') {
        printf("%6d\t", lines++);
        is_new_line = 0;
      }

      if (flag->e && c == '\n')
        printf("$");

      if (flag->t && c == '\t') {
        printf("^I");
        continue;
      }

      if (flag->v && c != '\n' && c != '\t') {
        if (c >= 0 && c <= 31) {
          printf("^%c", c + 64);
          continue;
        } else if (c == 127) {
          printf("^?");
          continue;
        }
      }

      if (c == '\n')
        is_new_line = 1;
      printf("%c", c);
    }
    fclose(file);
  }
}

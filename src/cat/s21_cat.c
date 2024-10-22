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

    // Reading the file character by character
    while ((c = fgetc(file)) != EOF) {
      // Handle -s flag (squeeze blank lines)
      if (flag->s) {
        if (c == '\n') {
          ++empty_lines;
          if (empty_lines > 2)
            continue; // Skip more than two blank lines
        } else {
          empty_lines = 0; // Reset for non-blank lines
        }
      }

      // Handle -n flag (number all lines) if -b is not active
      if (flag->n && is_new_line && !flag->b) {
        printf("%6d\t", lines++);
        is_new_line = 0;
      }

      // Handle -b flag (number non-blank lines)
      if (flag->b && is_new_line && c != '\n') {
        printf("%6d\t", lines++);
        is_new_line = 0;
      }

      // Handle -e flag (append $ at the end of each line)
      if (flag->e && c == '\n')
        printf("$");

      // Handle -t flag (replace tabs with ^I)
      if (flag->t && c == '\t') {
        printf("^I");
        continue; // Move to the next character
      }

      // Handle -v flag (show non-printable characters)
      if (flag->v && c != '\n' && c != '\t') {
        if (c >= 0 && c <= 31) {
          printf("^");
          c += 64; // Convert to visible character
          printf("%c", c);
          continue; // Skip original character print
        } else if (c == 127) {
          printf("^?");
          continue; // Skip original character print
        }
      }

      // Reset new line flag
      if (c == '\n')
        is_new_line = 1;

      // Print the character
      printf("%c", c);
    }

    fclose(file); // Close the file after processing
  }
}

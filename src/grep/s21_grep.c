#include "s21_grep.h"

int main(int argc, char *argv[]) {
  // Check if no arguments are passed and print usage instructions if true
  if (argc == 1) {
    printf(
        "usage: grep [-eivclnhsof] [--ignore-case] [--invert-match] [--count]\n"
        "\t[--files-with-matches] [--line-number] [--no-filename]\n"
        "\t[--s] [--f] [--o] pattern [file ...]\n");
    exit(EXIT_FAILURE);
  }

  // Initialize flags_grep structure with default values
  static flags_grep flags_grep = {0};
  
  // Parse command-line flags_grep and store results in the flags_grep structure
  check_flags_grep(argc, argv, &flags_grep);
  
  // Process files based on parsed flags_grep
  open_file(argc, argv, &flags_grep);
  return 0;
}

// Define flags_grep with associated keys for parsing options
struct option long_flags_grep[] = {{"e", no_argument, 0, 'e'},
                              {"ignore-case", no_argument, 0, 'i'},
                              {"invert-match", no_argument, 0, 'v'},
                              {"count", no_argument, 0, 'c'},
                              {"files-with-matches", no_argument, 0, 'l'},
                              {"line-number", no_argument, 0, 'n'},
                              {"no-filename", no_argument, 0, 'h'},
                              {"s", no_argument, 0, 's'},
                              {"f", no_argument, 0, 'f'},
                              {"o", no_argument, 0, 'o'},
                              {0, 0, 0, 0}};

void check_flags_grep(int argc, char **argv, flags_grep *flags_grep) {
  int res = 0;
  opterr = 0;  // Disable automatic error messages from getopt_long

  // Read flags_grep using getopt_long until -1 is returned
  while ((res = getopt_long(argc, argv, "e:ivclnhsf:o", long_flags_grep, 0)) != -1) {
    switch (res) {
    case 'e':
      flags_grep->e = 1;  // Enable -e flag
      flag_grep_e(flags_grep);  // Call function to process -e flag
      break;
    case 'i':
      flags_grep->i = 1;  // Enable case-insensitive search
      break;
    case 'v':
      flags_grep->v = 1;  // Enable inverted search
      break;
    case 'c':
      flags_grep->c = 1;  // Enable count of matching lines
      break;
    case 'l':
      flags_grep->l = 1;  // Enable list of files with matches
      break;
    case 'n':
      flags_grep->n = 1;  // Enable line numbering in output
      break;
    case 'h':
      flags_grep->h = 1;  // Enable suppression of filenames in output
      break;
    case 's':
      flags_grep->s = 1;  // Enable silent mode (suppress error messages)
      break;
    case 'f':
      flags_grep->f = 1;  // Enable file-based pattern search
      flag_grep_f(argv, flags_grep);  // Process file for -f flag
      break;
    case 'o':
      flags_grep->o = 1;  // Enable output of only matching strings
      break;
    case '?':
      // Print error message for invalid option and exit
      fprintf(stderr, "%s: invalid option -- %s\n", argv[0] + 2, argv[optind - 1]);
      fprintf(stderr,
              "usage: %s [-eivclnhsfo] [file ...] [-e pattern] [-f file] \n",
              argv[0] + 2);
      exit(EXIT_FAILURE);
    }
  }
}

void flag_grep_e(flags_grep *flags_grep) {
  if (flags_grep->e_count > 1)  // Add "|" if multiple expressions are present
    strcat(flags_grep->search_express, "|");
  strcat(flags_grep->search_express, optarg);  // Append the pattern from -e flag
  flags_grep->e_count++;  // Increment the pattern count
}

void flag_grep_f(char *argv[], flags_grep *flags_grep) {
  FILE *file;
  file = fopen(optarg, "r");  // Open file from -f flag argument
  if (file == NULL) {
    fprintf(stderr, "%s: %s: No such file or directory\n", argv[0] + 2, argv[optind - 1]);
    exit(EXIT_FAILURE);
  }

  static char buffer[BUFFSIZE] = {0};

  // Read patterns line by line from file and add them to search expression
  while (fgets(buffer, BUFFSIZE, file) != NULL) {
    if (buffer[strlen(buffer) - 1] == '\n')  // Remove newline character
      buffer[strlen(buffer) - 1] = '\0';
    if (flags_grep->e_count > 0)
      strcat(flags_grep->search_express, "|");  // Add "|" if multiple patterns
    strcat(flags_grep->search_express, buffer);  // Add the pattern
    flags_grep->e_count++;
  }
  fclose(file);  // Close the file
}

int is_flag(char **argv, int i) {
  if (argv[i] != NULL && argv[i][0] == '-') {  // Check if argument is a flag
    if (argv[i][strlen(argv[i]) - 1] == 'e' || argv[i][strlen(argv[i]) - 1] == 'f')
      return 2;  // Return 2 for -e or -f, as they require additional arguments
    return 1;  // Return 1 for other flags_grep
  }
  return 0;  // Return 0 if not a flag
}

void check_files(int argc, char **argv, flags_grep *flags_grep) {
  for (int i = 1; i < argc; i++) {
    int flag_type = is_flag(argv, i);
    if (flag_type == 0 && flags_grep->e_count == 0) {
      flags_grep->e_count = argc - i;  // Count files without flags_grep
      break;
    }
  }
}

void open_file(int argc, char **argv, flags_grep *flags_grep) {
  FILE *file;
  check_files(argc, argv, flags_grep);  // Check for file arguments

  for (int i = 1; i < argc; i++) {
    int flag_type = is_flag(argv, i);
    if (flag_type == 2) {  // Skip additional argument for -e or -f
      i++;
      continue;
    }
    if (!(flags_grep->f || flags_grep->e) && flag_type == 0 && flags_grep->e_count == 0) {
      strcat(flags_grep->search_express, argv[i]);  // Set search expression
      flags_grep->e_count++;
      optind++;
      continue;
    }
    if (flag_type == 0) {  // Process files
      file = fopen(argv[i], "r");
      if (file == NULL) {
        if (!flags_grep->s) {
          fprintf(stderr, "%s: %s: No such file or directory\n", argv[0] + 2, argv[i]);
        }
        continue;
      }
      search_and_print(argv, file, flags_grep, i);  // Search in file
      fclose(file);
    }
  }
}

void search_and_print(char *argv[], FILE *file, flags_grep *flags_grep, int file_index) {
  regex_t regex;
  int regflags = REG_EXTENDED;
  if (flags_grep->i) {
    regflags |= REG_ICASE;  // Set case-insensitive search if -i flag is set
  }

  // Compile the regex with search expression and selected flags_grep
  if (regcomp(&regex, flags_grep->search_express, regflags) != 0) {
    fprintf(stderr, "Regex compilation error.\n");
    return;
  }

  char line[BUFFSIZE] = {0};
  flags_grep->string_count = 0;

  // Read lines one by one from the file
  while (fgets(line, BUFFSIZE, file)) {
    flags_grep->string_count++;  // Count lines
    int match = (regexec(&regex, line, 0, NULL, 0) == 0) != flags_grep->v;

    if (match) {
      if (flags_grep->n) {
        printf("%lld:", flags_grep->string_count);  // Print line number if -n flag is set
      }
      if (!flags_grep->h && argv[file_index] != NULL) {
        printf("%s:", argv[file_index]);  // Print filename if -h flag is not set
      }
      printf("%s", line);  // Print matched line
    }
  }

  regfree(&regex);  // Free regex resources
}

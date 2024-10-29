#include "s21_grep.h"

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf(
    "usage: grep [-eivclnhsof] [--ignore-case] [--invert-match] [--count]\n"
    "\t[--files-with-matches] [--line-number] [--no-filename]\n"
    "\t[--s] [--f] [--o] pattern [file ...]\n");
    exit(EXIT_FAILURE);
  }
  static flags_grep flags_grep = {0};
  check_flags_grep(argc, argv, &flag);
  open_file(argc, argv, &flag);
  return 0;
}

struct option flags[] = {{"e", no_argument, 0, 'e'},  // -b
                         {"ignore-case", no_argument, 0, 'i'},
                         {"invert-match", no_argument, 0, 'v'},
                         {"count", no_argument, 0, 'c'},
                         {"files-with-matches", no_argument, 0, 'l'},
                         {"line-number", no_argument, 0, 'n'},
                         {"no-filename", no_argument, 0, 't'},
                         {"s", no_argument, 0, 's'},
                         {"f", no_argument, 0, 'f'},
                         {"o", no_argument, 0, 'o'},
                         {0, 0, 0, 0}};



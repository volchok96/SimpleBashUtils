#include "s21_grep.h"

int main(int argc, char *argv[]) {
  if (argc == 1) {
    printf(
        "usage: grep [-eivclnhsfo] [file ...] [-e pattern] [-f file]\n");
    exit(EXIT_FAILURE);
  }
  flags_grep flag = {0};
  check_flags_grep(argc, argv, &flag);
  open_file(argc, argv, &flag);
  return 0;
}

struct option long_options[] = {
    {"e", no_argument, 0, 'e'},
    {"ignore-case", no_argument, 0, 'i'},
    {"invert-match", no_argument, 0, 'v'},
    {"count", no_argument, 0, 'c'},
    {"files-with-matches", no_argument, 0, 'l'},
    {"line-number", no_argument, 0, 'n'},
    {"no-filename", no_argument, 0, 'h'},
    {"s", no_argument, 0, 's'},
    {"f", no_argument, 0, 'f'},
    {"o", no_argument, 0, 'o'},
    {0, 0, 0, 0}
};

void check_flags_grep(int argc, char **argv, flags_grep *flag) {
  int res = 0;
  opterr = 0;
  while ((res = getopt_long(argc, argv, "e:ivclnhsf:o", long_options, 0)) != -1) {
    switch (res) {
      case 'e':
        flag->e = 1;
        flag_grep_e(flag);
        break;
      case 'i':
        flag->i = 1;
        break;
      case 'v':
        flag->v = 1;
        break;
      case 'c':
        flag->c = 1;
        break;
      case 'l':
        flag->l = 1;
        break;
      case 'n':
        flag->n = 1;
        break;
      case 'h':
        flag->h = 1;
        break;
      case 's':
        flag->s = 1;
        break;
      case 'f':
        flag->f = 1;
        flag_grep_f(argv, flag);
        break;
      case 'o':
        flag->o = 1;
        break;
      case '?':
        dprintf(2, "%s: invalid option -- %s\n", argv[0] + 2,
                ((argv[optind - 1]) + 1));
        dprintf(2,
                "usage: %s [-eivclnhsfo] [file ...] [-e pattern] [-f file] \n",
                argv[0] + 2);
        exit(1);
    }
  }
}

void flag_grep_e(flags_grep *flag) {
  if (flag->e_count > 0) strcat(flag->search_express, "|");
  strcat(flag->search_express, optarg);
  flag->e_count++;
}

void flag_grep_f(char *argv[], flags_grep *flag) {
  FILE *fd = fopen(optarg, "r");
  if (fd == NULL) {
    dprintf(2, "%s: %s: No such file or directory\n", argv[0] + 2, argv[optind - 1]);
    exit(EXIT_FAILURE);
  }

  char buffer[BUFFSIZE] = {0};
  while (fgets(buffer, BUFFSIZE, fd) != NULL) {
    if (buffer[strlen(buffer) - 1] == '\n') buffer[strlen(buffer) - 1] = 0;
    if (flag->e_count > 0) strcat(flag->search_express, "|");
    if (*buffer == '\0') {
      flag->empty_lines = 1;
      strcat(flag->search_express, ".");
    } else {
      strcat(flag->search_express, buffer);
    }
    flag->e_count++;
  }
  fclose(fd);
}

int is_it_flag(char **argv, int i) {
  int is_flag = 0;
  if (argv[i][0] == '-') {
    is_flag = 1;
    if ((argv[i][strlen(argv[i]) - 1] == 'e') || (argv[i][strlen(argv[i]) - 1] == 'f'))
      is_flag = 2;
  }
  return is_flag;
}

void is_it_files(int argc, char **argv, flags_grep *flag) {
  for (int i = 1, found = 0; i < argc; ++i) {
    int flag_type = is_it_flag(argv, i);
    if (flag_type == 0 && found == 0) {
      if ((argc - i - 1) > 1) flag->file_s++;
      break;
    }
    found = flag_type;
  }
}

void open_file(int argc, char **argv, flags_grep *flag) {
  is_it_files(argc, argv, flag);

  for (int i = 1; i < argc; i++) {
    int flag_type = is_it_flag(argv, i);
    if (flag_type == 2) { 
      i++;
      continue;
    }
    if (!(flag->f || flag->e) && flag_type == 0 && !flag->e_count) {
      strcat(flag->search_express, argv[i] ? argv[i] : "");
      flag->e_count++;
      optind++;
      continue;
    }
    if (flag_type == 0) {
      FILE *fd = fopen(argv[i], "r");
      if (fd == NULL) {
        if (flag->s != 1) {
          dprintf(2, "%s: %s: No such file or directory\n", argv[0] + 2, argv[i]);
        }
        continue;
      }
      search_and_print(argv, fd, flag, i);
      fclose(fd);
    }
  }
}

void search_and_print(char *argv[], FILE *fd, flags_grep *flag, int file_index) {
  regex_t rx;
  int cflags = REG_EXTENDED | (flag->i ? REG_ICASE : 0);
  if (regcomp(&rx, flag->search_express, cflags) != 0) exit(EXIT_FAILURE);

  char str_var[BUFFSIZE] = {0};
  flag->match_lines_count = flag->match_files_count = flag->string_count = 0;

  while (fgets(str_var, BUFFSIZE, fd)) {
    flag->string_count++;
    char *ptr = str_var;
    int match = regexec(&rx, ptr, 1, (regmatch_t[1]){0}, 0);
    if ((match == 0 && flag->v == 0) || (match == REG_NOMATCH && flag->v == 1)) {
      if (!flag->c && !flag->l) {
        if (flag->file_s && !flag->h && !(flag->o && flag->empty_lines)) printf("%s:", argv[file_index]);
        if (flag->n) printf("%lld:", flag->string_count);
        if (flag->o) {
          while (!match && ptr[0]) {
            printf("%.*s\n", (int)(str_var + match - ptr), ptr);
            ptr += match;
            match = regexec(&rx, ptr, 1, (regmatch_t[1]){0}, REG_NOTBOL);
          }
        }
        if (!flag->o || (flag->v && flag->o == 1)) printf("%s", str_var);
      } else {
        flag->match_lines_count++;
      }
    }
  }
  if (flag->c) printf("%d\n", flag->match_lines_count);
  if (flag->l && flag->match_files_count && !flag->v && !flag->h) printf("%s\n", argv[file_index]);

  regfree(&rx);
}

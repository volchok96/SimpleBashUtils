#include "s21_cat.h"

int main(int argc, char **argv) {
  static flag flag;

  if (argc == 1) {
    cat_no_arg();

  } else {
    init_flags(&flag);
    check_flags(argc, argv, &flag);
    print(argc, argv, &flag);
  }
  return 0;
}
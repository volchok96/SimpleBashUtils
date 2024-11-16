##################### VARIABLES #####################

TARGET_CAT = s21_cat
CC = gcc
FLAGS = -Wall -Werror -Wextra -std=c11
LFLAGS = # -fsanitize=address -g

SRC_DIR = .
SRC_C = $(wildcard $(SRC_DIR)/*.c)
SRC_H = $(wildcard $(SRC_DIR)/*.h)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(SRC_C))
INC = -I$(SRC_DIR)

##################### TARGETS #####################

# Main targets
all: s21_cat

s21_cat: $(OBJ)
	$(CC) $(LFLAGS) $(FLAGS) $(OBJ) -o $(TARGET_CAT)

# Rule for compiling .o from .c
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(LFLAGS) $(FLAGS) $(INC) -c $< -o $@

##################### STYLE AND CLEANUP #####################

# Check code style without fixing
style:
	clang-format -n $(SRC_C) $(SRC_H)

# Fix code style
stylecor:
	clang-format -i $(SRC_C) $(SRC_H)

# Clean up built files
clean:
	rm -rf $(TARGET_CAT) $(OBJ)
	rm -rf CPPLINT.cfg

##################### ADDITIONAL TARGETS #####################

.PHONY: clean run_cat test_cat cppcheck

# Static analysis using cppcheck
cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_C) $(SRC_H)

# Tests for cat
test_cat: $(TARGET_CAT)
	bash ./test_cat.sh

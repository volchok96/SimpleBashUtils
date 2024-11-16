##################### VARIABLES #####################

TARGET_CAT = src/cat/s21_cat
TARGET_GREP = src/grep/s21_grep
CC = gcc
FLAGS = -Wall -Werror -Wextra -std=c11
LFLAGS = # -fsanitize=address -g

SRC_DIR_CAT = src/cat
SRC_DIR_GREP = src/grep
SRC_C_CAT = $(wildcard $(SRC_DIR_CAT)/*.c)
SRC_C_GREP = $(wildcard $(SRC_DIR_GREP)/*.c)
SRC_H_CAT = $(wildcard $(SRC_DIR_CAT)/*.h)
SRC_H_GREP = $(wildcard $(SRC_DIR_GREP)/*.h)
OBJ_CAT = $(patsubst $(SRC_DIR_CAT)/%.c, $(SRC_DIR_CAT)/%.o, $(SRC_C_CAT))
OBJ_GREP = $(patsubst $(SRC_DIR_GREP)/%.c, $(SRC_DIR_GREP)/%.o, $(SRC_C_GREP))
INC_CAT = -I$(SRC_DIR_CAT)
INC_GREP = -I$(SRC_DIR_GREP)

##################### TARGETS #####################

# Main targets
all: s21_cat s21_grep

s21_cat: clean stylecor $(TARGET_CAT)
s21_grep: clean stylecor $(TARGET_GREP)

# Create executable file for cat
$(TARGET_CAT): $(OBJ_CAT)
	$(CC) $(LFLAGS) $(FLAGS) $(OBJ_CAT) -o $(TARGET_CAT)

# Create executable file for grep
$(TARGET_GREP): $(OBJ_GREP)
	$(CC) $(LFLAGS) $(FLAGS) $(OBJ_GREP) -o $(TARGET_GREP)

# Rule for compiling .o from .c in src/cat/
$(SRC_DIR_CAT)/%.o: $(SRC_DIR_CAT)/%.c
	$(CC) $(LFLAGS) $(FLAGS) $(INC_CAT) -c $< -o $@

# Rule for compiling .o from .c in src/grep/
$(SRC_DIR_GREP)/%.o: $(SRC_DIR_GREP)/%.c
	$(CC) $(LFLAGS) $(FLAGS) $(INC_GREP) -c $< -o $@

##################### STYLE AND CLEANUP #####################

# Check code style without fixing
style:
	clang-format -n $(SRC_C_CAT) $(SRC_H_CAT) $(SRC_C_GREP) $(SRC_H_GREP)

# Fix code style
stylecor:
	clang-format -i $(SRC_C_CAT) $(SRC_H_CAT) $(SRC_C_GREP) $(SRC_H_GREP)

# Clean up built files
clean:
	rm -rf $(TARGET_CAT) $(TARGET_GREP) $(OBJ_CAT) $(OBJ_GREP)
	rm -rf CPPLINT.cfg

##################### ADDITIONAL TARGETS #####################

.PHONY: clean run_cat run_grep test_cat test_grep cppcheck

# Static analysis using cppcheck
cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_C_CAT) $(SRC_H_CAT) $(SRC_C_GREP) $(SRC_H_GREP)

# Run compiled cat with arguments
run_cat: $(TARGET_CAT)
	./$(TARGET_CAT) $(ARGS)

# Run compiled grep with arguments
run_grep: $(TARGET_GREP)
	./$(TARGET_GREP) $(ARGS)

# Tests for cat
test_cat: $(TARGET_CAT)
	bash $(SRC_DIR_CAT)/test_cat.sh

# Tests for grep
test_grep: $(TARGET_GREP)
	bash $(SRC_DIR_GREP)/test_grep.sh

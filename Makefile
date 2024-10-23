##################### VARIABLES #####################

TARGET = s21_cat
CC = gcc
FLAGS = -Wall -Werror -Wextra -std=c11
LFLAGS = # -fsanitize=address -g
SRC_DIR = src/cat
SRC_C = $(wildcard $(SRC_DIR)/*.c)
SRC_H = $(wildcard $(SRC_DIR)/*.h)
OBJ = $(patsubst $(SRC_DIR)/%.c, $(SRC_DIR)/%.o, $(SRC_C))  # Compile object files in src/cat/
INC = -I$(SRC_DIR)
TEST_FILES = $(wildcard $(SRC_DIR)/test*.txt)  # All test files in format test*.txt in src/cat

##################### TARGETS #####################

all: clean stylecor $(TARGET)

# Creating the executable file
$(TARGET): $(OBJ)
	$(CC) $(LFLAGS) $(FLAGS) $(OBJ) -o $(SRC_DIR)/$(TARGET)

# Rule for compiling .o from .c (object files are stored in src/cat/)
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(LFLAGS) $(FLAGS) $(INC) -c $< -o $@

# Checking style without changes
style:
	clang-format -n $(SRC_C) $(SRC_H)

# Correcting code style
stylecor:
	clang-format -i $(SRC_C) $(SRC_H)

.PHONY: clean run test cppcheck

# Cleaning compiled files
clean:
	rm -rf $(SRC_DIR)/$(TARGET) $(SRC_DIR)/*.o
	rm -rf CPPLINT.cfg

# Static analysis with cppcheck
cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_C) $(SRC_H)

# Running the compiled executable with arguments
run: all
	./$(SRC_DIR)/$(TARGET) $(ARGS)

# Running tests with predefined files from src/cat directory
test: 
	bash src/cat/test_cat.sh
#####################VARIABLES#####################

TARGET = s21_cat
CC = gcc
FLAGS = -Wall -Werror -Wextra -std=c11
LFLAGS = -fsanitize=address -g
SRC_DIR = src/cat
SRC_C = $(wildcard $(SRC_DIR)/*.c)
SRC_H = $(wildcard $(SRC_DIR)/*.h)
OBJ = $(patsubst %.c, %.o, $(SRC_C))
INC = -I$(SRC_DIR)

#####################TARGETS#####################

all: clean $(TARGET)

# Target for creating the binary file
$(TARGET): $(OBJ)
	$(CC) $(LFLAGS) $(FLAGS) $(OBJ) -o $(TARGET)

# Rule for compiling .o from .c
%.o: %.c 
	$(CC) $(LFLAGS) $(FLAGS) $(INC) -c $< -o $@

# Style check without changes
style:
	clang-format -n $(SRC_C) $(SRC_H)

# Fixing the style in code
stylecor:
	clang-format -i $(SRC_C) $(SRC_H)

.PHONY: clean 

# Cleaning up compiled files
clean:
	rm -rf $(TARGET) $(OBJ)
	rm -rf CPPLINT.cfg

# Static analysis with cppcheck
cppcheck:
	cppcheck --enable=all --suppress=missingIncludeSystem $(SRC_C) $(SRC_H)

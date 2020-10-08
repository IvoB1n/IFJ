CFLAGS=-std=gnu99 -Wall -Wextra
CC=gcc
TARGET=add_target
EXECUTABLE=add_executable

all:
	@echo '*** Please, rewrite Makefile with valid TARGET and EXECUTABLE names ***'
#   @$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)
clean:
	rm -rf $(EXECUTABLE) *.o *.out
CFLAGS=-std=gnu99 -Wall -Wextra
CC=gcc
TARGET=add_target

all:
	@echo '*** Please, rewrite Makefile with valid TARGET ***'
#   @$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)
clean:
	rm -rf *.o
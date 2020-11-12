CFLAGS=-std=gnu99 -O2 -Wall -Wextra
CC=gcc
EXECUTABLE=compiler

all:
	@$(CC) $(CFLAGS) main.c -o main.o -c
	@$(CC) $(CFLAGS) scanner.c -o scanner.o -c
	@$(CC) -o compiler main.o scanner.o
	
clean:
	rm -rf $(EXECUTABLE) *.o *.out
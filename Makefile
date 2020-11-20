CFLAGS=-std=gnu99 -O2 -Wall -Wextra
CC=gcc

all:
	$(CC) $(CFLAGS) main.c -o main.o -c
	$(CC) $(CFLAGS) scanner.c -o scanner.o -c
	$(CC) $(CFLAGS) parser.c -o parser.o -c
	$(CC) $(CFLAGS) token_dll.c -o token_dll.o -c
	$(CC) $(CFLAGS) symtable.c -o symtable.o -c
	$(CC) $(CFLAGS) expression.c -o expression.o -c
	$(CC) $(CFLAGS) -o compiler main.o scanner.o parser.o token_dll.o symtable.o expression.o
	
clean:
	rm -rf compiler *.o *.out

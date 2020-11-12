CFLAGS=-std=gnu99 -Wall -Wextra
CC=gcc

all:
	$(CC) $(CFLAGS) main.c -o main.o -c
	$(CC) $(CFLAGS) scanner.c -o scanner.o -c
	$(CC) $(CFLAGS) parser.c -o parser.o -c
	$(CC) $(CFLAGS) dll.c -o dll.o -c
	$(CC) -o compiler main.o scanner.o dll.o parser.o

clean:
	rm -rf $(EXECUTABLE) *.o *.out
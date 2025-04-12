CC = gcc
CFLAGS = -g -Wall -fsanitize=address

all: mysh.c
	$(CC) $(CFLAGS) mysh.c -o mysh
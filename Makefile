CC = gcc
CFLAGS = -g -Wall -std=c99 -fsanitize=address,undefined

all: mysh.c
	$(CC) $(CFLAGS) mysh.c -o mysh
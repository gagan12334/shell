CC = gcc
CFLAGS = -g -Wall -fsanitize=address,undefined

all: mysh.c
	$(CC) $(CFLAGS) mysh.c -o mysh
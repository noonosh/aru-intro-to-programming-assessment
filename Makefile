CC = gcc

all: main.c
	@${CC} main.c -o main -lcurl
	@./main	

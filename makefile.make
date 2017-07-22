CFLAGS = -g -ansi -pedantic -Werror -Wall -I ../../inc/
CC = gcc

#Hash
HashMap.o: HashMap.c HashMap.h
  $(CC) -g -c -o HashMap.o $(FLAGS) -fPIC HashMap.c -lm

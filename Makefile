LIBDIR = /usr/lib
INCLUDEDIR = /usr/include
LIBS = -lncurses
CC = gcc
FLAGS = -O2 -Wall
OBJS = greedy.o

.c.o:
	$(CC)  $(FLAGS) -c $< -o $*.o

greedy: $(OBJS)
	$(CC) $(FLAGS) -lncurses -o greedy

all:: greedy

clean:
	rm ./greedy
	rm *.o

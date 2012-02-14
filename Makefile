CC = gcc
STRIP = strip
FLAGS = -O2 -fomit-frame-pointer -funroll-loops

bindir = /usr/local/bin

greedy:
	$(CC) $(FLAGS) -lncurses -o greedy greedy.c
	$(STRIP) greedy

debug:
	$(CC) -O2 -Wall -lncurses -o greedy greedy.c

all:: greedy

clean:
	rm ./greedy

install:
	cp greedy $(bindir)

uninstall:
	rm $(bindir)/greedy

remove:: uninstall

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
	touch /var/lib/games/greedy.scores
	chown root:games greedy
	chmod u+s greedy
	cp greedy $(bindir)

uninstall:
	rm $(bindir)/greedy
	rm /var/lib/games/greedy.scores

remove:: uninstall

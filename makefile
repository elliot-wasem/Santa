all:
	gcc -o santa santa.c -lncurses -g

clean:
	rm santa

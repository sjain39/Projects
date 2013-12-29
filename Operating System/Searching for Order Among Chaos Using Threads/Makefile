all: orderSearcher

orderSearcher: orderSearcher.c
	gcc orderSearcher.c -o orderSearcher -lpthread -lm

gdb: orderSearcher.c
	gcc -g orderSearcher.c -o orderSearcher -lpthread -lm

clean:
	rm orderSearcher

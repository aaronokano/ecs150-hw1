all: compare connect

compare: p1.c
	gcc -g p1.c -o compare
connect: p2.c
	gcc -g p2.c -o connect

all: compare connect kill_it

compare: p1.c
	gcc -g p1.c -o compare
connect: p2.c
	gcc -g p2.c -o connect
kill_it: p3.c
	gcc -g p3.c -o kill_it

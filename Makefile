all: compare connect kill_it move_files simple_sum

compare: p1.c
	gcc -g p1.c -o compare
connect: p2.c
	gcc -g p2.c -o connect
kill_it: p3.c
	gcc -g p3.c -o kill_it
move_files: p4.c
	gcc -g p4.c -o move_files
simple_sum: p5.c
	gcc -g p5.c -o simple_sum

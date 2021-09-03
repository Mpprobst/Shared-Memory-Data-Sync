all: load clear
load: Load.c header.h
	gcc -Wall -o load Load.c header.c
clear: Clear.c header.h password.h
	gcc -Wall -o clear Clear.c header.c password.c
clean:
	rm -f load clear *.o output.txt

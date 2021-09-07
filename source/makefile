all: load query print clear
load: Load.c header.h
	gcc -Wall -o load Load.c header.c
query: Query.c header.h
	gcc -Wall -o query Query.c header.c
print: Print.c header.h
	gcc -Wall -o print Print.c header.c
clear: Clear.c header.h password.h
	gcc -Wall -o clear Clear.c header.c password.c
clean:
	rm -f load query print clear *.o output.txt

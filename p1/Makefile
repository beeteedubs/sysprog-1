all: libBuild build

clean:
	rm -f mymalloc
libClean: mymalloc.c
	rm -f library.o
libBuild: mymalloc.c libClean
	gcc -c mymalloc.c -Wall
build: memgrind.c clean
	gcc memgrind.c mymalloc.o -o memgrind -Wall

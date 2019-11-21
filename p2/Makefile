CC = gcc
EXECUTABLE_NAME = searchtest

all:
	$(error done goofed, type "make thread" or "make process")

thread:
	$(CC) -lpthread -g searchtest.c multitest_thread.c -o  $(EXECUTABLE_NAME) -Wall

process:
	gcc -g  searchtest.c multitest_proc.c -o  $(EXECUTABLE_NAME) -Wall

clean:
	rm -f searchtest

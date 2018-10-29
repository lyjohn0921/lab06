CC=gcc
CFLAGS=-lWarn -pedantic

libmyifttt.a:	ifttt.o
	ar -rcs libmyifttt.a ifttt.o

ifttt.o: 	ifttt.c ifttt.h
	$(CC) $(CFLAGS) -c -ansi $<

all:	test

test.o: test.c ifttt.h
	$(CC) $(CFLAGS) -c -ansi -lwiringPi $<

test: test.o libmyifttt.a
	cc test.o -L. -lmyifttt -lcurl -lwiringPi -o test

clean:
	rm test *.o

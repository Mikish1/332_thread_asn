#Michael Kish
CC=gcc -Wall
CFLAGS=-I.

output: partB

partB: PartBTest.o petgroomsynch.o
	$(CC) -pthread -o PartB PartBTest.o petgroomsynch.o

clean:
	rm *.o PartB

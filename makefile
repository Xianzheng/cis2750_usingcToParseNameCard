CC = gcc
CFLAGS = -Wall -std=c11 -g -c

all:
	$(CC) $(CFLAGS)  src/LinkedListAPI.c -o llist.o -Iinclude
	$(CC) $(CFLAGS)  src/GEDCOMparser.c -o cparse.o -Iinclude
	ar cr bin/libllist.a llist.o
	ar cr bin/libparser.a cparse.o

list:
	$(CC) $(CFLAGS)  src/LinkedListAPI.c -o llist.o -Iinclude
	ar cr bin/libllist.a llist.o

parser:
	$(CC) $(CFLAGS)  src/GEDCOMparser.c -o cparse.o -Iinclude
	ar cr bin/libparser.a cparse.o
clean:
	rm -f *.o
	rm -f *.so
	rm bin/*.o

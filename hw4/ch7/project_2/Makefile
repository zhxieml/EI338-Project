# makefile for thread pool
#

CC=gcc
CFLAGS=-Wall
PTHREADS=-lpthread

all: main.o buffer.o producer_consumer.o
	$(CC) $(CFLAGS) -o example main.o buffer.o producer_consumer.o $(PTHREADS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(PTHREADS)

buffer.o: buffer.c buffer.h
	$(CC) $(CFLAGS) -c buffer.c $(PTHREADS)

producer_consumer.o: producer_consumer.c producer_consumer.h
	$(CC) $(CFLAGS) -c producer_consumer.c $(PTHREADS)


clean:
	rm -rf *.o
	rm -rf example


CFLAGS = -Wall -Wextra -Werror -Wpedantic -std=c99 -g
CC = gcc
OBJECTS = huffman.o bv.o

.PHONY: all
all: encode decode

encode: $(OBJECTS) encode.o queue.o code.h
	$(CC) $(CFLAGS) -o encode encode.o $(OBJECTS) queue.o
decode: $(OBJECTS) decode.o stack.o 
	$(CC) $(CFLAGS) -o decode decode.o $(OBJECTS) stack.o 
encode.o: encode.c
	$(CC) $(CFLAGS) -c encode.c
decode.o: decode.c
	$(CC) $(CFLAGS) -c decode.c
stack.o: huffman.o stack.c 
	$(CC) $(CFLAGS) -c stack.c
queue.o: queue.c
	$(CC) $(CFLAGS) -c queue.c
bv.o: bv.c
	$(CC) $(CFLAGS) -c bv.c
huffman.o: huffman.c
	$(CC) $(CFLAGS) -c huffman.c
	
.PHONY: clean
clean:
	rm -f encode decode $(OBJECTS) stack.o encode.o decode.o queue.o

CC=gcc
CFLAGS=-c -g -std=c99 -Wall
all: waf

waf: main.o parse_sigs.o header_allow.o req_allow.o
	$(CC) -Wall -std=c99 main.o parse_sigs.o header_allow.o req_allow.o -o waf

main.o: main.c
	$(CC) $(CFLAGS) main.c

parse_sigs.o: parse_sigs.c
	$(CC) $(CFLAGS) parse_sigs.c

header_allow.o: header_allow.c
	$(CC) $(CFLAGS) header_allow.c

req_allow.o: req_allow.c
	$(CC) $(CFLAGS) req_allow.c

clean:
	rm *o waf

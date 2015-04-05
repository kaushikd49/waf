all: waf

waf: main.o parse_sigs.o header_allow.o req_allow.o
	gcc main.o parse_sigs.o header_allow.o req_allow.o -o waf

main.o: main.c
	gcc -c main.c

parse_sigs.o: parse_sigs.c
	gcc -c parse_sigs.c

header_allow.o: header_allow.c
	gcc -c header_allow.c

req_allow.o: req_allow.c
	gcc -c req_allow.c

clean:
	rm *o waf

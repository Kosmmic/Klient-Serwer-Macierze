all: serwer klient

serwer: server.c common.h
	gcc server.c -o serwer

klient: client.c common.h
	gcc client.c -o klient -lncurses

clean: 
	rm -f serwer klient

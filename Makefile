all: serwer klient

serwer: serwer.c common.h
	gcc server.c -o serwer

klient: klient.c common.h
	gcc client.c -o klient -lncurses

clean: 
	rm -f serwer klient

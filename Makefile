all: serwer klient

serwer: serwer.c wspolny.h
	gcc serwer.c -o serwer

klient: klient.c wspolny.h
	gcc klient.c -o klient -lncurses

clean: 
	rm -f serwer klient

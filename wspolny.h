//wspolny.h
#ifndef WSPOLNE_H
#define WSPOLNE_H
#include <sys/types.h>

#define KLUCZ_KOLEJKI 123456
#define MAX_M 10
#define MAX_N 10


struct msg_do_serwera {
	long mtype;
	pid_t klient_pid;
	int m;
	int n;
	double tablica[MAX_M][MAX_N];
};

struct msg_do_klienta {
	long mtype;
	double suma;
};

#endif

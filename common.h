//common.h
#ifndef COMMON_H
#define COMMON_H
#include <sys/types.h>

#define QUEUE_KEY	 123456
#define MAX_M 10
#define MAX_N 10


struct msg_to_server {
	long mtype;
	pid_t client_pid;
	int m;
	int n;
	double tablica[MAX_M][MAX_N];
};

struct msg_to_client {
	long mtype;
	double total_sum;
};

#endif

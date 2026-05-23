//serwer.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h> 
#include "wspolny.h"

void obsluga_zombie(int signum){
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
	int msgid;
	struct msg_do_serwera msg_odebrany;
	struct msg_do_klienta msg_zwrotny;
	double suma;
	//Automatyczne czyszczenie procesow potomnych 
	signal(SIGCHLD, obsluga_zombie);
	
	//Utworzenie kolejki komunikatow
	msgid = msgget(KLUCZ_KOLEJKI, IPC_CREAT | 0666);
	if( msgid == -1){
		perror("Blad podczas tworzenia kolejki komunikatow\n");
		exit(1);
	}

	printf("Wspolbiezny serwer uruchomiony. Czekam na klientow...\n");
	
	while (1){
		if (msgrcv(msgid, &msg_odebrany, sizeof(msg_odebrany) - sizeof(long), 1, 0) == -1){
			if (errno == EINTR) {
			        printf("[Serwer-Rodzic] Zostałem przerwany przez obsługę zombie (błąd EINTR). Wracam do pracy!\n");
			        continue; 
			 }
			 perror("Inny, krytyczny błąd");
			 exit(1);
		}

		pid_t pid = fork();
		if (pid == -1){
			printf("Blad fork()");
			continue;
		}

		if(pid ==0){
			//kod procesu potomnego

			printf("[Serwer-Dziecko %d] Odebrano dane od klienta o PID: %d\n", getpid(), (int)msg_odebrany.klient_pid);
			suma = 0.0;
			for(int i=0;i<msg_odebrany.m;i++){
				for(int j=0;j<msg_odebrany.n;j++){
					suma+= msg_odebrany.tablica[i][j];
				}
			}

			msg_zwrotny.mtype = msg_odebrany.klient_pid;
			msg_zwrotny.suma = suma;

			if (msgsnd(msgid, &msg_zwrotny, sizeof(msg_zwrotny) - sizeof(long), 0) == -1){
				perror("[SERWER-Dziecko] Blad msgsnd");
				exit(1);
			}
			printf("[Serwer-Dziecko %d] Wynik (%f) odeslany. Koncze prace.\n", getpid(), suma);
			exit(0);
		}
	}


	return 0;
}

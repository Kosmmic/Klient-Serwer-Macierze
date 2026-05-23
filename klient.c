//klient.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ncurses.h> 
#include "wspolny.h"

int main() {
	int msgid;
	struct msg_do_serwera moj_komunikat;
	struct msg_do_klienta komunikat_zwrotny;
	pid_t moj_pid = getpid();

	initscr();
	cbreak();

	mvprintw(1,2, "===KLIENT IPC Z INTERFEJSEM NCURSES (PID: %d) ===", moj_pid);
	refresh();

	msgid = msgget(KLUCZ_KOLEJKI, 0666);
	if(msgid == -1){
		endwin();
		fprintf(stderr,"Blad, nie mozna polaczyc sie z kolejka. Czy serwer jest uruchomiony?\n");
		exit(1);
	}

	mvprintw(3,2, "Podaj liczbe wierzy M (max %d): ", MAX_M);
	refresh();
	echo();
	scanw("%d", &moj_komunikat.m);
	
	mvprintw(4,2, "Podaj liczbe kolumn N (max %d): ", MAX_N);
	refresh();
	scanw("%d", &moj_komunikat.n);

	if (moj_komunikat.m <=0 || moj_komunikat.m > MAX_M || moj_komunikat.n <=0 || moj_komunikat.n > MAX_N){
		mvprintw(6,2, "Bledne wymiary macierzy! Nacisnij klawisz, aby wyjsc...");
		refresh();
		noecho();
		getch();
		endwin();
		exit(1);
	}

	mvprintw(6,2, "Wprowadz elementy macierzy:");
	int linia =7;
	for(int i=0; i<moj_komunikat.m;i++){
		for(int j=0;j<moj_komunikat.n;j++){
			mvprintw(linia, 4, "Element [%d][%d]: ", i, j);
			refresh();
			scanw("%lf", &moj_komunikat.tablica[i][j]);
			linia++;
		}
	}
	noecho();

	moj_komunikat.mtype=1;
	moj_komunikat.klient_pid = moj_pid;

	mvprintw(linia+1, 2, "Wysylam dane do serwera i czekam na wynik...");
	refresh();

	if(msgsnd(msgid, &moj_komunikat, sizeof(moj_komunikat)-sizeof(long), 0) == -1){
		endwin();
		perror("Blad wysylania");
		exit(1);
	}
	if(msgrcv(msgid, &komunikat_zwrotny, sizeof(komunikat_zwrotny) - sizeof(long), moj_pid, 0) == -1){
		endwin();
		perror("Blad odbierania wyniku");
		exit(1);
	}

	clear();
	mvprintw(2,2, "===WYNIK PRZETWARZANIA SERWERA===");
	mvprintw(4,2, "Orzymano odpowiedz dla klienta o PID: %d", moj_pid);
	mvprintw(5,2, "Obliczona przez wspolbiezny serwer suma wynosi: %f", komunikat_zwrotny.suma);
	mvprintw(7, 2, "Nacisnij dowolny klawisz, aby zamknac program...");
	refresh();

	getch();
	endwin();
	return 0;
}

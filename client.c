//client.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ncurses.h> 
#include "common.h"

int main() {
	int msgid;
	struct msg_to_server my_message;
	struct msg_to_client response_message;
	pid_t my_pid = getpid();

	initscr();
	cbreak();

	mvprintw(1,2, "=== IPC CLIENT WITH NCURSES TUI (PID: %d) ===", my_pid);
	refresh();

	msgid = msgget(QUEUE_KEY, 0666);
	if(msgid == -1){
		endwin();
		fprintf(stderr,"Error: Cannot connect to the message queue. Is the server running?\n");
		exit(1);
	}

	mvprintw(3,2, "Podaj liczbe wierzy M (max %d): ", MAX_M);
	refresh();
	echo();
	scanw("%d", &my_message.m);
	
	mvprintw(4,2, "Podaj liczbe kolumn N (max %d): ", MAX_N);
	refresh();
	scanw("%d", &my_message.n);

	if (my_message.m <=0 || my_message.m > MAX_M || my_message.n <=0 || my_message.n > MAX_N){
		mvprintw(6,2, "Bledne wymiary macierzy! Nacisnij klawisz, aby wyjsc...");
		refresh();
		noecho();
		getch();
		endwin();
		exit(1);
	}

	mvprintw(6,2, "Wprowadz elementy macierzy:");
	int line =7;
	for(int i=0; i<my_message.m;i++){
		for(int j=0;j<my_message.n;j++){
			mvprintw(line, 4, "Element [%d][%d]: ", i, j);
			refresh();
			scanw("%lf", &my_message.tablica[i][j]);
			line++;
		}
	}
	noecho();

	my_message.mtype=1;
	my_message.client_pid = my_pid;

	mvprintw(line+1, 2, "Wysylam dane do serwera i czekam na wynik...");
	refresh();

	if(msgsnd(msgid, &my_message, sizeof(my_message)-sizeof(long), 0) == -1){
		endwin();
		perror("Blad wysylania");
		exit(1);
	}
	if(msgrcv(msgid, &response_message, sizeof(response_message) - sizeof(long), my_pid, 0) == -1){
		endwin();
		perror("Blad odbierania wyniku");
		exit(1);
	}

	clear();
	mvprintw(2,2, "===WYNIK PRZETWARZANIA SERWERA===");
	mvprintw(4,2, "Orzymano odpowiedz dla klienta o PID: %d", my_pid);
	mvprintw(5,2, "Obliczona przez wspolbiezny serwer suma wynosi: %f", response_message.total_sum);
	mvprintw(7, 2, "Nacisnij dowolny klawisz, aby zamknac program...");
	refresh();

	getch();
	endwin();
	return 0;
}

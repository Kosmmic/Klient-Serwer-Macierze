//server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h> 
#include "common.h"

// Signal handler to automatically clean up zombie child processes
void handle_zombie(int signum){
	while(waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
	int msgid;
	struct msg_to_server received_msg;
	struct msg_to_client response_msg;
	double total_sum;

	// Register signal handler to eliminate zombie processes
	signal(SIGCHLD, handle_zombie);
	
	// Create the system message queue
	msgid = msgget(QUEUE_KEY, IPC_CREAT | 0666);
	if( msgid == -1){
		perror("Error creating system message queue\n");
		exit(1);
	}

	printf("Concurrent IPC Server is running. Waiting for clients...\n");
	
	while (1){
		// Fetch computation request from the queue (mtype = 1)
		if (msgrcv(msgid, &received_msg, sizeof(received_msg) - sizeof(long), 1, 0) == -1){
			// Handle interrupt by the SIGCHLD signal handler safely
			if (errno == EINTR) {
			      	printf("[Server-Parent] Interrupted by zombie cleaner (EINTR error). Resuming loop...\n");
			        continue; 
			 }
			 perror("Other, critical error");
			 exit(1);
		}

		// Fork a child process to handle concurrent matrix processing
		pid_t pid = fork();
		if (pid == -1){
			printf("Failed to create child process via fork()");
			continue;
		}

		if(pid ==0){
			// --- Child Process Context ---

			printf("[Server-Child %d] Received data from client with PID: %d\n", getpid(), (int)received_msg.klient_pid);
			total_sum = 0.0;
			for(int i=0;i<received_msg.m;i++){
				for(int j=0;j<received_msg.n;j++){
					total_sum+= received_msg.tablica[i][j];
				}
			}

			// Route response back specifically to the requesting client's PID
			response_msg.mtype = received_msg.klient_pid;
			response_msg.total_sum = total_sum;

			if (msgsnd(msgid, &response_msg, sizeof(response_msg) - sizeof(long), 0) == -1){
				perror("[Server-Child] IPC send error via msgsnd");
				exit(1);
			}
			 printf("[Server-Child %d] Result (%f) transmitted successfully. Terminating process.\n", getpid(), total_sum);
			exit(0);
		}
	}


	return 0;
}

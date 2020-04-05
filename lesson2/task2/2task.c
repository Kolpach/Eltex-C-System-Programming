#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#define MESSAGE_SIZE 100
void waitForInput(int fread) {
	char message[MESSAGE_SIZE];
	while(1) {
		scanf("%s", message);//reading message
		write(fread, message, MESSAGE_SIZE);//sending massage to interlocutor
		if(message[0] == 'e' && message[1] =='x' && message[2] == 'i' &&  message[3] == 't') {
			printf("Exited!\n");
			break;
		}
	}
}
int main(int argc, char* argv[]) {
	if(argc < 2) {
		printf("Not enogth parametrs. Your ID is missing.\n");
		return 0;
	}
	if(argc < 3) {
		printf("Not enogth parametrs. Interlocutors ID is missing.\n");
		return 0;
	}
	char* yourId = argv[1];
	char* interlocutorId = argv[2];
	// создание двух FIFO, OK. если они существуют 
	if ((mkfifo(yourId, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0) && (errno != EEXIST))
	{
		printf("can't create %s", yourId);
		exit(-1);
	}
	if ((mkfifo(interlocutorId, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0) && (errno != EEXIST)) {
		unlink(yourId);
		printf("can't create %s", interlocutorId);
		exit(-2);
	}
	int readfd, writefd;
	printf("I'm writing to %s\n", yourId);
	printf("I'm reading from %s\n", interlocutorId);
	switch(strcmp(yourId, interlocutorId)){
		case 1:
			printf("Waiting for interlocutor\n");
			writefd = open(yourId,  O_WRONLY);
			readfd = open(interlocutorId, O_RDONLY);
			break;
		case 0:
			printf("your and interlocutor Id can't match\n");
			exit(0);
		case -1:
			printf("Waiting for interlocutor\n");
			readfd = open(interlocutorId, O_RDONLY);
			writefd = open(yourId,  O_WRONLY);
			break;
	}
	printf("-----\nStart chat\n");

	pid_t pid = fork();
	switch(pid){
		case -1:
			printf("Error in fork()\n");
			break;
		case 0:
		//child
			waitForInput(writefd);
			exit(1);
		default:
		//parent
			break;
	}
	char buf[MESSAGE_SIZE];
	int status = 0;
	while(1){
		int countSym = read(readfd, buf, MESSAGE_SIZE);
		if(countSym){
			if(buf[0] == 'e' && buf[1] =='x' && buf[2] == 'i' &&  buf[3] == 't') {
				write(writefd, buf, MESSAGE_SIZE);
				printf("interlocutor disconnected.\n");
				break;
			}
			printf("interlocutor> %s\n", buf);
		}
	}
	close(readfd);
	close (writefd);
	unlink(yourId);
	unlink(interlocutorId);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/msg.h>

#include "msgbuf.h"
#define MAX_USERS_COUNT 100
/*Чтение для владельца 	0400
Запись для владельца 	0200
Чтение для группы 	0040
Запись для группы 	0020
Чтение для остальных 	0004
Запись для остальных 	0002*/

int getNumber(char* text) {
	int count = 0;
	for(int i = 0; i < strlen(text); ++i){
		if(text[i] >= '0' && text[i] <= '9'){
			count *= 10;
			count += (text[i] - '0');
		}
		else {
			printf("Wrong parametr. Must be integer\n");
			break;
		}
	}
	return count;
}
void waitForExit() {
	char let;
	scanf("%c", &let);
}
int main(){
	int idGenerator = 3;
	key_t queueKey = ftok("msgbuf.h", idGenerator);
	int users[2][MAX_USERS_COUNT];
	int currentUser = 0;
	int msgID;
	if((msgID = msgget(queueKey, 0)) > 0) {
		if ( msgctl(msgID, IPC_RMID, NULL) < 0 )
		{
			perror("msgctl");
		}
	}
	msgID = msgget(queueKey, IPC_CREAT | 0622);
	if(msgID == -1) {
		perror("msgget can't create new.\n");
		return 0;
	}
	printf("key is %d, queue ID is %d\n", queueKey, msgID);
	struct msgbuf buf;
	
	pid_t pid = fork();
	if(pid == 0){
		waitForExit();
		return 0;
	}
	int status = 0;
	while(waitpid(pid, &status, WNOHANG) == 0) {
		int res;
		 
		if( (res = msgrcv(msgID, &buf, sizeof(buf) - sizeof(long), 0, IPC_NOWAIT)) == -1 && errno == ENOMSG) {
			// No message
			sleep(1);
			continue;
		}
		if(res == -1) {
			printf("error in reciving message %d\n", errno);
			break;
		}
		//if got somthing
		//if this is new user
		if(buf.mtype == 2) {
			users[1][currentUser] = getNumber(buf.id);
			if((users[0][currentUser++] = open(buf.id, O_WRONLY)) == -1 ) {
				perror("Can't add user\n");
			}
		}
		//if user left server
		if(buf.mtext[0] == 'e' && buf.mtext[1] =='x' && buf.mtext[2] == 'i' &&  buf.mtext[3] == 't') {
		 	for(int i = 0; i < currentUser; ++i) {
				if(users[1][i] ==  getNumber(buf.id)) {
					users[1][i] = -1;
					close(users[0][i]);
				}
			}
		}
		else{
			printf("%s\n", buf.mtext);
			//sending message to each user
			for(int i = 0; i < currentUser; ++i) {
				//sending to everyone except sender
				if(users[1][i] !=  getNumber(buf.id) && users[1][i] != -1) {
					if(write(users[0][i], buf.mtext, MSGBUF_SIZE) == -1)
						perror("Can't send messge to user\n");
				}
			}

		}
	}
	printf("Exited.\n");
	char exitMessage[] = "exit";
	for(int i = 0; i < currentUser; ++i) {
		if(users[1][i] !=  0) {
			users[1][i] = 0;
			write(users[0][i], exitMessage, 5);
			close(users[0][i]);
		}
	}
	msgctl(msgID, IPC_RMID, NULL);
	return 0;
}
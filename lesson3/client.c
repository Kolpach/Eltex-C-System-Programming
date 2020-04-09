#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "msgbuf.h"
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
void waitForInput(int msgID, char id[]) {
	struct msgbuf buf;
	buf.mtype = 1;
	strcpy(buf.id, id);
	while(1){
        scanf("%s", buf.mtext);
        int res = msgsnd(msgID, &buf, sizeof(buf)- sizeof(long), 0);
		if(buf.mtext[0] == 'e' && buf.mtext[1] =='x' && buf.mtext[2] == 'i' &&  buf.mtext[3] == 't') {
			break;
		}
		if(res < 0) {
			printf("error in sending message %d\n", errno);
			break;
		}
	}
}

int main(){
	//key for sending
	int idGenerator = 3;
	key_t queueKey = ftok("msgbuf.h", idGenerator);
	int msgID = msgget(queueKey, 0);
	if(msgID == -1){
		perror("can't get message ID\n");
		return 0;
	}
	printf("key is %d, queue ID is %d\n", queueKey, msgID);

	
	//let id be a pid in a char form
	char myID[20];
	int myPid = getpid();
	int sym_count = 0;
	int foo = myPid;
	for(sym_count = 0; foo > 0; ++sym_count, foo/=10);
	myID[sym_count] = '\0';
	for(;sym_count > 0; --sym_count, myPid /= 10) {
		myID[sym_count-1] = (char)(myPid %10 + '0');
	}
	printf("I HAVE ID: %s\n", myID);

	//fifo for reciving
	if ((mkfifo(myID, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH) < 0) && (errno != EEXIST))
	{
		printf("can't create %s", myID);
		exit(-1);
	}
	int readfd;
	readfd = open(myID, O_RDONLY | O_NONBLOCK);

	printf("-----\nStart chat\n");
	//sending start signal
	struct msgbuf locBuf;
	locBuf.mtype = 2;
	strcpy(locBuf.id, myID);
	strcpy(locBuf.mtext, "--new-member-entered--");
	int res = msgsnd(msgID, &locBuf, sizeof(locBuf)- sizeof(long), 0);
	if(res < 0) {
		printf("error in sending message %d\n", errno);
	}

	//reading and writing are parallel process
	pid_t pid = fork();
	switch(pid){
		case -1:
			printf("Error in fork()\n");
			break;
		case 0:
		//child
			waitForInput(msgID, myID);//writing
			exit(1);
		default:
		//parent
			break;
	}

	//reciving lines from server
	char readBuf[MSGBUF_SIZE];
	int status = 0;
	while(waitpid(pid, &status, WNOHANG) == 0){
		int res;
		if((res = read(readfd, readBuf, MSGBUF_SIZE)) == -1 && errno != EAGAIN){
			printf("can't read %d\n", errno);
		}
		if(res > 0){
			if(readBuf[0] == 'e' && readBuf[1] =='x' && readBuf[2] == 'i' &&  readBuf[3] == 't') {
				kill(pid, SIGKILL);
				break;
			}
			printf("%s\n", readBuf);
		}
	}
	close(readfd);
	unlink(myID);
	printf("Exited.\n");
	return 0;
}
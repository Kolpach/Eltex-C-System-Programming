#include <sys/msg.h>
#define MSGBUF_SIZE 100
struct msgbuf {
    long mtype; /* тип сообщения, должен быть > О */ 
	char mtext[MSGBUF_SIZE]; /* данные */
    char id[20];
};
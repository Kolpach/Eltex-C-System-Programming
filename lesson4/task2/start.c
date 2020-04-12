#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <ctype.h>
#include <dirent.h>
union semnum
{
	int val;
	struct semid_ds *buf;
	short *array;
};
//locking the semaphore
void my_lock(int semid) {
	struct sembuf sbuf = {0, -1, 0};
	if(semop(semid, &sbuf, 1) != 0)
		printf("Error in Lock\n");
}
//unlocking the semaphore
void my_unlock(int semid) {
	struct sembuf sbuf = {0, 1, 0};
	if(semop(semid, &sbuf, 1) != 0)
		printf("Error in Unlock\n");
}
//setting the semaphore value
void my_setValue(int semid, int value) {
	union semnum arg;
	arg.val=value;
	if(semctl(semid, 0, SETVAL, arg) < 0) {
		perror("semctl\n");
	}

}
//check file for being executable
int checkExec(char* name) {
	FILE *fp;
	const char *first  = "/bin/test -x \"";
	const char *second = "\" && echo 1 || echo 0";

	char command[150];
	snprintf(command, sizeof command, "%s%s%s", first, name, second);
	fp = popen(command, "r");
	if (fp == NULL) {
		printf("Failed to run command\n" );
		return -1;
	}
	char let = fgetc(fp);
	pclose(fp);
	return let - '0';
}
int normalFork(struct dirent * file, int key) {
	pid_t pid = fork();
	switch(pid) {
		case -1:
			printf("Error in normalFork function.\n");
			exit(NULL);
			break;
		case 0: {
			//this is child
			// Access to semaphore
			int semid;
			if((semid = semget(key, 0, 0)) == -1) {
				perror("semget child\n");
				exit(1);
			}
			//locking semaphore
			my_lock(semid);
			/* Получим область разделяемой памяти */
			int id;
			if ((id = shmget(key, 0, 0)) < 0) {
				perror("shmget main");
				exit(1);
			}
			/* Получим доступ к разделяемой памяти */
			int* segment;
			if ((segment = shmat(id, NULL, 0)) == (int *) -1) {
				perror("shmat error");
				exit(1);
			}

			//reading and counting symbols
			int countOfSym = 0;
			FILE *fd = fopen(file->d_name, "r");
			while(!feof(fd)) {
				char a = fgetc(fd);
				++countOfSym;
				++(*segment);
			}
			printf("%s  %d\n", file->d_name, countOfSym);

			shmdt(segment);
			//unlocking semaphore
			my_unlock(semid);
			exit(1);
		}
		default:
			//this is parent;
			return pid;
	}
}
int main(int argc, char * argv[]) {
	int id = 0;
	key_t key = ftok(".", 'a');
	 /* Создадим область разделяемой памяти */
	if ((id = shmget(key, sizeof(int), IPC_CREAT | 0666)) < 0) {
		perror("shmget main\n");
		exit(1);
	}
	/* Получим доступ к разделяемой памяти */
	int* segment;
	if ((segment = shmat(id, NULL, 0)) == (int *) -1) {
		perror("shmat error\n");
		exit(1);
	}
	//setting sym count to zero
	*segment = 0;
	shmdt(segment);

	int semid;
	if((semid=semget(key, 1, IPC_CREAT | 0666)) == -1)
	{
		perror("semget");
		exit(1);
	}
	//set shemaphore value
	my_setValue(semid, 1);

	//open directory
	DIR * directory;
	struct dirent * file;
	directory = opendir("./");
	if (directory == NULL)
		perror ("Couldn't open the directory");

	int countOfSym = 0;
	//search for all files in directory
	while (file = readdir (directory)){
		//getting files full path
		char dirPathName[200];
		getcwd(dirPathName, 200);
		char fullPathName[250];
		snprintf(fullPathName, sizeof fullPathName, "%s%c%s", dirPathName, '/', file->d_name);

		//check for not being executable and be a regular file
		if(checkExec(fullPathName) || file->d_type != DT_REG) {
			continue;
		}
		//child will calculate number of symbols
		normalFork(file, key);
	}
	
	(void) closedir (directory);

	int status;
	wait(&status);

	if((segment = shmat(id, NULL, 0)) == (int *) -1){
		perror("shmat error\n");
		exit(1);
	}
	printf("------\n%d\n", *segment);
	shmdt(segment);

	shmctl(id, IPC_RMID, NULL);
	semctl(semid, 0, IPC_RMID);
    return 0;
}
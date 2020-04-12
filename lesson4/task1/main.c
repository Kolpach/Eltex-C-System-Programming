#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/wait.h>

void numToChar(int number, char str[11]) {
	int sym_count = 0;	
	for(int foo = number; foo > 0; ++sym_count, foo/=10);
	str[sym_count] = '\0';
	for(;sym_count > 0; --sym_count, number /= 10) {
		str[sym_count-1] = (char)(number %10 + '0');
	}
}

int normalFork(int key, int task) {
	pid_t pid = fork();
	switch(pid) {
		case -1:
			printf("Error in normalFork function.\n");
			exit(NULL);
			break;
		case 0: {
			//this is child
			char keyS[11], taskS[11];
			numToChar(key, keyS);
			numToChar(task, taskS);
			char* argv[] = {"child.c", keyS, taskS, NULL};
			execv("child", argv);
			exit("OK");
		}
		default:
			//this is parent;
			return pid;
	}
}

void create_index(void **m, int rows, int cols, size_t sizeElement){
    int i;  
    size_t sizeRow = cols * sizeElement;
    m[0] = m+rows;
    for(i=1; i<rows; i++){      
        m[i] = (m[i-1]+sizeRow);
    }
}

int main(int argc, char* argv[]) {

	key_t key = ftok("main.c", 1);
	int id = 0;
	 /* Создадим область разделяемой памяти */
	if ((id = shmget(key, sizeof(int) * 25, IPC_CREAT | 0666)) < 0) {
		perror("shmget main");
		exit(1);
	}
	/* Получим доступ к разделяемой памяти */
	int** matrix;
	//int* s;
	if ((matrix = shmat(id, NULL, 0)) == (int **) -1) {
		perror("shmat main");
		exit(1);
	}
	//matrix = (int **)s;
	create_index(matrix, 5, 5, sizeof(int));
	//int matrix[5][5];
	srand(time(0));
	rand();

	//filling matrix
	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 5; ++j) {
			matrix[i][j] = rand() % 100;
		}
	}

	//show matrix
	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 5; ++j) {
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}

	pid_t pid1, pid2, pid3, pid4;
	int status1, status2, status3, status4;

	pid1 = normalFork(key, 1);
	pid2 = normalFork(key, 2);
	pid3 = normalFork(key, 3);
	pid4 = normalFork(key, 4);

	shmdt(matrix);
	//waiting all pids to stop working
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	waitpid(pid3, &status3, 0);
	waitpid(pid4, &status4, 0);

	shmctl(id, IPC_RMID, NULL);
	return 0;
}
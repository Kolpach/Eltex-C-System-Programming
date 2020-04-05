#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
char* multiplayStringToNumber(int number, char* str, int* size) {
	if(number < 0) {
		printf("number must be positive\n");
		return str;
	}
	int rest = 0;
	for(int i = 0; i < *size; ++i) {
		//multiply each digit
		int res = (str[i] - '0') * number;
		str[i] = (char)( (res+ rest) % 10  + '0');
		rest = (res + rest) / 10;
	}
	if(rest){
		while(rest > 0) {
			str[*size] = (char)(rest % 10 + '0');
			(*size) ++;
			rest /= 10;
		}
	}
	return str;
}
void taskForChild1(int matrix[5][5], int pipeToWrite) {
	int max = 0;
	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 5; ++j) {
			if(max < matrix[i][j]) {
				max = matrix[i][j];
			}
		}
	}
	//printf("%d", max);
	write(pipeToWrite, &max, sizeof(int));
	close(pipeToWrite);
}
void taskForChild2(int matrix[5][5], int pipeToWrite) {
	int min = 100;
	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 5; ++j) {
			if(min > matrix[i][j]) {
				min = matrix[i][j];
			}
		}
	}
	write(pipeToWrite, &min, sizeof(int));
	close(pipeToWrite);
}
void taskForChild3(int matrix[5][5], int pipeToWrite) {
	int sum = 0;
	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 5; ++j) {
			sum += matrix[i][j];
		}
	}
	write(pipeToWrite, &sum, sizeof(int));
	close(pipeToWrite);
}
void taskForChild4(int matrix[5][5], int pipeToWrite) {
	//task4

	//store large number as a string
	int delta = 10;
	int sizeOfNumString = delta;
	char* numInTextForm = malloc(sizeof(char) * sizeOfNumString);
	int trueNumSize = 1;
	numInTextForm[0] = '1';
	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 5; ++j) {
			int number = matrix[i][j];
			multiplayStringToNumber(number , numInTextForm, &trueNumSize);
			//hear 2 is max count of digits in number (99 -> 2)
			if(trueNumSize + 2 >= sizeOfNumString) {
				sizeOfNumString += delta;
				numInTextForm = realloc(numInTextForm, sizeOfNumString);
			}
		}
	}

	//deleting zero numbers at the begining;
	while(numInTextForm[trueNumSize - 1] == '0' && trueNumSize > 1) {
		--trueNumSize;
	}
	numInTextForm = realloc(numInTextForm, trueNumSize + 1);
	numInTextForm[trueNumSize] = '\0';

	//reverse string
	for(int i = 0, j = trueNumSize - 1; i < j; --j, ++i) {
		int buf = numInTextForm[j];
		numInTextForm[j] = numInTextForm[i];
		numInTextForm[i] = buf;
	}

	write(pipeToWrite, numInTextForm, sizeof(char) * (trueNumSize + 1) );
	free(numInTextForm);
	close(pipeToWrite);
}

int normalFork(void (*function)(int[5][5] , int), int matrix[5][5], int pipeToWrite[2]) {
	pid_t pid = fork();
	switch(pid) {
		case -1:
			printf("Error in normalFork function.\n");
			exit(NULL);
			break;
		case 0:
			//this is child
			close(pipeToWrite[0]);
			(*function)(matrix, pipeToWrite[1]);
			exit("OK");
			printf("this never gona happen");
		default:
			//this is parent;
			close(pipeToWrite[1]);
			return pid;
	}
}

int main(int argc, char* argv[]) {
	int matrix[5][5];
	srand(time(0));
	rand();

	int pipe1[2], pipe2[2], pipe3[2], pipe4[2];
	pipe(pipe1);
	pipe(pipe2);
	pipe(pipe3);
	pipe(pipe4);

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
	
	pid1 = normalFork(&taskForChild1, matrix, pipe1);
	pid2 = normalFork(&taskForChild2, matrix, pipe2);
	pid3 = normalFork(&taskForChild3, matrix, pipe3);
	pid4 = normalFork(&taskForChild4, matrix, pipe4);
	
	//waiting all pids to stop working
	waitpid(pid1, &status1, 0);
	waitpid(pid2, &status2, 0);
	waitpid(pid3, &status3, 0);
	waitpid(pid4, &status4, 0);

	int buf1 = 0, buf2 = 0, buf3 = 0;
	char buf4[100];

	read(pipe1[0], &buf1, sizeof(int));
	close(pipe1[0]);
	read(pipe2[0], &buf2, sizeof(int));
	close(pipe2[0]);
	read(pipe3[0], &buf3, sizeof(int));
	close(pipe3[0]);
	read(pipe4[0], buf4, 100);
	close(pipe4[0]);

	printf("-----\nmax number is %d\n", buf1);
	printf("min number is %d\n", buf2);
	printf("sum is %d\n", buf3);
	printf("product is %s\n", buf4);
	return 0;
}
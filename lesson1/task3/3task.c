#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
void taskForChild(int argc, char* locArgv[]) {
	char* fileName = locArgv[0];
	char* argv[argc-1];
	int i;
	for(i = 0; i < argc - 1; ++i) {
		argv[i] = locArgv[i+1];
	}
	execv(fileName, argv);
}

int normalFork(void (*function)(int, char* []), int argc, char* locArgv[] ) {
	pid_t pid = fork();
	switch(pid) {
		case -1:
			printf("Error in normalFork function.\n");
			break;
		case 0:
			//this is child
			(*function)(argc, locArgv);
			break;
		default:
			//this is parent;
			return pid;
	}
	exit(-1);
}
char* readWordAndMalloc() {
	int wordSize = 10;//must be 1 or greater
	int delta = 10;
	char* word = malloc(sizeof(char) * wordSize);
	char sym ;
	int i = 0;
	//finding begining of word
	do {
		sym = getchar();
	}
	while(sym == '\0' ||  sym == ' ' || sym == '\n' );
	//put first symbol in array
	word[i++] = sym;

	//puts other symbols in array
	do {
		//scanf("%c", &sym);
		sym = getchar();
		if( sym == ' ' || sym == '\n' ) {
			sym = '\0';
		}

		if(i == wordSize) {
			wordSize += delta;
			word = realloc(word, sizeof(char) * wordSize);
		}
		
		word[i++] = sym;

	}
	while(sym != '\0');
	word = realloc(word, i * sizeof(char));
	return word;
}
int main(int mainArgc, char* mainArgv[]) {
	pid_t pid;
	do{
		printf("Choose program to run: 1, 2, 3\n");
		int switcher;
		scanf("%d", &switcher);
		switch(switcher) {
			case 1: {
				printf("Type matrix size\n");
				char* num1 = readWordAndMalloc();
				char* num2 = readWordAndMalloc();

				int count = 5;
				char* argv[] = {"additionals/1", mainArgv[0], num1, num2, NULL};
				pid = normalFork(&taskForChild, count, argv);
				free(num1);
				free(num2);
				break;
			}
			case 2: {
				printf("Type one line of text\n");
				char* line = readWordAndMalloc();
				printf("-----\n");

				int count = 4;
				char* argv[] = {"additionals/2", mainArgv[0], line, NULL};
				pid = normalFork(&taskForChild, count, argv);
				free(line);
				break;
			}
			case 3: {
				printf("Type the number of elements\n");
				int size;
				scanf("%d", &size);
				char ** argv = malloc(sizeof(char*) * (size + 3));
				argv[0] = malloc(sizeof(char) * (strlen("additionals/3") + 1) );
				strcpy(argv[0], "additionals/3");
				argv[1] = mainArgv[0];

				printf("Fill array with numbers\n");
				for(int i = 2, num = 0; i < size + 2; ++i) {
					argv[i] = readWordAndMalloc();
				}

				argv[size + 2] = NULL;
				
				printf("-----\n");

				//pid = normalFork(&taskForChild, size + 2, argv);
				taskForChild(size+ 3, argv);
				for(int i = 0; i < size + 3; ++i) {
					free(argv[i]);
				}
				free(argv);
				break;
			}
			default:
				printf("Wrong input, try again.\n");
				continue;
		}

	}
	while(0);
	int status;
	waitpid(pid, &status, 0);
	return 0;
}
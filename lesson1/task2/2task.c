#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void doit(){
	pid_t pid1, pid2;
	pid_t pid_child1, pid_child2, pid_child3;
	int status1, status2, status3;
	pid1 = fork();//Создаём первого потомка
	
	if (pid1 == 0) {
		printf("This is main child 1 my pid is %d\n", getpid());
		pid_child1 = fork();//Создаём 1 подпотомка
		if( pid_child1 > 0 ) {
			pid_child2 = fork();//Создаём 2 потомка 
			if(pid_child2 > 0) {
				pid_child3 = fork();//Создаём 3 потомка
				if(pid_child3 > 0) {
					if(waitpid(pid_child1, &status1, 0) == -1){
						printf("wait() error!\n");
					}
					else if(WIFEXITED(status1)) {
						printf("C pid is %d, this status is %d\n", pid_child1, WEXITSTATUS(status1));
					}

					if(waitpid(pid_child2, &status2, 0) == -1){
						printf("wait() error!\n");
					}
					else if(WIFEXITED(status2)) {
						printf("D pid is %d, this status is %d\n", pid_child2, WEXITSTATUS(status2));
					}

					if(waitpid(pid_child3, &status3, 0) == -1){
						printf("wait() error!\n");
					}
					else if(WIFEXITED(status3)) {
						printf("E pid is %d, this status is %d\n", pid_child3, WEXITSTATUS(status3));
					}
					exit(0);
				}
				else if(pid_child3 == 0) {
					printf("This is E. My pid %d\n", getpid());
					//sleep(3);
					exit('E');
				}
			}
			else if(pid_child2 == 0) {
				printf("This is D. My pid %d\n", getpid());
				exit('D');
			}
		}
		else if(pid_child1 == 0) {
			printf("This is C. My pid %d\n", getpid());
			exit('C');
		}
	} 
	else if(pid1 > 0){
		pid2 = fork(); //Создаём второго потомка
		if(pid2 > 0) {//Родитель
			int status;
			int status_2;
			if(waitpid(pid1, &status, 0) == -1){
				printf("wait() error!\n");
			}
			else if(WIFEXITED(status)) {
				printf("Ended 1 child with pid %d, his status is %d\n", pid1, WEXITSTATUS(status));
			}

			if(waitpid(pid2, &status_2, 0) == -1){
				printf("wait() error!\n");
			}
			else if(WIFEXITED(status_2)) {
				printf("Ended 2 child with pid %d, his status is %d\n", pid2, WEXITSTATUS(status_2));
			}
			printf("finish\n");
		}
		else if(pid2 == 0) {
			printf("This is main child 2 my pid is %d\n", getpid());
			pid_child1 = fork();//Создаём 1 подпотомка
			if( pid_child1 > 0 ) {
				pid_child2 = fork();//Создаём 2 потомка 
				if(pid_child2 > 0) {
					if(waitpid(pid_child1, &status1, 0) == -1){
						printf("wait() error!\n");
					}
					else if(WIFEXITED(status1)) {
						printf("F pid is %d, this status is %d\n", pid_child1, WEXITSTATUS(status1));
					}

					if(waitpid(pid_child2, &status2, 0) == -1){
						printf("wait() error!\n");
					}
					else if(WIFEXITED(status1)) {
						printf("G pid is %d, this status is %d\n", pid_child2, WEXITSTATUS(status1));
					}
					exit(0);
				}
				else if(pid_child2 == 0) {
					printf("This is G. My pid %d\n", getpid());
					exit('G');
				}
			}
			else if(pid_child1 == 0) {
				printf("This is F. My pid %d\n", getpid());
				//sleep(2);
				exit('F');
			}
		}
	} 
}
	
int main(int argc, char** argv) {
	doit();
}

#include "stdio.h"
#include "stdlib.h"
#include "sys/types.h"
#include "sys/wait.h"
//функция переворачивающая слово
void reverse(char* beg, char* end){
	char* from = beg;
	char* to = end + 1;
	char sym;
	while(beg < end) {
		sym = *beg;
		*beg = *end;
		*end = sym;

		++beg;
		--end; 
	}
	while(from < to) {
		printf("%c", *from);
		from++;
	}
	printf("\n");
}
int main(int argc, char* argv[]) {
	if(argc < 2) {
		printf("Not enogth arguments!\n");
		return 0;
	}
	char* text = argv[1];
	char* begin;
	char* end;
	begin = text;
	end = text;
	//перебираем весь текст
	while( *end != '\0') {
		//если нашли слово
		if( *end == ' ' ) {
			pid_t pid = fork();
			if(pid == 0){
				printf("Child with pid %d: ", (int)getpid());
				reverse(begin, end-1);
				return 0;
			}
			begin = end + 1;
		}
		++end;
	}
	pid_t pid = fork();
	if(pid == 0){
		printf("Child with pid %d: ", (int)getpid());
		reverse(begin, end-1);
		return 0;
	}
	return 0;
}

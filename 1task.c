#include "stdio.h"
#include "sys/types.h"
void reverse(char* beg, char* end){
	char sym;
	while(beg < end) {
		sym = *beg;
		*beg = *end;
		*end = sym;

		++beg;
		--end; 
	}
}
int main(int argc, char* argv[]) {
	pid_t pid = fork();
	if(argc < 2) {
		printf("Not enogth arguments!");
	}
	char* text = argv[1];
	char* begin;
	char* end;
	begin = text;
	end = text;
	while( *end != '\0') {
		if( *end == ' ' ) {
			if(pid == 0){
				reverse(begin, end-1);
			}
			begin = end + 1;
		}
		++end;
	}
	//pid = fork();
	if(pid == 0){
		reverse(begin, end-1);
		printf("%s\n", text);
	}
	return 0;
}

#include "stdio.h"
#include "stdlib.h"
int main(int argc, char* argv[]){
	if(argc != 2) {
		printf("Error in passing arguments.\n");
		exit(-1);
	}
	printf("%s\n", argv[1]);
	return 0;
}
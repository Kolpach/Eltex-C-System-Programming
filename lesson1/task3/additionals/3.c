#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int stringToInt(char* string) {
	int count = 0;
	for(int i = 0; i < strlen(string); ++i){
		if(string[i] >= '0' && string[i] <= '9'){
			count *= 10;
			count += (string[i] - '0');
		}
		else {
			printf("Wrong parametr. Must be integer\n");
			break;
		}
	}
	return count;
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		printf("Error in passing arguments\n");
		exit(-1);
	}
	int massiveSize = argc - 1;
	int* thingsToSort = (int*) malloc(sizeof(int) * massiveSize);
	//setting massive
	for(int i = 0; i < massiveSize; ++i) {
		thingsToSort[i] = stringToInt(argv[i + 1]);
	}

	for(int sortIteration = 0; sortIteration < massiveSize - 1; ++sortIteration){
		for(int i = 0; i < massiveSize - 1 - sortIteration; ++i) {
			if( thingsToSort[i] > thingsToSort[i + 1] ) {
				int buf;
				buf = thingsToSort[i];
				thingsToSort[i] = thingsToSort[i + 1];
				thingsToSort[i + 1] = buf;
			}
		}
	}
	for(int i = 0; i < massiveSize; ++i) {
		printf("%d ", thingsToSort[i]);
	}
	printf("\n");
}
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/shm.h>
int getNumber(char* text) {
	int count = 0;
	for(int i = 0; i < strlen(text); ++i){
		if(text[i] >= '0' && text[i] <= '9'){
			count *= 10;
			count += (text[i] - '0');
		}
		else {
			printf("Wrong parametr. Must be integer\n");
			break;
		}
	}
	return count;
}

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

void taskForChild1(int** matrix) {
	int max = 0;
	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 5; ++j) {
			if(max < matrix[i][j]) {
				max = matrix[i][j];
			}
		}
	}
	printf("max number is %d\n", max);
}
void taskForChild2(int** matrix) {
	int min = 100;
	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 5; ++j) {
			if(min > matrix[i][j]) {
				min = matrix[i][j];
			}
		}
	}
	printf("min number is %d\n", min);
}
void taskForChild3(int** matrix) {
	int sum = 0;
	for(int i = 0; i < 5; ++i) {
		for(int j = 0; j < 5; ++j) {
			sum += matrix[i][j];
		}
	}
	printf("sum is %d\n", sum);
}
void taskForChild4(int** matrix) {
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

printf("product is %s\n", numInTextForm);
	free(numInTextForm);
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
	if(argc < 3){
		perror("Not enogth args passed\n");
		return 0;
	}
	int key = getNumber(argv[1]);
	int taskNumber = getNumber(argv[2]);
	int id;
	/* Создадим область разделяемой памяти */
	if ((id = shmget(key, 0, 0)) < 0) {
		perror("shmget child");
		exit(1);
	}

	/* Получим доступ к разделяемой памяти */
	int** matrix;
	if ((matrix = shmat(id, NULL, 0)) == (int **) -1) {
		perror("shmat child");
		exit(1);
	}
	create_index(matrix, 5, 5, sizeof(int));

	switch (taskNumber)
	{
	case 1:
		taskForChild1(matrix);
		break;
	case 2:
		taskForChild2(matrix);
		break;
	case 3:
		taskForChild3(matrix);
		break;
	case 4:
		taskForChild4(matrix);
		break;

	default:
		break;
	}
	shmdt(matrix);
	return 0;
}
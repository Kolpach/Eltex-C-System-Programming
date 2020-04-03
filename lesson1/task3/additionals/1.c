#include "stdio.h"
#include "stdlib.h"
#include "string.h"
int main(int argc, char* argv[]){
    if(argc != 3) {
        printf("Error in passing arguments!\n");
        exit(-1);
    }

    int countOfStrings = 0;
    //считываем count из строки
    for(int i = 0; i < strlen(argv[1]); ++i){
        if(argv[1][i] >= '0' && argv[1][i] <= '9'){
            countOfStrings *= 10;
            countOfStrings += (argv[1][i] - '0');
        }
        else {
            printf("Wrong parametr. Must be integer\n");
            break;
        }
    }

    int countOfElements = 0;
    //считываем count из строки
    for(int i = 0; i < strlen(argv[2]); ++i){
        if(argv[2][i] >= '0' && argv[2][i] <= '9'){
            countOfElements *= 10;
            countOfElements += (argv[2][i] - '0');
        }
        else {
            printf("Wrong parametr. Must be integer\n");
            break;
        }
    }

    for(int i = 0; i < countOfStrings; ++i){
        for(int j = 0; j < countOfElements; ++j){
            printf("0 ");
        }
        printf("\n");
    }
    return 0;
}
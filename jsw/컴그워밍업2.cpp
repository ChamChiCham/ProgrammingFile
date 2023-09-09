#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int main() {
    const char* filename = "data.txt";

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("파일을 열 수 없습니다.\n");
        return 0;
    }

    int wordcount = 0;
    int numbercount = 0;
    int capitalcount = 0;
    char c;
    char words[20][100] = { 0 };
    // 새로운 변수 i
    int j = 0;
    while ((c = fgetc(file)) != EOF) {
        // c에 문자를 읽음

        // words[i]에 이 문자를 넣음
        // i증가

        // 만약 '\n'을 만나면
        // i = 0;
        // j 증가
        
        /*
        for (int i = 0; i < 100; i++) {
            words[j][i] = c;
            if (c == '\n') {
                j++;
            }
        }
        */
    }


    fclose(file);

    printf("word count: %d\n", wordcount - numbercount);
    printf("number count: %d\n", numbercount);
    printf("Capital word: %d\n", capitalcount);

    return 0;
}
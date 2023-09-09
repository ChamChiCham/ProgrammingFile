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
    int i = 0;
    int j = 0;
    while ((c = fgetc(file)) != EOF) {

        words[j][i] = c;
        i++;

        if (c == '\n') {
            i = 0;
            j++;
        }
    }
    fclose(file);
    for (j = 0; j < 20; j++) {
        for (int i = 0; i < 100; i++) {
            printf("%c", words[j][i]);
        }
        printf("\n");
    }

    for (j = 0; j < 20; j++) {
        for (int i = 0; i < 100; i++) {
            if (words[j][i] == '\n' || words[j][i] == ' ') {
                wordcount++;
            }
            else if (words[j][i] >= '0' && words[j][i] <= '9') {
                numbercount++;
            }
        }
    }
    // 스페이스 단위로 단어를 뗀다
    // 숫자뒤에 스페이스가 올 때까지 숫자를 1카운트한다

    printf("word count: %d\n", wordcount - numbercount);
    printf("number count: %d\n", numbercount);
    printf("Capital word: %d\n", capitalcount);

    return 0;
}
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
    bool check;
    
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
            else if (words[j][i] >= '0' && words[j][i] <= '9' && check == false) {
                check = true;
            }
            else if (check == true) {
                if (words[j][i] <= '0' && words[j][i] >= '9') {
                    check = false;
                }
                else if (words[j][i] == '\n' || words[j][i] == ' ') {
                    numbercount++;
                }
            }
        }
    }
    // 숫자뒤에 스페이스가 올 때까지 숫자를 1카운트한다

    /*
    * bool 변수 하나 (check)
    * 
    * for문 돌려서 문자 확인
    * if (숫자 만나면 && check가 false면
    *   check 트루로 바꿈
    * if (check가 true면
        if (숫자를 안만나면
            check를 false로
        else if (' '나 \n을 만나면
            카운트
    *   
    * 
    */
   


    printf("word count: %d\n", wordcount - numbercount);
    printf("number count: %d\n", numbercount);
    printf("Capital word: %d\n", capitalcount);

    return 0;
}
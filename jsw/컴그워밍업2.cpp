#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

int main() {
    const char* filename = "data.txt";

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("������ �� �� �����ϴ�.\n");
        return 0;
    }

    int wordcount = 0;
    int numbercount = 0;
    int capitalcount = 0;
    char c;
    char words[100] = { 0 };

    while ((c = fgetc(file)) != EOF) {

        // �̷��� ���� ����
        // �ϴ� �� �־� ���� ����
        // �����̽��� '\n' �ν�
        for (int i = 0; i < 100; i++) {
            words[i] = c;
        }
        /*if ((c >= 'A') && (c <= 'Z')) {
            words[wordcount] = c;
        }
        else if ((c >= 'a') && (c <= 'z')) {
            words[wordcount] = c;
        }
        else if ((c >= '0') && (c <= '9')) {
            words[wordcount] = c;
        }
        else {
            wordcount++;
        }*/
    }
    for (int i = 0; i < 100; i++) {
        if ((char)words[i] == ' ' || words[i] == '\n') {
            wordcount++;
        }
    }

    fclose(file);

    printf("word count: %d\n", wordcount - numbercount);
    printf("number count: %d\n", numbercount);
    printf("Capital word: %d\n", capitalcount);

    return 0;
}
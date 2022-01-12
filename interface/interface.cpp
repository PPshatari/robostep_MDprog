#include "interface.h"
Interface::Interface(PinName tx, PinName rx):Input(tx, rx) {}
///y/nを問う関数.
int Interface::AskPermission(const char* comment, const char yes, const char no)
{
    int isFinish = 0;
    int isok = 0;
    do {
        printf("%s\r\n",comment);
        printf(" yes:");
        printf("%c",yes);
        printf(" no:");
        printf("%c\r\n",no);

        char buffer[2];
        if (!getns(buffer, sizeof(buffer)/sizeof(char))) {
            printf("error\r\n");
            continue;
        }
        char ans = buffer[0];
        printf("%c\r\n",ans);
        if(ans == yes) {
            isok = 1;
            isFinish = 1;
        } else if(ans == no) {
            isok = 0;
            isFinish = 1;
        } else {
            printf("error: you should put %c or %c\r\n", yes, no);
            isFinish = 0;
        }
    } while (!isFinish);
    return isok;
}

double Interface::AskNum(const char* info)
{
    int isFinish = 0;
    double num = 0;
    do {
        printf("%s ",info);
        fflush(stdout);
        char *error;
        char buffer[255];
        if (!getns(buffer, sizeof(buffer)/sizeof(char))) {
            printf("error\r\n");
            continue;
        }
        num = strtod(buffer, &error);
        if (*error != '\0') {
            printf("error:cannot recognize %s\r\n", error);
            isFinish = 0;
        } else {
            isFinish = 1;
        }
    } while(!isFinish);
    return num;
}

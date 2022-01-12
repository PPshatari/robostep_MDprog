#include "input.h"
#include <mbed.h>

const int kDelete = 127;//deleteの文字コード
Input::Input(PinName tx, PinName rx):Serial(tx,rx){}
//返り値 成功:0,失敗:1
int Input::getns(char* str, int bufsize)
{
    int isok = 1;
    int position = 0;
    int isFinish = 0;
    while(!isFinish) {
        if (!readable()) continue;
        char tmp = getc();
        if(tmp == '\n' || tmp == '\r') {
            str[position] = '\0';
            isok=1;
            isFinish = 1;
        } else if(tmp == '\b' || tmp == kDelete) {
            if(position>0)position--;
            str[position] = '\0';
            printf("\b \b");
        } else {
            str[position++] = tmp;
            printf("%c",tmp);
        }
        if(position > bufsize-1){
            printf("\r\n");
            printf("error :Your text has exceeded the allowed number of character %d\r\n", bufsize-1);
            printf("put string again\r\n");
            isok = 0;
            str[0] = '\0';
            position = 0;
        }
    }
    printf("\r\n");
    return isok;
}

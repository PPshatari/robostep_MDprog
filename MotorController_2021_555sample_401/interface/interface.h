#ifndef _INCLUDED_INTERFACE_INTERFACE_H
#define _INCLUDED_INTERFACE_INTERFACE_H
#include "mbed.h"
#include "input.h"
/** @class Interface
@brief interface
*/
class Interface:public Input
{
public:
    Interface(PinName tx, PinName rx);

    /** @param comment yes/noの前にprintfする言葉
    @param yes yesの時に打ち込んでほしい文字
    @param no noの時に打ち込んでほしい文字
    @return 入力がyesなら1, noなら0
    @code
    #include "mbed.h"
    #include "interface.h"
    Interface interface(USBTX,USBRX);
    int main() {
    int isok = interface.AskPermission("Can I do it?", 'y', 'n');
    printf("%d\r\n",isok);
    }
    @endcode
    */
    int AskPermission(const char* comment, const char yes, const char no);

    /** @param comment 表示する文字
    @return 数値(double)
    @code
    #include "mbed.h"
    #include "interface.h"
    Interface interface(USBTX,USBRX);
    int main() {
    double num = interface.AskNum("put number");
    printf("num is %f\r\n",num);
    }
    @endcode
    */
    double AskNum(const char* comment);
};
#endif
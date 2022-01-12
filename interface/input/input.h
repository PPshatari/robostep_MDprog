#ifndef _INCLUDED_INPUT_INPUT_H
#define _INCLUDED_INPUT_INPUT_H
#include "mbed.h"
/** @class Input
*@brief teratermから文字列を読み取る
*/
class Input:public Serial
{
public:
    Input(PinName tx, PinName rx);
    /** 文字列を読み取る関数
*@param str 出力先の配列
*@param bufsize 配列サイズ
*@return  成功:0,失敗:1
*/
    int getns(char* str,int bufsize);
};
#endif
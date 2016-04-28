
//
// Created by wujian on 16/1/12.
//

#include "gk_code_util.h"


char * long2AsciiBytes(long val)
{
    int negative =  val<0;
    long abs = labs(val);
    int size = abs == 0?1 :((int)log10(abs)+(negative?2:1));
    char *arr = calloc(size,sizeof(char));
    if(negative)
        arr[0]='-';
    long next = abs;
    while((next /=10)>0) {
        arr[--size] =(char)('0'+(abs %10));
        abs = next;
    }
    arr[--size]=(char)('0'+abs);
    return arr;
}

long asciiBytes2long(char * val)
{
    if (val == NULL || strlen(val)==0)
        return 0L;
    int negative = val[0] =='-';
    long res = 0;
    for (int i=(negative ?1:0);i<strlen(val);i++) {
        res *=10;
        res +=val[i]-'0';
    }
    if(negative) res *=-1;
    return res;

}


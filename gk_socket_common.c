//
// Created by wujian on 16/1/11.
//


#include "gk_socket_common.h"


void GKUserMessage(const char *msg, const char *detail) {
    fputs(msg, stderr);
    fputs(": ", stderr);
    fputs(detail, stderr);
    fputc('\n', stderr);
    //exit(1);
}

void GKSystemMessage(const char *msg) {
    perror(msg);
    //exit(1);
}

char * combine(char* a, char* b) {
    int n = strlen(a);
    while(*b) {
        a[n++] = *b++;
    }
    return a;
}

char * seekString(char * src,char *seekChar)
{

    char* ptr   = strstr(src, seekChar);
    int   pos   = (int)(ptr-src);
    char* split = (char*)malloc(pos*sizeof(char));
    strncpy(split,src,pos);
    return  split;
}

/*从字符串的左边截取n个字符*/
char * left(char *dst,char *src, int n)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len;
    /*p += (len-n);*/   /*从右边第n个字符开始*/
    while(n--) *(q++) = *(p++);
    *(q++)='\0'; /*有必要吗？很有必要*/
    return dst;
}

/*从字符串的右边截取n个字符*/
char * right(char *dst,char *src, int n)
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len;
    p += (len-n);   /*从右边第n个字符开始*/
    while(*(q++) = *(p++));
    return dst;
}


/*从字符串的中间截取n个字符*/
char * mid(char *dst,char *src, int n,int m) /*n为长度，m为位置*/
{
    char *p = src;
    char *q = dst;
    int len = strlen(src);
    if(n>len) n = len-m;    /*从第m个到最后*/
    if(m<0) m=0;    /*从第一个开始*/
    if(m>len) return NULL;
    p += m;
    while(n--) *(q++) = *(p++);
    *(q++)='\0'; /*有必要吗？很有必要*/
    return dst;
}

int getNumLength(long val)
{
    int negative =  val<0;
    long abs = labs(val);
    int size = abs == 0?1 :((int)log10(abs)+(negative?2:1));
    return  size;
}
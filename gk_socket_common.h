//
// Created by wujian on 16/1/11.
//

#ifndef GKSOCKETCOMMON_H
#define GKSOCKETCOMMON_H

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef TRUE
#undef TRUE
#endif

#ifndef FALSE
#undef  FALSE
#endif



#define CRLF "\r\n"
#define CR '\r'
#define LF '\n'




void GKUserMessage(const char *msg, const char *detail);
void GKSystemMessage(const char *msg);

char * combine(char* a, char* b);
char * seekString(char* src,char* seekChar);
char * left(char *dst,char *src, int n);
char * right(char *dst,char *src, int n);
char * mid(char *dst,char *src, int n,int m);
int getNumLength(long val);
#endif

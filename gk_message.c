//
// Created by wujian on 16/1/11.
//

#include "gk_message.h"



#define MAX_BUFF 5120
#define LENGTH_CRLF 2



char * encode_int_message(struct message m)
{
    int * data    = (int *)m.data;
    char * content = long2AsciiBytes(*data);

    size_t size   = strlen(content)+1+strlen(CRLF);

    char * encode = calloc(size,sizeof(char));
    encode[0]=INTEGER_MARKER;
    strcat(encode,content);
    strcat(encode,CRLF);

    free(content);

    return  encode;
}

char * encode_str_message(struct message m)
{
    char * data     = (char *)m.data;
    char * capacity = long2AsciiBytes(m.capacity);
    size_t size     = 0;
    char *encode;
    if(m.capacity<1){
        size = strlen(capacity)+3;
        encode = calloc(size,sizeof(char));
        encode[0]=STRING_MARKER;
        strcat(encode,capacity);
        strcat(encode,CRLF);
        free(capacity);
        return  encode;
    }else
    {
        size = strlen(capacity)+strlen(data)+1+2*strlen(CRLF);
        encode = calloc(size,sizeof(char));
        encode[0]=STRING_MARKER;
        strcat(encode,capacity);
        strcat(encode,CRLF);
        strcat(encode,data);
        strcat(encode,CRLF);
        free(capacity);
        return encode;
    }
}

char * encode_ary_message(struct message *ms,size_t length)
{
    char * capacity = long2AsciiBytes(length);
    size_t size = 0;
    char *encode;
    if(atoi(capacity)<1) {
        size = strlen(capacity)+3;
        encode = calloc(size,sizeof(char));
        encode[0]=ARRAY_MARKER;
        strcat(encode,capacity);
        strcat(encode,CRLF);
        free(capacity);
        return encode;
    }else
    {

        char * maxEncode = calloc(MAX_BUFF,sizeof(char));
        maxEncode[0] = ARRAY_MARKER;
        strcat(maxEncode,capacity);
        strcat(maxEncode,CRLF);


        for (int i=0;i<length;i++){
            char * tempCode;
            struct message m = ms[i];
            if(m.m_type ==int_msg) {
                tempCode =encode_int_message(m);
            }else if(m.m_type == string_msg) {
                tempCode = encode_str_message(m);
            }

            strcat(maxEncode,tempCode);
            free(tempCode);
        }

        size = strlen(maxEncode);
        char * subEncode = calloc(size,sizeof(char));
        strcat(subEncode,maxEncode);
        free(maxEncode);
        return subEncode;

    }
}

struct message * decode_message(char * msg)
{
    char * marker = &msg[0];
    struct message * m = NULL;
    if(*marker ==INTEGER_MARKER){
        m = decode_int_message(++msg);
    }else if(*marker == STRING_MARKER){
        m = decode_str_message(++msg);
    }else {
        m =decode_ary_message(++msg);
    }

    return m;
}


struct message * decode_str_message(char * msg)
{
    char* lptr = seekString(msg,CRLF);
    long length = asciiBytes2long(lptr);

    char *content = calloc((size_t)length,sizeof(char));
    mid(content,msg,(int)length,LENGTH_CRLF+(int)strlen(lptr));

    struct message * m = malloc(sizeof(struct message));
    m->m_type = string_msg;
    m->data =calloc(sizeof(content),sizeof(char));
    memcpy(m->data,content,sizeof(content)*sizeof(char));

    free(lptr);
    return  m;
}

struct message * decode_int_message(char * msg)
{
    char* ptr  = seekString(msg,CRLF);
    long data  = asciiBytes2long(ptr);

    struct message *m = calloc(1,sizeof(struct message));
    m->m_type =int_msg;
    m->data = malloc(sizeof(long));
    memcpy(m->data,&data,sizeof(uint32_t));
    free(ptr);

    return m;
}

struct message * decode_ary_message(char * msg)
{
    char *lptr = seekString(msg,CRLF);
    long length = asciiBytes2long(lptr);

    struct message * m = calloc(length,sizeof(struct message));


    msg = msg+LENGTH_CRLF+(int)strlen(lptr);
    int i=0;
    for(;i<(int)length;i++)
    {


        if(msg[0] == INTEGER_MARKER)
        {
            m[i] = *decode_int_message(++msg);
            msg = msg+LENGTH_CRLF+getNumLength(*(int*)(m[i].data));

        }else if(msg[0] == STRING_MARKER)
        {
            m[i] = *decode_ary_message(++msg);
            msg = msg+2*LENGTH_CRLF+strlen((char*)m[i].data)+getNumLength(m[1].capacity);

        }
    }


    m[i].data = NULL;

    free(lptr);
    return m;
}













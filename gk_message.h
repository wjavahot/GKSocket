//
// Created by wujian on 16/1/11.
//


#ifndef GKMESSAGE_H
#define GKMESSAGE_H


#include "gk_code_util.h"
#include "gk_socket_common.h"

#define INTEGER_MARKER ':'
#define STRING_MARKER '$'
#define ARRAY_MARKER '*'


enum message_type
{
    int_msg    = 0x01,
    string_msg = 0x02,
    array_msg  = 0x03,
};


struct message
{
    void *data;
    enum message_type m_type;
    uint32_t capacity;
};


char * encode_int_message(struct message m);
char * encode_str_message(struct message m);
char * encode_ary_message(struct message * ms,size_t length);

struct message * decode_int_message(char * msg);
struct message * decode_str_message(char * msg);
struct message * decode_ary_message(char * msg);
struct message * decode_message(char * msg);



#endif //

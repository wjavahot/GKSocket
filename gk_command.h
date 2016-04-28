//
// Created by wujian on 16/1/18.
//



#ifndef GKCOMMOND_H
#define GKCOMMOND_H

#include "gk_message.h"

enum command_type
{
    sub = 0x01,
    re_sub = 0xFE,
    bs =0x02,
    re_bs =0xfd,
};

;

enum net_type{
    net_2g     = 0x01,
    net_3g     = 0x02,
    net_4g     = 0x03,
    net_wifi   = 0x10,
    net_unknow = 0x20,
};


struct command{
    enum command_type c_type;
    enum net_type n_type;
    uint32_t length;
    uint32_t heart_beat;
    uint32_t seq;
    uint32_t total;
    char * token_and_secret;
    char * session_id;
    char * version;
    char * check_sum;
    char * content;
};


struct sub_command
{
    enum command_type c_type;
    enum net_type n_type;
    uint32_t length;
    char * token_and_secret;
    uint32_t heart_beat;
    char * version;
    char * content;
};


struct re_sub_command
{
    enum command_type c_type;
    uint32_t length;
    char * session_id;
    uint32_t heart_beat;
    char * version;
    char * content;
};


struct bs_command
{
    enum command_type c_type;
    enum net_type n_type;
    uint32_t length;
    char * token_and_sercret;
    char * session_id;
    uint32_t heart_beat;
    char * version;
    char * content;
};


struct re_bs_command
{
    enum command_type c_type;
    uint32_t length;
    char * session_id;
    uint32_t heart_beat;
    char * version;
    char * content;
};



struct sub_command * generate_sub_command(struct message * m);
struct re_sub_command * generate_re_sub_command(struct message *m);
struct bs_command * generate_bs_command(struct message *m);
struct re_bs_command * generate_re_bs_command(struct message * m);

struct message * sub_command_to_message(struct sub_command * s_command);
struct message * re_sub_command_to_message(struct re_sub_command * r_s_command);
struct message * bs_command_to_message(struct bs_command * b_command);
struct message * re_bs_command_to_message(struct re_bs_command * r_b_command);


#endif

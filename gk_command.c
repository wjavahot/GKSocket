

//
// Created by wujian on 16/1/11.
//

#include "gk_command.h"


struct message * sub_command_to_message(struct sub_command * s_command)
{
    struct message * m = calloc(7,sizeof(struct message));
    //
    m[0].m_type = int_msg;
    m[0].data = &s_command->c_type;

    m[1].m_type = int_msg;
    m[1].data = &s_command->length;

    m[2].m_type = string_msg;
    m[2].data = s_command->token_and_secret;
    m[2].capacity = sizeof(s_command->token_and_secret);

    m[3].m_type = int_msg;
    m[3].data = &s_command->heart_beat;

    m[4].m_type = string_msg;
    m[4].data = s_command->version;
    m[4].capacity = sizeof(s_command->version);


    m[5].m_type = int_msg;
    m[5].data = &s_command->n_type;

    m[6].m_type = string_msg;
    m[6].data = s_command->content;
    m[6].capacity = sizeof(s_command->content);


    return m;
}

struct sub_command * generate_sub_command(struct message * m)
{
    struct sub_command * s_commad    = calloc(7,sizeof(struct message));
    s_commad->c_type    = *(enum command_type *)m[0].data;
    s_commad->length         = *(uint32_t *)m[1].data;
    s_commad->token_and_secret = (char *)m[2].data;
    s_commad->heart_beat      = *(uint32_t *)m[3].data;
    s_commad->version        = (char *)m[4].data;
    s_commad->n_type        = *(enum net_type *)m[5].data;
    s_commad->content        = (char *)m[6].data;
    return s_commad;
}

struct message * re_sub_command_to_message(struct re_sub_command * r_s_command)
{
    struct message * m = calloc(7,sizeof(struct message));

    m[0].m_type = int_msg;
    m[0].data = &r_s_command->c_type;

    m[1].m_type = int_msg;
    m[1].data = &r_s_command->length;

    m[2].m_type = string_msg;
    m[2].data = r_s_command->session_id;
    m[2].capacity = sizeof(r_s_command->session_id);

    m[3].m_type = int_msg;
    m[3].data = &r_s_command->heart_beat;

    m[4].m_type = string_msg;
    m[4].data = r_s_command->version;
    m[4].capacity = sizeof(r_s_command->version);

    m[5].m_type = string_msg;
    m[5].data = r_s_command->content;
    m[6].capacity = sizeof(r_s_command->content);

    return m;
}


struct re_sub_command * generate_re_sub_command(struct message *m)
{
    struct re_sub_command * r_s_command = calloc(6,sizeof(struct message));
    r_s_command->c_type = *(enum command_type *)m[0].data;
    r_s_command->length = *(uint32_t *)m[1].data;
    r_s_command->session_id = (char *)m[2].data;
    r_s_command->heart_beat = *(uint32_t *)m[3].data;
    r_s_command->version   = (char *)m[4].data;
    r_s_command->content   = (char *)m[5].data;
    return r_s_command;
}

struct message * bs_command_to_message(struct bs_command * b_command)
{
    struct message * m = calloc(8,sizeof(struct message));

    m[0].m_type = int_msg;
    m[0].data = &b_command->c_type;

    m[1].m_type = int_msg;
    m[1].data = &b_command->length;

    m[2].m_type = string_msg;
    m[2].data = b_command->token_and_sercret;
    m[2].capacity = sizeof(b_command->token_and_sercret);

    m[3].m_type = string_msg;
    m[3].data = b_command->session_id;
    m[3].capacity = sizeof(b_command->session_id);

    m[4].m_type = int_msg;
    m[4].data = &b_command->heart_beat;

    m[5].m_type = string_msg;
    m[5].data = b_command->version;
    m[5].capacity = sizeof(b_command->version);

    m[6].m_type = int_msg;
    m[6].data = (enum CommandType *)b_command->n_type;

    m[7].m_type = string_msg;
    m[7].data = b_command->content;
    m[7].capacity = sizeof(b_command->content);

    return m;
}



struct bs_command * generate_bs_command(struct message *m)
{
    struct bs_command * b_command  = calloc(8,sizeof(struct message));
    b_command->c_type = *(enum command_type *)m[0].data;
    b_command->length      = *(uint32_t *)m[1].data;
    b_command->token_and_sercret = (char *)m[2].data;
    b_command->session_id   = (char *)m[3].data;
    b_command->heart_beat   = *(uint32_t *)m[4].data;
    b_command->version     = (char *)m[5].data;
    b_command->n_type     = *(enum net_type *)m[6].data;
    b_command->content     = (char *)m[7].data;
    return  b_command;
}

struct message * re_bs_command_to_message(struct re_bs_command * r_b_command)
{
    struct message * m = calloc(6,sizeof(struct message));

    m[0].m_type = int_msg;
    m[0].data = &r_b_command->c_type;

    m[1].m_type = int_msg;
    m[1].data = &r_b_command->length;

    m[2].m_type = string_msg;
    m[2].data = &r_b_command->session_id;
    m[2].capacity = sizeof(r_b_command->session_id);

    m[3].m_type = int_msg;
    m[3].data = &r_b_command->heart_beat;

    m[4].m_type = string_msg;
    m[4].data = &r_b_command->version;
    m[4].capacity = sizeof(r_b_command->version);

    m[5].m_type = string_msg;
    m[5].data = &r_b_command->content;
    m[5].capacity = sizeof(r_b_command->content);

    return m;
}


struct re_bs_command * generate_re_bs_command(struct message * m)
{
    struct re_bs_command * r_b_command = calloc(6,sizeof(struct message));
    r_b_command->c_type  = *(enum command_type *)m[0].data;
    r_b_command->length       = *(uint32_t *)m[1].data;
    r_b_command->session_id    = (char *)m[2].data;
    r_b_command->heart_beat    = *(uint32_t *)m[3].data;
    r_b_command->version      = (char *)m[4].data;
    r_b_command->content      = (char *)m[5].data;
    return  r_b_command;
}













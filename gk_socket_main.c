//
// Created by gordon on 16/2/9.
//
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "gk_socket_manager.h"
#include "gk_command.h"
#include "gk_delim_framer.h"



#define  MAX_WIRE_SIZE 10240



#define T1        60        /* idle time before heartbeat */
#define T2        10



void send_subscribe(struct socket *s)
{
    char * content = "hello world wujian";
    char * tokenAndSecret = "xx&yy";
    struct sub_command s_command = {sub,net_wifi,sizeof(content),tokenAndSecret,300,"1.0",content};
    struct message * m     = sub_command_to_message(&s_command);
    char * message_char    = encode_ary_message(m,7);

    ssize_t n = write(s->socket_fd,message_char,sizeof(message_char));
    if(n<0)
    {
        switch(errno) {
            case EINTR:
            case EAGAIN:
                break;
            default:
                fprintf(stderr, "socket-server: write error :%s.\n",strerror(errno));
                force_close(s);
        }
    }
}

int send_heat_beat(struct socket *s)
{

    char * tokenAndSecret = "xx&yy";
    struct bs_command b_command = {bs,net_wifi,0,tokenAndSecret,"1",300,"1.0",NULL};
    struct message * m = bs_command_to_message(&b_command);
    char * message_char = encode_ary_message(m,8);

    ssize_t n = write(s->socket_fd,message_char,sizeof(message_char));
    if(n<0)
    {
        switch(errno) {
            case EINTR:
            case EAGAIN:
                break;
            default:
                fprintf(stderr, "socket-server: write error :%s.\n",strerror(errno));
                return -1;
        }
    }

    return 1;

}





static void * socket_loop(void *uc)
{
    struct socket_context * sc = uc;
    fd_set fdSocket;
    FD_ZERO(&fdSocket);
    FD_SET(sc->slot->socket_fd, &fdSocket);

    struct timeval tv;
    tv.tv_sec = T1;
    tv.tv_usec = 0;
    int missed_heartbeats = 0;

    int length;
    while (1)
    {


        if(sc->slot->status !=SOCKET_TYPE_CONNECTED)
        {
            goto close;
        }

        fd_set fdRead = fdSocket;
        int nRet = select(sc->slot->socket_fd + 1, &fdRead, NULL, NULL, &tv);
        if (nRet == 0)        /* timed out */
        {
            if (++missed_heartbeats > 3)
            {
                goto close;
                //printf("连续三个心跳未收到，可能服务器已挂\n");
            }
            //printf("发送心跳 #%d\n", missed_heartbeats);

            int r = send_heat_beat(sc->slot);
            if(r<0){
                goto close;
            }
            tv.tv_sec = T2;
            continue;
        }
        else
        {

            int n;
            uint8_t buffer[MAX_WIRE_SIZE];
            n = get_next_message(sc->slot->socket_fd,buffer,MAX_WIRE_SIZE);

            if (n < 0) {
                free(buffer);
                switch(errno) {
                    case EINTR:
                    case EAGAIN:
                        fprintf(stderr, "socket-server: EAGAIN capture.\n");
                        break;
                    default:
                        // close when error
                        goto close;
                }
            }

            if (n==0) {
                free(buffer);
                goto close;
            }

            if (n > 0)
            {

                struct message * m = decode_message((char *)buffer);
                enum command_type  type =  *(enum command_type *)m[0].data;

                switch (type)
                {
                    case re_sub:
                        break;
                    case re_bs:
                        break;
                    default:
                        break;
                }


                missed_heartbeats = 0;
                tv.tv_sec = T1 + T2;
                free(buffer);
            }

        }
    }

    close:
        force_close(sc->slot);
        FD_CLR(sc->slot->socket_fd, &fdSocket);
        pthread_exit(sc->slot);
}


static void * cmd_loop(void *uc)
{
    struct socket_context * sc = uc;

    while(1){


        if(sc->slot->status !=SOCKET_TYPE_CONNECTED)
        {
            goto close;
        }

        if(sc->checkctrl)
        {
            if(has_cmd(sc)){
                int type = ctrl_cmd(sc);
                if(type != -1) {
                    goto close;
                } else{
                    continue;
                }
            }else {
                sc->checkctrl = 0;
            }
        }

        if (sc->event_index == sc->event_n) {
            sc->event_n = sp_wait(sc->event_fd, sc->ev, MAX_EVENT);
            sc->checkctrl = 1;
            sc->event_index = 0;
            if (sc->event_n <= 0) {
                sc->event_n = 0;
                continue;
            }
        }

        struct event *e = &sc->ev[sc->event_index++];
        struct socket *s = e->s;
        if (s == NULL) {
            // dispatch pipe message at beginning
            continue;
        }

        if (e->write) {
            int type = send_buffer(sc, s);
            if (type == -1)
                continue;
            clear_closed_event(sc,type);
            goto close;
        }

    }
    close:
        force_close(sc->slot);
        pthread_exit(sc);

}

void create_thread(struct socket_context * sc)
{

    struct socket *s = sc->slot;

    while(1){
        if(s->status!=SOCKET_TYPE_CONNECTED){
            socket_connect(8090,"127.0.0.1",sc);
            send_subscribe(s);

            pthread_t socket_pid;
            pthread_create(&socket_pid,NULL,socket_loop,sc);

            pthread_t cmd_pid;
            pthread_create(&socket_pid,NULL,cmd_loop,sc);

            pthread_join(socket_pid,NULL);
            pthread_join(cmd_pid,NULL);
        }
        sleep(15);
    }

}


int main()
{


    struct socket *s = calloc(1,sizeof(struct socket));
    s->status = SOCKET_TYPE_INVALID;

    struct socket_context * sc = socket_setup();
    sc->slot = s;

    create_thread(sc);

    while(1)
    {

    }


    return  0;
}
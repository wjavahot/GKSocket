//
// Created by gordon on 16/2/9.
//

#ifndef gk_socket_manager_h
#define gk_socket_manager_h
#define MAX_EVENT 64

#include "gk_socket_poll.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/errno.h>

#define SOCKET_TYPE_INVALID    0
#define SOCKET_TYPE_CONNECTED  1
#define SOCKET_TYPE_HALFCLOSE  3
#define SOCKET_TYPE_CLOSE      4


#define DEFAULT_NETWORK_TIMEOUT 10

struct write_buffer {
    struct write_buffer * next;
    void * buffer;
    char *ptr;
    int sz;
};


struct wb_list {
    struct write_buffer * head;
    struct write_buffer * tail;
};


struct socket
{
    long id;
    int socket_fd;
    int status;
    int64_t wb_size;
    struct wb_list list;
};

struct socket_context
{
    int recv_fd;
    int send_fd;
    poll_fd event_fd;
    int event_n;
    int event_index;
    int checkctrl;
    fd_set rfds;
    struct event ev[MAX_EVENT];
    struct socket *slot;
};



struct request_package{
    char header[2];
    int id;
    int sz;
    char * buffer;
};

struct socket_context *
socket_setup();

void
socket_connect(int port ,char * server,struct socket_context *sc);

void
force_close(struct socket *s);

int
has_cmd(struct socket_context *sc);

int
ctrl_cmd(struct socket_context *sc);

int
send_buffer(struct socket_context *sc, struct socket *s);

void
clear_closed_event(struct socket_context *sc, int type);

#endif

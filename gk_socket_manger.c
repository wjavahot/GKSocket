//
// Created by gordon on 16/2/11.
//



#include <assert.h>
#include "gk_socket_manager.h"


struct timeval prostream_network_delay = {DEFAULT_NETWORK_TIMEOUT, 0};

static inline void
write_buffer_free(struct write_buffer *wb) {
    free(wb->buffer);
}


static void
free_wb_list(struct wb_list *list) {
    struct write_buffer *wb = list->head;
    while (wb) {
        struct write_buffer *tmp = wb;
        wb = wb->next;
        write_buffer_free(tmp);
    }
    list->head = NULL;
    list->tail = NULL;
}


void
force_close(struct socket *s) {

    if (s->status == SOCKET_TYPE_INVALID) {
        return;
    }
    free_wb_list(&s->list);
    s->status = SOCKET_TYPE_INVALID;
}


void
clear_closed_event(struct socket_context *sc, int type) {
    if (type == SOCKET_TYPE_CLOSE){
        int i;
        for (i=sc->event_index; i<sc->event_n; i++) {
            struct event *e = &sc->ev[i];
            struct socket *s = e->s;
            if (s) {
                if (s->status == SOCKET_TYPE_INVALID) {
                    e->s = NULL;
                }
            }
        }
    }
}




/***************************************************************/
static int
send_list_tcp(struct socket *s, struct wb_list *list) {
    while (list->head) {
        struct write_buffer * tmp = list->head;
        for (;;) {
            ssize_t sz = write(s->socket_fd, tmp->buffer, tmp->sz);
            if (sz < 0) {
                switch(errno) {
                    case EINTR:
                        continue;
                    case EAGAIN:
                        return -1;
                }
                force_close(s);
                return SOCKET_TYPE_CLOSE;
            }
            s->wb_size -= sz;
            if (sz != tmp->sz) {
                tmp->buffer += sz;
                tmp->sz -= sz;
                continue;
            }
            break;
        }
        list->head = tmp->next;
        write_buffer_free(tmp);
    }
    list->tail = NULL;

    return -1;
}



int
send_buffer(struct socket_context *sc, struct socket *s) {

    if (send_list_tcp(s,&s->list) == SOCKET_TYPE_CLOSE) {
        return SOCKET_TYPE_CLOSE;
    }
    if (s->list.head == NULL) {
        sp_write(sc->event_fd, s->socket_fd, s, false);
    }
    return -1;
}

static inline int
send_buffer_empty(struct socket *s) {
    return (s->list.head == NULL);
}




static struct write_buffer *
append_sendbuffer_(struct socket_context *sc, struct wb_list *s, struct request_package * request,int n) {
    struct write_buffer * buf = calloc(1, sizeof(struct write_buffer));
    buf->ptr = request->buffer+n;
    buf->sz = request->sz - n;
    buf->buffer = request->buffer;
    buf->next = NULL;
    if (s->head == NULL) {
        s->head = s->tail = buf;
    } else {
        assert(s->tail != NULL);
        assert(s->tail->next == NULL);
        s->tail->next = buf;
        s->tail = buf;
    }
    return buf;
}



static inline void
append_sendbuffer(struct socket_context *sc, struct socket *s, struct request_package * request, int n) {
    struct write_buffer *buf = append_sendbuffer_(sc, &s->list, request, n);
    s->wb_size += buf->sz;
}


static int
send_socket(struct socket_context *sc, struct request_package * request) {
    int id = request->id;
    struct socket * s = sc->slot;

    if (send_buffer_empty(s) && s->status == SOCKET_TYPE_CONNECTED) {
        ssize_t n = write(s->socket_fd,request->buffer,(size_t)request->sz);
        if (n<0) {
            switch(errno) {
                case EINTR:
                case EAGAIN:
                    n = 0;
                    break;
                default:
                    fprintf(stderr, "socket-server: write to %d (fd=%d) error :%s.\n",id,s->socket_fd,strerror(errno));
                    force_close(s);
                    return -1;
            }
        }
        if (n == request->sz) {
            free(request);
            return -1;
        }
        append_sendbuffer(sc, s, request, n);
        sp_write(sc->event_fd, s->socket_fd, s, true);
    } else {
        append_sendbuffer(sc, s, request,0);
    }
    return -1;
}



/***************************************************************/
static void
send_request(struct socket_context * sc,struct request_package *request,char type)
{
    request->header[0] = (uint8_t)type;
    request->header[1] = (uint8_t)request->sz;

    for (;;)
    {
        ssize_t n = write(sc->send_fd,&request->header[0],(size_t)request->sz+2);
        if (n<0) {
            if (errno != EINTR) {
                fprintf(stderr, "socket-server : send ctrl command error %s.\n", strerror(errno));
            }
            continue;
        }
        return;
    }
}


static void
block_readpipe(int pipefd, void *buffer, int sz) {
    for (;;) {
        int n = read(pipefd, buffer, sz);
        if (n<0) {
            if (errno == EINTR)
                continue;
            fprintf(stderr, "socket-server : read pipe error %s.\n",strerror(errno));
            return;
        }
        // must atomic read from a pipe
        return;
    }
}


int
has_cmd(struct socket_context *sc) {
    struct timeval tv = {0,0};
    int retval;

    FD_SET(sc->recv_fd, &sc->rfds);

    retval = select(sc->recv_fd+1, &sc->rfds, NULL, NULL, &tv);
    if (retval == 1) {
        return 1;
    }
    return 0;
}

int
ctrl_cmd(struct socket_context *sc) {
    int fd = sc->recv_fd;
    // the length of message is one byte, so 256+8 buffer size is enough.
    uint8_t buffer[256];
    uint8_t header[2];
    block_readpipe(fd, header, sizeof(header));
    int type = header[0];
    int len = header[1];
    block_readpipe(fd, buffer, len);
    // ctrl command only exist in local fd, so don't worry about endian.
    switch (type) {
        case 'D':
            return send_socket(sc, (struct request_package *)buffer);
        default:
            fprintf(stderr, "socket-server: Unknown ctrl %c.\n",type);
            return -1;
    };

    return -1;
}



/***************************************************************/

struct socket_context *
        socket_setup()
{
    int i;
    int fd[2];
    poll_fd efd = sp_create();
    if (sp_invalid(efd)) {
        fprintf(stderr, "socket-server: create event pool failed.\n");
        return NULL;
    }
    if (pipe(fd)) {
        sp_release(efd);
        fprintf(stderr, "socket-server: create socket pair failed.\n");
        return NULL;
    }

//    if (sp_add(efd, fd[0], NULL)) {
//        // add recvctrl_fd to event poll
//        fprintf(stderr, "socket-server: can't add server fd to event pool.\n");
//        close(fd[0]);
//        close(fd[1]);
//        sp_release(efd);
//        return NULL;
//    }


    struct socket_context * sc = calloc(1,sizeof(struct socket_context));
    sc->event_fd = efd;
    sc->recv_fd = fd[0];
    sc->send_fd = fd[1];
    sc->event_n = 0;
    sc->event_index = 0;
    sc->checkctrl = 1;

    FD_ZERO(&sc->rfds);

    return sc;
}

/***************************************************************/
static int wait_connect(int s, int r)
{
    fd_set rset;
    struct timeval timeout;
    if(r ==0)
    {
        return 0;
    }

    else if(r ==-1)
    {
        if(errno == EINPROGRESS)
        {

        }else {
            return  -1;
        }
    }

    FD_ZERO(&rset);
    FD_SET(s,&rset);

    timeout = prostream_network_delay;
    r = select(s+1,&rset,NULL,NULL,&timeout);

    if(r<0){
        fprintf(stderr, "socketConnect: ERROR_CONNECTION_ERROR.\n");
        return -1;
    }
    if(r==0){
        fprintf(stderr, "socketConnect: ERROR_CONNECTION_TIMEOUT.\n");
        return -1;
    }

    if(!FD_ISSET(s,&rset))
    {
        return -1;
    }
    return 0;
}

static int verify_sock_errors(int s)
{
    uint len;
    int val;
    len = sizeof(val);
    if (getsockopt(s, SOL_SOCKET, SO_ERROR, &val, &len) < 0) {
        return -1;
    } else if (val != 0) {
        return -1;
    }
    return 0;
}

void
socket_connect(int port ,char * server,struct socket_context *sc)
{
    struct addrinfo hints, *res, *ai;
    struct addrinfo la_hints;
    char   port_str[6];

    int sock;
    int r;


    clock_t t = clock();
    long timestamp = t / 1000;

    sc->slot->id     = timestamp;


    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;



    /* convert port from integer to string. */
    snprintf(port_str, sizeof(port_str), "%d", port);

    res = NULL;
    if (getaddrinfo(server, port_str, &hints, &res) != 0)
        return;

    sock = -1;
    for (ai = res; ai != NULL; ai = ai->ai_next) {
        sock = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);

        if (sock == -1)
            continue;
#ifdef SO_NOSIGPIPE
        int kOne = 1;
        int err = setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &kOne, sizeof(kOne));
        if (err != 0)
            continue;
#endif

        int nRecvBuf = 100*1024;
        setsockopt(sock,SOL_SOCKET,SO_RCVBUF,(const char*)&nRecvBuf,sizeof(int));
        int nSendBuf = 100*1024;
        setsockopt(sock,SOL_SOCKET,SO_SNDBUF,(const char*)&nSendBuf,sizeof(int));
//       int nZero =0;
//        setsockopt(s,SOL_SOCKET,SO_SNDBUF,(char *)&nZero,sizeof(nZero));
//        setsockopt(s,SOL_SOCKET,SO_RCVBUF,(char *)&nZero,sizeof(nZero));

        sp_nonblocking(sock);

        r = connect(sock, ai->ai_addr, ai->ai_addrlen);
        r = wait_connect(sock, r);

        if (r != -1) {
            r = verify_sock_errors(sock);
        }

        if (r == -1) {
            if (ai->ai_next) {
                close(sock);
                continue;
            } else {
                goto close_socket;
            }
        }
        /* if we're here, we're good */
        break;
    }

    if (res != NULL)
        freeaddrinfo(res);

    if (ai == NULL)
        return;


    sc->slot->socket_fd = sock;
    sc->slot->status    = SOCKET_TYPE_CONNECTED;
    sp_write(sc->event_fd,sc->slot->socket_fd,sc->slot,true);

    close_socket:
    if (res != NULL)
        freeaddrinfo(res);
    close(sock);
}
/***************************************************************/
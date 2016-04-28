//
// Created by wujian on 16/1/21.
//

#ifndef GKDELIMFRAMER_H
#define GKDELIMFRAMER_H

#include <unistd.h>
#include "gk_socket_common.h"

int get_next_message(int fd,uint8_t *buf, size_t bufSize);

int PutMsg(uint8_t buf[], size_t msgSize, FILE *out);

#endif //GKSOCKETCLIENT_GKDELIMFRAMER_H


//
// Created by wujian on 16/1/21.
//


#include "gk_delim_framer.h"

static const char DELIMITER = '*';

int
get_next_message(int fd,uint8_t *buf, size_t bufSize)
{
    int count = 0;
    char nextChar = 0;

    while(count<bufSize){
        read(fd, &nextChar, 1);
        if(nextChar == EOF){
            if(count>0)
                GKUserMessage("GetNextMsg()","Stream ended prematurely");
            else
                return -1;
        }
        if((nextChar == DELIMITER)&&(buf[count]==LF)&&(buf[count-1]==CR)){
            break;
        }
        buf[count++]= (uint8_t)nextChar;
    }

    if(nextChar != DELIMITER)
    {
        return -count;
    }
    else {
        return  count;
    }

}




int PutMsg(uint8_t buf[], size_t msgSize, FILE *out) {
    // Check for delimiter in message
    int i;
    for (i = 0; i < msgSize; i++)
        if ((buf[i] == LF)&&(buf[i-1]==CR))
            return -1;
    if (fwrite(buf, 1, msgSize, out) != msgSize)
        return -1;
    fputc(DELIMITER, out);
    fflush(out);
    return msgSize;
}
#ifndef __UVIOT_MSG_H
#define __UVIOT_MSG_H

#include <jansson.h>

typedef struct uviot_msg
{
    char *dst;
    char *src;
    u32 id;
    json_t *req;
    json_t *rsp;
}UVIOT_MSG;

#endif

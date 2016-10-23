#ifndef __UVIOT_MSG_H
#define __UVIOT_MSG_H

#include <jansson.h>

typedef struct uviot_msg
{
    char *remote;
    char *dst;
    char *local;
    char *src;
    u32 id;
    json_t *req;
    json_t *rsp;
    int (*callback)(struct uviot_msg *);
}UVIOT_MSG;

int uviot_process_msg(UVIOT_MSG *msg);

#endif

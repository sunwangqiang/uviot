#ifndef __UVIOT_MSG_H
#define __UVIOT_MSG_H

#include <jansson.h>

typedef struct uviot_msg
{
    /* connect */
    char *remote;
    char *local;
    
    /* module */
    char *dst;
    char *src;
    
    /* business event */
    u32 id; 
    json_t *req;
    json_t *rsp;

    /* context */
    int (*callback)(struct uviot_msg *);
    void *priv;
}UVIOT_MSG;

int uviot_send_msg(UVIOT_MSG *msg);

#endif

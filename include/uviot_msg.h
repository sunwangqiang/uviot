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
    
    /* event */
    u32 id; 

    /* business */
    json_t *req;
    json_t *rsp;

    /* context */
    int (*callback)(struct uviot_msg *);
    void *priv;
}UVIOT_MSG;

int uviot_xmit_msg(UVIOT_MSG *msg);

#endif

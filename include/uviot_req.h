#ifndef __UVIOT_MSG_H
#define __UVIOT_MSG_H

#include <jansson.h>

typedef struct uviot_req
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
    int (*callback)(struct uviot_req *);
    void *priv;
}UVIOT_REQ;

int uviot_send_req(UVIOT_REQ *req);

#endif

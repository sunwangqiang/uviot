#ifndef __UVIOT_MSG_H
#define __UVIOT_MSG_H

#include <jansson.h>

typedef struct uviot_req
{
    struct list_head list;
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
UVIOT_REQ *uviot_alloc_req(void);
void uviot_free_req(UVIOT_REQ *);

#endif

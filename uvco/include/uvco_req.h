#ifndef __UVCO_MSG_H
#define __UVCO_MSG_H

#include <jansson.h>

typedef struct uvco_req
{
    struct list_head list;
    /* connect */
    char *remote;
    char *local;
    
    /* module */
    char *dst;
    char *src;
    
    /* business event */
    char *method; 
    json_t *req;
    json_t *rsp;

    /* context */
    int (*callback)(struct uvco_req *);
    void *priv;
}UVCO_REQ;

int uvco_send_req(UVCO_REQ *req);
UVCO_REQ *uvco_alloc_req(void);
void uvco_free_req(UVCO_REQ *);

#endif

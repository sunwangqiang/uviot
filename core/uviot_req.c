
#include <uviot.h>

#define UVIOT_REQ_POOL_SIZE 128

struct uviot_req_list_t
{
    struct list_head head;
    u32 avail;
};

static struct uviot_req_list_t uviot_req_list;

int uviot_req_broadcast_cb(UVIOT_MODULE *mod, void *arg)
{
    UVIOT_REQ *req = (UVIOT_REQ *)arg;
    
    uviot_module_recv_req(mod, req);
    
    return UVIOT_LIST_MODULE_CONTINUE;
}

int uviot_local_recv_req(UVIOT_REQ *req)
{
    UVIOT_MODULE *mod;
    
    mod = uviot_lookup_module(req->dst);
    if(mod){
        uviot_module_recv_req(mod, req);
        return 1;
    }

    if(!strcmp(req->dst, UVIOT_BROADCAST_DST)){
        uviot_list_each_module(uviot_req_broadcast_cb, req);
        return 1;
    }
    return 0;
}

int uviot_process_remote_req(UVIOT_REQ *req)
{
    return 0;
}

int uviot_send_req(UVIOT_REQ *req)
{
    int done;
    
    if(!req->dst){
        return -1;
    }
    /*
     * lookup local modules
     */
    done = uviot_local_recv_req(req);
    if(done){
       return 0;
    }
    
    /*
     * lookup remote modules
     */
    uviot_conn_send(req);
    
    return 0;
}

UVIOT_REQ *uviot_alloc_req(void)
{
    UVIOT_REQ *req;
        
    if(!list_empty(&uviot_req_list.head)){
        req = list_entry(uviot_req_list.head.next, UVIOT_REQ, list);
        
        list_del(&req->list);
        memset(req, 0, sizeof(UVIOT_REQ));
        
        return req;
    }
    return NULL;
}

void uviot_free_req(UVIOT_REQ *req)
{
    if(req){
        list_add(&req->list, &uviot_req_list.head);
    }
}

static int uviot_req_init(void)
{
    int i;
    UVIOT_REQ *req;
    
    
    req = malloc(UVIOT_REQ_POOL_SIZE*sizeof(UVIOT_REQ));
    if(!req){
        return -1;
    }
    memset(req, 0, UVIOT_REQ_POOL_SIZE*sizeof(UVIOT_REQ));

    INIT_LIST_HEAD(&uviot_req_list.head);    
    for(i = 0; i<UVIOT_REQ_POOL_SIZE; i++){
        
        list_add(&req->list, &uviot_req_list.head);
        uviot_req_list.avail++;

        req++;
    }
    return 0;
}

CORE_INIT(uviot_req_init);


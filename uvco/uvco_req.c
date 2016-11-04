
#include <uvco.h>

#define UVCO_REQ_POOL_SIZE 128

struct uvco_req_list_t
{
    struct list_head head;
    u32 avail;
};

static struct uvco_req_list_t uvco_req_list;

int uvco_req_broadcast_cb(UVCO_MODULE *mod, void *arg)
{
    UVCO_REQ *req = (UVCO_REQ *)arg;
    
    uvco_module_recv_req(mod, req);
    
    return UVCO_LIST_MODULE_CONTINUE;
}

int uvco_local_recv_req(UVCO_REQ *req)
{
    UVCO_MODULE *mod;
    
    mod = uvco_lookup_module(req->dst);
    if(mod){
        uvco_module_recv_req(mod, req);
        return 1;
    }

    if(!strcmp(req->dst, UVCO_BROADCAST_DST)){
        uvco_list_each_module(uvco_req_broadcast_cb, req);
        return 1;
    }
    return 0;
}

int uvco_process_remote_req(UVCO_REQ *req)
{
    return 0;
}

int uvco_send_req(UVCO_REQ *req)
{
    int done;
    
    if(!req->dst){
        return -1;
    }
    /*
     * lookup local modules
     */
    done = uvco_local_recv_req(req);
    if(done){
       return 0;
    }
    
    /*
     * lookup remote modules
     */
    uvco_conn_send(req);
    
    return 0;
}

UVCO_REQ *uvco_alloc_req(void)
{
    UVCO_REQ *req;
        
    if(!list_empty(&uvco_req_list.head)){
        req = list_entry(uvco_req_list.head.next, UVCO_REQ, list);
        
        list_del(&req->list);
        memset(req, 0, sizeof(UVCO_REQ));
        
        return req;
    }
    return NULL;
}

void uvco_free_req(UVCO_REQ *req)
{
    if(req){
        list_add(&req->list, &uvco_req_list.head);
    }
}

static int uvco_req_init(void)
{
    int i;
    UVCO_REQ *req;
    
    
    req = malloc(UVCO_REQ_POOL_SIZE*sizeof(UVCO_REQ));
    if(!req){
        return -1;
    }
    memset(req, 0, UVCO_REQ_POOL_SIZE*sizeof(UVCO_REQ));

    INIT_LIST_HEAD(&uvco_req_list.head);    
    for(i = 0; i<UVCO_REQ_POOL_SIZE; i++){
        
        list_add(&req->list, &uvco_req_list.head);
        uvco_req_list.avail++;

        req++;
    }
    return 0;
}

CORE_INIT(uvco_req_init);


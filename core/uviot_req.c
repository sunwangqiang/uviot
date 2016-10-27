
#include <uviot.h>

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


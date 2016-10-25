
#include <uviot.h>

int uviot_msg_broadcast_cb(UVIOT_MODULE *mod, void *arg)
{
    UVIOT_MSG *msg = (UVIOT_MSG *)arg;
    
    uviot_module_recv_msg(mod, msg);
    
    return UVIOT_LIST_MODULE_CONTINUE;
}

int uviot_msg_recv_local_msg(UVIOT_MSG *msg)
{
    UVIOT_MODULE *mod;
    
    mod = uviot_lookup_module(msg->dst);
    if(mod){
        uviot_module_recv_msg(mod, msg);
        return 1;
    }

    if(!strcmp(msg->dst, UVIOT_BROADCAST_DST)){
        uviot_list_each_module(uviot_msg_broadcast_cb, msg);
        return 1;
    }
    return 0;
}

int uviot_process_remote_msg(UVIOT_MSG *msg)
{
    return 0;
}

int uviot_send_msg(UVIOT_MSG *msg)
{
    int done;
    
    if(!msg->dst){
        return -1;
    }
    /*
     * lookup local modules
     */
    done = uviot_msg_recv_local_msg(msg);
    if(done){
       return 0;
    }
    
    /*
     * lookup remote modules
     */
    uviot_conn_send(msg);
    
    return 0;
}


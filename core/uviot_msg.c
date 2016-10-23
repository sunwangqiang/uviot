
#include <uviot.h>

int uviot_module_broadcast_msg_cb(UVIOT_MODULE *mod, void *arg)
{
    UVIOT_MSG *msg = (UVIOT_MSG *)arg;
    
    uviot_module_process_msg(mod, msg);
    
    return UVIOT_LIST_MODULE_CONTINUE;
}

void uviot_process_local_msg(UVIOT_MSG *msg)
{
    UVIOT_MODULE *mod;
    
    mod = uviot_lookup_module(msg->dst);
    if(mod){
        uviot_module_process_msg(mod, msg);
        return;
    }

    if(!strcmp(msg->dst, UVIOT_BROADCAST_DST)){
        uviot_list_each_module(uviot_module_broadcast_msg_cb, msg);
        return;
    }
    
}

int uviot_process_remote_msg(UVIOT_MSG *msg)
{
    return 0;
}

int uviot_process_msg(UVIOT_MSG *msg)
{
    if(!msg->dst){
        return -1;
    }
    if(!msg->remote){
        uviot_process_local_msg(msg);
    }
    
    /*
     * msg to remote process or node
     */
    if(msg->remote && (!msg->local)){
        return -1;
    }
    uviot_process_remote_msg(msg);
    
    return 0;
}


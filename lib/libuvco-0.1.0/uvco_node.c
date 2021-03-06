#include <uvco.h>

static json_t *uvco_node_table;

/*
 * uvco_node_xxx are shortcut functions
 * the original path is:
 * .prepare req args, set req->node_name, req->obj_name
 * .send req to uvco node modules
 * .uvco node modules process create/read/write/list... events
 * .uvco node modules ack the request
 */
int uvco_node_create(char *node_name, char *obj_name, json_t *req, json_t *rsp)
{
    UVCO_NODE *node;
    
    /*
     * local node?
     */
    node = (UVCO_NODE *)(json_integer_value(json_object_get(uvco_node_table, node_name)));
    if(node){
        if(node->create){
            return node->create(node, obj_name, req, rsp);
        }else{
            return -1;
        }
    }

    /*
     * TODO: remote node, send node rpc to broker
     */
    return -1;
}

int uvco_node_read(char *node_name, char *obj_name, json_t *req, json_t *rsp)
{
    UVCO_NODE *node;
    
    /*
     * local node?
     */
    node = (UVCO_NODE *)(json_integer_value(json_object_get(uvco_node_table, node_name)));
    if(node){
        if(node->read){
            return node->read(node, obj_name, req, rsp);
        }else{
            return -1;
        }
    }

    /*
     * TODO: remote node, send node rpc to broker
     */
    return -1;
}

int uvco_node_write(char *node_name, char *obj_name, json_t *req, json_t *rsp)
{
    UVCO_NODE *node;
    
    /*
     * local node?
     */
    node = (UVCO_NODE *)(json_integer_value(json_object_get(uvco_node_table, node_name)));
    if(node){
        if(node->write){
            return node->write(node, obj_name, req, rsp);
        }else{
            return -1;
        }
    }

    /*
     * TODO: remote node, send node rpc to broker
     */
    return -1;
}

int uvco_node_list(char *node_name, char *obj_name, json_t *req, json_t *rsp)
{
    UVCO_NODE *node;
    
    /*
     * local node?
     */
    node = (UVCO_NODE *)(json_integer_value(json_object_get(uvco_node_table, node_name)));
    if(node){
        if(node->list){
            return node->list(node, obj_name, req, rsp);
        }else{
            return -1;
        }
    }

    /*
     * TODO: remote node, send node rpc to broker
     */
    return -1;
}

int uvco_node_remove(char *node_name, char *obj_name, json_t *req, json_t *rsp)
{
    UVCO_NODE *node;
    
    /*
     * local node?
     */
    node = (UVCO_NODE *)(json_integer_value(json_object_get(uvco_node_table, node_name)));
    if(node){
        if(node->remove){
            return node->remove(node, obj_name, req, rsp);
        }else{
            return -1;
        }
    }

    /*
     * TODO: remote node, send node rpc to broker
     */
    return -1;
}

static s32 uvco_node_start(struct uvco_event *ev, json_t *req, json_t *rsp)
{
    /*
     * TODO: publish and discover node tree
     */
    
    return 0;
}


static UVCO_MODULE uvco_node_module ={
	.name = "uvco.node",
};

static UVCO_EVENT uvco_node_event[] = 
{
    {.method = "start", .handler = uvco_node_start},
};

int uvco_node_module_init(void)
{
    uvco_register_module(&uvco_node_module, uvco_node_event, 
                        ARRAY_SIZE(uvco_node_event));
    return 0;
}

int uvco_node_core_init(void)
{
    uvco_node_table = json_object();
    return 0;
}

int uvco_register_node(UVCO_NODE *node, u32 size)
{
	
    if(!node || !node->name){
        return -EINVAL;
    }
    return json_object_set(uvco_node_table, node->name, json_integer((json_int_t)node));
}

int uvco_unregister_node(char *name)
{
    return json_object_del(uvco_node_table, name);
}


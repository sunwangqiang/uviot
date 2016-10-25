#include <uviot.h>

static json_t *uviot_node_table;

int uviot_node_create(char *node_name, char *obj_name, int flags)
{
    UVIOT_NODE *node;
    
    /*
     * local node?
     */
    node = (UVIOT_NODE *)(json_integer_value(json_object_get(uviot_node_table, node_name)));
    if(node){
        if(node->create){
            return node->create(node, obj_name, flags);
        }else{
            return -1;
        }
    }

    /*
     * TODO: remote node, send node rpc to broker
     */
    return -1;
}

int uviot_node_read(char *node_name, char *obj_name, json_t *obj)
{
    UVIOT_NODE *node;
    
    /*
     * local node?
     */
    node = (UVIOT_NODE *)(json_integer_value(json_object_get(uviot_node_table, node_name)));
    if(node){
        if(node->read){
            return node->read(node, obj_name, obj);
        }else{
            return -1;
        }
    }

    /*
     * TODO: remote node, send node rpc to broker
     */
    return -1;
}

int uviot_node_write(char *node_name, char *obj_name, json_t *obj)
{
    UVIOT_NODE *node;
    
    /*
     * local node?
     */
    node = (UVIOT_NODE *)(json_integer_value(json_object_get(uviot_node_table, node_name)));
    if(node){
        if(node->write){
            return node->write(node, obj_name, obj);
        }else{
            return -1;
        }
    }

    /*
     * TODO: remote node, send node rpc to broker
     */
    return -1;
}

int uviot_node_list(char *node_name, json_t *obj)
{
    UVIOT_NODE *node;
    
    /*
     * local node?
     */
    node = (UVIOT_NODE *)(json_integer_value(json_object_get(uviot_node_table, node_name)));
    if(node){
        if(node->list){
            return node->list(node, obj);
        }else{
            return -1;
        }
    }

    /*
     * TODO: remote node, send node rpc to broker
     */
    return -1;
}

int uviot_node_remove(char *node_name, char *obj_name)
{
    UVIOT_NODE *node;
    
    /*
     * local node?
     */
    node = (UVIOT_NODE *)(json_integer_value(json_object_get(uviot_node_table, node_name)));
    if(node){
        if(node->remove){
            return node->remove(node, obj_name);
        }else{
            return -1;
        }
    }

    /*
     * TODO: remote node, send node rpc to broker
     */
    return -1;
}

static s32 uviot_node_start(struct uviot_event *ev, UVIOT_MSG *msg)
{
    /*
     * TODO: publish and discover node tree
     */
    
	return 0;
}


static UVIOT_MODULE uviot_node_module ={
	.name = "node.uviot",
};

static UVIOT_EVENT uviot_node_event[] = 
{
    {.id = UVIOT_MODULE_START, .handler = uviot_node_start},
};

static int uviot_node_init()
{
	uviot_register_module(&uviot_node_module, uviot_node_event, 
                          ARRAY_SIZE(uviot_node_event));
	return 0;
}

MODULE_INIT(uviot_node_init);

int uviot_node_core_init(void)
{
	uviot_node_table = json_object();
	return 0;
}

CORE_INIT(uviot_node_core_init);

int uviot_register_node(UVIOT_NODE *node, u32 size)
{
	
	if(!node || !node->name){
		return -EINVAL;
	}
	return json_object_set(uviot_node_table, node->name, json_integer((json_int_t)node));
}

int uviot_unregister_node(char *name)
{
	return json_object_del(uviot_node_table, name);
}


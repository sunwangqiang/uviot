#include <uviot.h>

static json_t *uviot_node_table;

static s32 uviot_node_start(struct uviot_event *ev, UVIOT_MSG *msg)
{
    /*
     * TODO: public and discover node tree
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
	return json_object_set(uviot_node_table, name, json_integer((json_int_t)node));
}

int uviot_unregister_node(char *name)
{
	return json_object_del(uviot_node_table, name);
}


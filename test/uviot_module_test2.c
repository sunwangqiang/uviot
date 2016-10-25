#include <uviot.h>

static int test2_node_read(struct uviot_node *node, char *obj_name, json_t *obj)
{
    uviot_log(UVIOT_LOG_DEBUG, "run\n");
    return 0;
}

static int test2_node_write(struct uviot_node *node, char *obj_name, json_t *obj)
{
    uviot_log(UVIOT_LOG_DEBUG, "run\n");
    return 0;
}

static UVIOT_NODE test2_node = {
	.name = "Test2.Node",
    .read = test2_node_read,
    .write = test2_node_write,
};


static s32 test2_ev_handler(struct uviot_event *ev, UVIOT_MSG *msg)
{
	return 0;
}

static s32 test2_mod_start(struct uviot_event *ev, UVIOT_MSG *msg)
{
    UVIOT_MSG msg2;
    
    memset(&msg2, 0, sizeof(UVIOT_MSG));
    
    msg2.id = 0xdeadbeef;
    msg2.dst = "test_mod";
    
    uviot_send_msg(&msg2);
    
	return 0;
}

static UVIOT_MODULE test2_mod ={
	.name = "test2_mod",
};

static UVIOT_EVENT test2_ev[] = 
{
    {.id = 0xdeadbeef, .handler = test2_ev_handler},
    {.id = UVIOT_MODULE_START, .handler = test2_mod_start},
};

static int uviot_module_test_init()
{
	uviot_register_module(&test2_mod, test2_ev, ARRAY_SIZE(test2_ev));
	uviot_register_node(&test2_node, 1);
	
	return 0;
}

MODULE_INIT(uviot_module_test_init);

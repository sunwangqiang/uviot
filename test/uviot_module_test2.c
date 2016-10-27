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


static s32 test2_ev_handler(struct uviot_event *ev, UVIOT_REQ *req)
{
	return 0;
}

static s32 test2_mod_start(struct uviot_event *ev, UVIOT_REQ *req)
{
    UVIOT_REQ req2;
    
    memset(&req2, 0, sizeof(UVIOT_REQ));
    
    req2.id = 0xdeadbeef;
    req2.dst = "test_mod";
    
    uviot_send_req(&req2);
    
    uviot_node_read("Test1.Node", NULL, NULL);
    uviot_node_write("Test2.Node", NULL, NULL);
    
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

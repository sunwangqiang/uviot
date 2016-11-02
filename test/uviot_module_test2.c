#include <uviot.h>

static int test2_node_read(struct uviot_node *node, char *obj_name, UVIOT_REQ *req)
{
    uviot_log(UVIOT_LOG_DEBUG, "run\n");
    return 0;
}

static int test2_node_write(struct uviot_node *node, char *obj_name, UVIOT_REQ *req)
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

static int test2_add_callback(struct uviot_req *req)
{
    uviot_log(UVIOT_LOG_INFO, "result = %"JSON_INTEGER_FORMAT"\n", 
        json_integer_value(json_object_get(req->req, "result")) );
    return 0;
}

static s32 test2_mod_start(struct uviot_event *ev, UVIOT_REQ *req)
{
    UVIOT_REQ *req2, *req3;

    req2 = uviot_alloc_req();
    req3 = uviot_alloc_req();
    if((!req2) || (!req3) ){
        return -1;
    }
    
    req2->id = 0x00000add;
    req2->dst = "test_mod";
    
    req2->req = json_object();
    
    json_object_set(req2->req, "adder1", json_integer(0x32));
    json_object_set(req2->req, "adder2", json_integer(0x32));
    json_object_set(req2->req, "result", json_integer(0x00));
    
    req2->callback = test2_add_callback;
    
    uviot_send_req(req2);
    
    uviot_node_read("Test1.Node", NULL, req3);
    uviot_node_write("Test2.Node", NULL, req3);
    
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

static int uviot_module_test_init(void)
{
	uviot_register_module(&test2_mod, test2_ev, ARRAY_SIZE(test2_ev));
	uviot_register_node(&test2_node, 1);
	
	return 0;
}

MODULE_INIT(uviot_module_test_init);

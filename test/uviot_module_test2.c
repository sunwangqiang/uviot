#include <uvco.h>

static int test2_node_read(struct uvco_node *node, char *obj_name, UVCO_REQ *req)
{
    uvco_log(UVCO_LOG_DEBUG, "run\n");
    return 0;
}

static int test2_node_write(struct uvco_node *node, char *obj_name, UVCO_REQ *req)
{
    uvco_log(UVCO_LOG_DEBUG, "run\n");
    return 0;
}

static UVCO_NODE test2_node = {
    .name = "Test2.Node",
    .read = test2_node_read,
    .write = test2_node_write,
};


static s32 test2_ev_handler(struct uvco_event *ev, UVCO_REQ *req)
{
    return 0;
}

static int test2_add_callback(struct uvco_req *req)
{
    uvco_log(UVCO_LOG_INFO, "result = %"JSON_INTEGER_FORMAT"\n", 
    json_integer_value(json_object_get(req->req, "result")) );
    return 0;
}

static s32 test2_mod_start(struct uvco_event *ev, UVCO_REQ *req)
{
    UVCO_REQ *req2, *req3;

    req2 = uvco_alloc_req();
    req3 = uvco_alloc_req();
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
    
    uvco_send_req(req2);
    
    uvco_node_read("Test1.Node", NULL, req3);
    uvco_node_write("Test2.Node", NULL, req3);
    
    return 0;
}

static UVCO_MODULE test2_mod ={
    .name = "test2_mod",
};

static UVCO_EVENT test2_ev[] = 
{
    {.id = 0xdeadbeef, .handler = test2_ev_handler},
    {.id = UVCO_MODULE_START, .handler = test2_mod_start},
};

static int uvco_module_test_init(void)
{
    uvco_register_module(&test2_mod, test2_ev, ARRAY_SIZE(test2_ev));
    uvco_register_node(&test2_node, 1);
	
    return 0;
}

MODULE_INIT(uvco_module_test_init);

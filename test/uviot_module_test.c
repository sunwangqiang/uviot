#include <uvco.h>

static int test_node_read(struct uvco_node *node, char *obj_name, UVCO_REQ *req)
{
    uvco_log(UVCO_LOG_DEBUG, "run\n");
    return 0;
}

static int test_node_write(struct uvco_node *node, char *obj_name, UVCO_REQ *req)
{
    uvco_log(UVCO_LOG_DEBUG, "run\n");
    return 0;
}

static UVCO_NODE test_node = {
    .name = "Test1.Node",
    .read = test_node_read,
    .write = test_node_write,
};

static s32 test_ev_handler(struct uvco_event *ev, UVCO_REQ *req)
{
    uvco_log(UVCO_LOG_DEBUG, "run\n");
	return 0;
}

static s32 test_ev_add(struct uvco_event *ev, UVCO_REQ *req)
{
    uvco_log(UVCO_LOG_DEBUG, "run\n");

    json_integer_set(json_object_get(req->req, "result"),
        json_integer_value(json_object_get(req->req, "adder1"))+
        json_integer_value(json_object_get(req->req, "adder2")) );

    if(req->callback){
        req->callback(req);
    }
    
    return 0;
}

static s32 test_mod_start(struct uvco_event *ev, UVCO_REQ *req)
{
    uvco_log(UVCO_LOG_DEBUG, "run\n");
    return 0;
}

static UVCO_MODULE test_mod ={
    .name = "test_mod",
};

static UVCO_EVENT test_ev[] = 
{
    {.method = "deadbeef", .handler = test_ev_handler},
    {.method = "add", .handler = test_ev_add},
    {.method = "start", .handler = test_mod_start},
};

static int uvco_module_test_init(void)
{
    uvco_register_module(&test_mod, test_ev, ARRAY_SIZE(test_ev));
    uvco_register_node(&test_node, 1);
	
    return 0;
}

MODULE_INIT(uvco_module_test_init);

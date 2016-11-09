#include <uvco.h>

static int test2_node_read(struct uvco_node *node, char *obj_name, json_t *req, json_t *rsp)
{
    uvco_log(UVCO_LOG_DEBUG, "run\n");
    return 0;
}

static int test2_node_write(struct uvco_node *node, char *obj_name, json_t *req, json_t *rsp)
{
    uvco_log(UVCO_LOG_DEBUG, "run\n");
    return 0;
}

static UVCO_NODE test2_node = {
    .name = "Test2.Node",
    .read = test2_node_read,
    .write = test2_node_write,
};


static s32 test2_ev_handler(struct uvco_event *ev, json_t *req, json_t *rsp)
{
    return 0;
}



static s32 test2_mod_start(struct uvco_event *ev, json_t *req, json_t *rsp)
{
    uvco_log(UVCO_LOG_DEBUG, "run\n");
    return 0;
}

static UVCO_MODULE test2_mod ={
    .name = "uvco.test.test2",
};

static UVCO_EVENT test2_ev[] = 
{
    {.method = "deadbeef", .handler = test2_ev_handler},
    {.method = "start", .handler = test2_mod_start},
};

static int uvco_module_test_init(void)
{
    uvco_register_module(&test2_mod, test2_ev, ARRAY_SIZE(test2_ev));
    uvco_register_node(&test2_node, 1);
	
    return 0;
}

MODULE_INIT(uvco_module_test_init);


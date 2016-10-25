#include <uviot.h>

static UVIOT_NODE test_node = {
	
};

static s32 test_ev_handler(struct uviot_event *ev, UVIOT_MSG *msg)
{
	return 0;
}

static s32 test_mod_start(struct uviot_event *ev, UVIOT_MSG *msg)
{
	return 0;
}

static UVIOT_MODULE test_mod ={
	.name = "test_mod",
};

static UVIOT_EVENT test_ev[] = 
{
    {.id = 0xdeadbeef, .handler = test_ev_handler},
    {.id = UVIOT_MODULE_START, .handler = test_mod_start},
};

static int uviot_module_test_init()
{
	uviot_register_module(&test_mod, test_ev, ARRAY_SIZE(test_ev));
	uviot_register_node(&test_node, 1);
	
	return 0;
}

MODULE_INIT(uviot_module_test_init);

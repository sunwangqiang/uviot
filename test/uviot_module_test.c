#include <uviot.h>


static s32 test_ev_handler(struct uviot_event *ev, void *msg, u32 len)
{
	return 0;
}

static UVIOT_MODULE test_mod ={
	.name = "test_mod",
};

static UVIOT_EVENT test_ev = {
	.id = 0xdeadbeef,
	.handler = test_ev_handler,
};

int uviot_module_test_init()
{
	uviot_register_module(&test_mod, &test_ev, 1);
	return 0;
}

MODULE_INIT(uviot_module_test_init);

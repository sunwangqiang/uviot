#include <uviot.h>
static json_t *uviot_ops_table;

int uviot_register_obj_ops(char *name, UVIOT_OBJ_OPS *ops)
{
	return 0;
}

int uviot_unregister_obj_ops(char *name)
{
	return 0;
}

int uviot_obj_module_init(void)
{
	uviot_ops_table = json_object();
	printf("uviot_ops_table@%p\n", uviot_ops_table);
	return 0;
}

CORE_INIT(uviot_obj_module_init);


#include <uviot.h>

static json_t *uviot_ops_table;

int uviot_register_obj_ops(char *name, UVIOT_OBJ_OPS *ops)
{
	
	if(!name || !ops){
		return -EINVAL;
	}
	return json_object_set(uviot_ops_table, name, json_integer((json_int_t)ops));
}

int uviot_unregister_obj_ops(char *name)
{
	return json_object_del(uviot_ops_table, name);
}

int uviot_obj_module_init(void)
{
	uviot_ops_table = json_object();
	return 0;
}

CORE_INIT(uviot_obj_module_init);


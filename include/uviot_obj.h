#ifndef __UVIOT_OBJ_H
#define __UVIOT_OBJ_H

#include <jansson.h>

typedef struct uviot_obj_ops
{
	int (*open)(struct uviot_obj_ops *ops, char *obj_name, int flags);
	int (*read)(struct uviot_obj_ops *ops, char *obj_name, char *buff, int size);
	int (*write)(struct uviot_obj_ops *ops, char *obj_name, char *buff, int size);
	int (*remove)(struct uviot_obj_ops *ops, char *obj_name);
	json_t *uviot_obj;
	void *priv;
}UVIOT_OBJ_OPS;

int uviot_register_obj_ops(char *name, UVIOT_OBJ_OPS *ops);
int uviot_unregister_obj_ops(char *name);

#endif

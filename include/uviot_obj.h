#ifndef __UVIOT_OBJ_H
#define __UVIOT_OBJ_H

#include <jansson.h>

typedef struct uviot_obj_ops
{
}UVIOT_OBJ_OPS;

int uviot_register_obj_ops(UVIOT_OBJ_OPS *ops);
int uviot_unregister_obj_ops(UVIOT_OBJ_OPS *ops);

#endif

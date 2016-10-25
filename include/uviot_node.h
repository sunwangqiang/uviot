#ifndef __UVIOT_NODE_H
#define __UVIOT_NODE_H

#include <jansson.h>

typedef struct uviot_node
{
    char *name;
	int (*create)(struct uviot_node *node, char *obj_name, int flags);
	int (*read)(struct uviot_node *node, char *obj_name, json_t *obj);
	int (*write)(struct uviot_node *node, char *obj_name, json_t *obj);
    int (*list)(struct uviot_node *node, json_t *obj);
	int (*remove)(struct uviot_node *node, char *obj_name);
	json_t *uviot_obj;
	void *priv;
}UVIOT_NODE;

int uviot_register_node(UVIOT_NODE *n, u32 size);
int uviot_unregister_node(char *name);

int uviot_node_create(char *node_name, char *obj_name, int flags);
int uviot_node_read(char *node_name, char *obj_name, json_t *obj);
int uviot_node_write(char *node_name, char *obj_name, json_t *obj);
int uviot_node_list(char *node_name, json_t *obj);
int uviot_node_remove(char *node_name, char *obj_name);

#endif
#ifndef __UVCO_NODE_H
#define __UVCO_NODE_H

#include <jansson.h>
#include <uvco_req.h>

typedef struct uvco_node
{
    char *name;
	int (*create)(struct uvco_node *node, char *obj_name, UVCO_REQ *req);
	int (*read)(struct uvco_node *node, char *obj_name, UVCO_REQ *req);
	int (*write)(struct uvco_node *node, char *obj_name, UVCO_REQ *req);
    int (*list)(struct uvco_node *node, char *obj_name, UVCO_REQ *req);
	int (*remove)(struct uvco_node *node, char *obj_name, UVCO_REQ *req);
	json_t *uvco_obj;
	void *priv;
}UVCO_NODE;

int uvco_register_node(UVCO_NODE *n, u32 size);
int uvco_unregister_node(char *name);

int uvco_node_create(char *node_name, char *obj_name, UVCO_REQ *req);
int uvco_node_read(char *node_name, char *obj_name, UVCO_REQ *req);
int uvco_node_write(char *node_name, char *obj_name, UVCO_REQ *req);
int uvco_node_list(char *node_name, char *obj_name, UVCO_REQ *req);
int uvco_node_remove(char *node_name, char *obj_name, UVCO_REQ *req);

#endif

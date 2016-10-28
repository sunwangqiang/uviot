/*
 * uviot sdk
 * author:sun.wangqiang@qq.com
 */
#ifndef __UVIOT_MODULE_H
#define __UVIOT_MODULE_H

#include <uv.h>

#define UVIOT_MODULE_NAME_SIZE 16
#define UVIOT_EVENT_SLOT_SIZE 8

#define UVIOT_LIST_MODULE_STOP 0x01
#define UVIOT_LIST_MODULE_CONTINUE 0x02

#define UVIOT_BROADCAST_DST "ffff.ffff.ffff"

typedef struct uviot_module{
    char *name;
    char *ext_address; // application address, unix or inet sock address
    uv_loop_t *loop;
	struct hlist_node hlist;
	struct hlist_head ev_head[UVIOT_EVENT_SLOT_SIZE];
    void *priv;
}UVIOT_MODULE;



#define UVIOT_EVENT_CONTINUE 0x0001
#define UVIOT_EVENT_STOP 0x0002

enum {
    UVIOT_MODULE_START = 1,
    UVIOT_MODULE_STOP,
};

enum uviot_event_hook_priorities
{
    UVIOT_EVENT_PRI_FIRST = (-((int)(~0U>>1))-1),
    UVIOT_EVENT_PRI_HIGH = -2000,
    UVIOT_EVENT_PRI_DEFAULT = 0,
    UVIOT_EVENT_PRI_LOW = 2000,
    UVIOT_EVENT_PRI_LAST = ((int)(~0U>>1))
};

typedef struct uviot_event
{
    u32  id;
    s32 (*handler)(struct uviot_event *, UVIOT_REQ *);
    struct uviot_event *next;
    s32 priority;
    void *priv;
}UVIOT_EVENT;


/*
 * event notifier call chain
 */
typedef struct uviot_event_list
{
	struct hlist_node hlist;
    struct uviot_event *head;
} UVIOT_EVENT_LIST;


// BKDR Hash Function
static inline u32 bkdr_hash(char *str)
{
    u32 seed = 131; // 31 131 1313 13131 131313 etc..
    u32 hash = 0;

    while (*str){
        hash = hash * seed + (*str++);
    }

    return (hash & 0x7FFFFFFF);
}

static inline u32 u32_hash(u32 value)
{
	u32 hash;
	u8 *c;
	
	c = (u8 *)&value;
	hash = c[0] + c[1] + c[2] + c[3];
	
	return hash;
}

int uviot_event_register(struct hlist_head *head, UVIOT_EVENT *ev);
int uviot_event_unregister(struct hlist_head *head, UVIOT_EVENT *ev);
int uviot_event_show(struct hlist_head *head);

int uviot_event_call(UVIOT_MODULE *mod, struct hlist_head *head, UVIOT_REQ *req);

UVIOT_MODULE *uviot_lookup_module(char *name);

void uviot_module_recv_req(UVIOT_MODULE *mod, UVIOT_REQ *req);

int uviot_register_module(UVIOT_MODULE *mod, UVIOT_EVENT *ev, u32 size);
int uviot_unregister_module(UVIOT_MODULE *mod);
int uviot_attach_event(UVIOT_MODULE *mod, UVIOT_EVENT *ev, u32 size);
void uviot_list_each_module(int (*cb)(UVIOT_MODULE *, void *), void *arg);
UVIOT_MODULE *uviot_lookup_module(char *name);

int uviot_section_init(void);
int uviot_module_start(void);

#endif

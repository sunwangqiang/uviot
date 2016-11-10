/*
 * uvco sdk
 * author:sun.wangqiang@qq.com
 */
#ifndef __UVCO_MODULE_H
#define __UVCO_MODULE_H

#include <uv.h>
#include <uvco_task.h>
#include <jansson.h>

#define UVCO_MODULE_NAME_SIZE 16
#define UVCO_EVENT_SLOT_SIZE 8

#define UVCO_LIST_MODULE_STOP 0x01
#define UVCO_LIST_MODULE_CONTINUE 0x02

#define UVCO_BROADCAST_DST "ffff.ffff.ffff"
#define UVCO_DST_KEY "dst"
#define UVCO_SRC_KEY "src"
#define UVCO_METHOD_KEY "method"

typedef struct uvco_module{
    char *name;
    struct hlist_node hlist;
    struct hlist_head ev_head[UVCO_EVENT_SLOT_SIZE];
    UVCO_TASK *task;
    void *priv;
}UVCO_MODULE;


#define UVCO_EVENT_CONTINUE 0x0001
#define UVCO_EVENT_STOP 0x0002

enum {
    UVCO_MODULE_START = 1,
    UVCO_MODULE_STOP,
};

enum uvco_event_hook_priorities
{
    UVCO_EVENT_PRI_FIRST = (-((int)(~0U>>1))-1),
    UVCO_EVENT_PRI_HIGH = -2000,
    UVCO_EVENT_PRI_DEFAULT = 0,
    UVCO_EVENT_PRI_LOW = 2000,
    UVCO_EVENT_PRI_LAST = ((int)(~0U>>1))
};
 
typedef struct uvco_event
{
    char *method;
    s32 (*handler)(struct uvco_event *, json_t *req, json_t *rsp);
    struct uvco_event *next;
    s32 priority;
    void *priv;
}UVCO_EVENT;


/*
 * event notifier call chain
 */
typedef struct uvco_event_list
{
	struct hlist_node hlist;
    struct uvco_event *head;
} UVCO_EVENT_LIST;


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

static inline UVCO_MODULE *uvco_get_module(void)
{
    return (UVCO_MODULE *)container_of(current, UVCO_MODULE, task);
}

int uvco_event_register(struct hlist_head *head, UVCO_EVENT *ev);
int uvco_event_unregister(struct hlist_head *head, UVCO_EVENT *ev);
int uvco_event_show(struct hlist_head *head);

int uvco_event_call(UVCO_MODULE *mod, struct hlist_head *head, char *method,
                    json_t *req, json_t *rsp);

UVCO_MODULE *uvco_lookup_module(char *name);

int uvco_register_module(UVCO_MODULE *mod, UVCO_EVENT *ev, u32 size);
int uvco_unregister_module(UVCO_MODULE *mod);
int uvco_attach_event(UVCO_MODULE *mod, UVCO_EVENT *ev, u32 size);
void uvco_list_each_module(int (*cb)(UVCO_MODULE *, void *), void *arg);
UVCO_MODULE *uvco_lookup_module(char *name);

int uvco_section_init(void);
int uvco_module_start(void);

#endif

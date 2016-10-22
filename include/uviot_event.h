#ifndef __UVIOT_EVENT_HANDLER_H
#define __UVIOT_EVENT_HANDLER_H

#include <uviot_msg.h>

#define UVIOT_EVENT_CONTINUE 0x0001
#define UVIOT_EVENT_STOP 0x0002

enum {
    UVIOT_MODULE_START,
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
    s32 (*handler)(struct uviot_event *, UVIOT_MSG *);
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


int uviot_event_register(struct hlist_head *head, UVIOT_EVENT *ev);
int uviot_event_unregister(struct hlist_head *head, UVIOT_EVENT *ev);
int uviot_event_show(struct hlist_head *head);

int uviot_event_call(struct hlist_head *head, UVIOT_MSG *msg);

#endif


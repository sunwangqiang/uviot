#ifndef __UVIOT_EVENT_HANDLER_H
#define __UVIOT_EVENT_HANDLER_H

#define UVIOT_EVENT_CONTINUE 0x0001
#define UVIOT_EVENT_STOP 0x0002


enum UVIOT_event_hook_priorities
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
    s32 (*handler)(struct uviot_event *, void *msg, u32 len);
    struct uviot_event *next;
    s32 priority;
    void *priv;
}UVIOT_EVENT;


/*
 * event notifier call chain
 */
typedef struct uviot_event_list
{
    struct list_head list;
    struct uviot_event *head;
} UVIOT_EVENT_LIST;

#endif


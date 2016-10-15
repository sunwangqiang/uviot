

#include <uviot.h>

static int __uviot_event_call(UVIOT_EVENT **head, void *msg, u32 len)
{
    UVIOT_EVENT *ev;
    int  ret = UVIOT_EVENT_CONTINUE;

    ev = *head;
    while (ev){
        ret = ev->handler(ev, msg, len);
        if (ret == UVIOT_EVENT_STOP){
            break;
        }
        ev = ev->next;
    }
    return ret;
}

int uviot_event_call(struct list_head *head, u32 id, void *msg, u32 len)
{
    struct list_head *p, *n;
    UVIOT_EVENT_LIST *el;

    list_for_each_safe(p, n, head){
        el = (UVIOT_EVENT_LIST *)list_entry(p, UVIOT_EVENT_LIST, list);
        if (el->head->id != id){
            continue ;
        }
        return __uviot_event_call(&el->head, msg, len);
    }
    return -EINVAL;
}

static int __uviot_event_register(UVIOT_EVENT **head, UVIOT_EVENT *n)
{
    while ((*head) != NULL){
        if (n->priority < (*head)->priority){
            break;
        }
        head = &((*head)->next);
    }
    n->next = *head;
    *head = n;
    return 0;
}

static int __uviot_event_unregister(UVIOT_EVENT **head, UVIOT_EVENT *n)
{
    while ((*head) != NULL){
        if ((*head) == n){
            *head = n->next;
            return 0;
        }
        head = &((*head)->next);
    }
    return -ENOENT;
}

int uviot_event_register(struct list_head *head, UVIOT_EVENT *ev)
{
    struct list_head *p, *n;
    UVIOT_EVENT_LIST *el;

    if (!head || !ev || (!ev->handler)){
        return -EINVAL;
    }

    list_for_each_safe(p, n, head){
        el = (UVIOT_EVENT_LIST *)list_entry(p, UVIOT_EVENT_LIST, list);
        if (el->head->id == ev->id){
            return __uviot_event_register(&el->head, ev);
        }
    }

    /*
     * new event
     */
    el = malloc(sizeof(UVIOT_EVENT_LIST));
    if (!el)
    {
        return -ENOMEM ;
    }
    el->head = ev;
    el->head->next = NULL;

    list_add(&el->list, head);
    return 0;
}

int uviot_event_unregister(struct list_head *head, UVIOT_EVENT *ev)
{
    struct list_head *p, *n;
    UVIOT_EVENT_LIST *el;
    int ret;

    if (!head || !ev || (!ev->handler)){
        return -EINVAL;
    }

    list_for_each_safe(p, n, head){
        el = (UVIOT_EVENT_LIST *)list_entry(p, UVIOT_EVENT_LIST, list);
        if (el->head->id != ev->id){
            continue ;
        }

        ret = __uviot_event_unregister(&el->head, ev);
        if (el->head == NULL){
            list_del(&el->list);
            free(el);
        }
        return ret;
    }

    return -EINVAL;
}






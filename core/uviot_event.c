

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

int uviot_event_call(struct hlist_head *head, u32 id, void *msg, u32 len)
{
    struct hlist_node *p, *n;
    UVIOT_EVENT_LIST *el;

    hlist_for_each_safe(p, n, head){
        el = (UVIOT_EVENT_LIST *)hlist_entry(p, UVIOT_EVENT_LIST, hlist);
        if (el->head->id != id){
            continue ;
        }
        return __uviot_event_call(&el->head, msg, len);
    }
    return -EINVAL;
}

static int __uviot_event_show(UVIOT_EVENT **head)
{
    UVIOT_EVENT *ev;

    ev = *head;
    while (ev){
        printf("event: id = %08x, handler = %p\n", ev->id, ev->handler);;
        ev = ev->next;
    }
    return 0;
}

int uviot_event_show(struct hlist_head *head)
{
    struct hlist_node *p, *n;
    UVIOT_EVENT_LIST *el;

    hlist_for_each_safe(p, n, head){
        el = (UVIOT_EVENT_LIST *)hlist_entry(p, UVIOT_EVENT_LIST, hlist);
        __uviot_event_show(&el->head);
    }
    return 0;
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

int uviot_event_register(struct hlist_head *head, UVIOT_EVENT *ev)
{
    struct hlist_node *p, *n;
    UVIOT_EVENT_LIST *el;

    if (!head || !ev || (!ev->handler)){
        return -EINVAL;
    }

    hlist_for_each_safe(p, n, head){
        el = (UVIOT_EVENT_LIST *)hlist_entry(p, UVIOT_EVENT_LIST, hlist);
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

    hlist_add_head(&el->hlist, head);
    return 0;
}

int uviot_event_unregister(struct hlist_head *head, UVIOT_EVENT *ev)
{
    struct hlist_node *p, *n;
    UVIOT_EVENT_LIST *el;
    int ret;

    if (!head || !ev || (!ev->handler)){
        return -EINVAL;
    }

    hlist_for_each_safe(p, n, head){
        el = (UVIOT_EVENT_LIST *)hlist_entry(p, UVIOT_EVENT_LIST, hlist);
        if (el->head->id != ev->id){
            continue ;
        }

        ret = __uviot_event_unregister(&el->head, ev);
        if (el->head == NULL){
            hlist_del(&el->hlist);
            free(el);
        }
        return ret;
    }

    return -EINVAL;
}






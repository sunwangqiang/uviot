#include <uviot.h>

#define UVIOT_MOD_SLOT_SIZE 64

static struct hlist_head *mod_head;

int uviot_register_module(UVIOT_MODULE *mod, UVIOT_EVENT *ev, u32 size)
{
	u32 hash;
	u32 i;
	
	if((!mod) || (!ev)){
		uviot_log(UVIOT_LOG_ERR, "arg error, mod = %p, ev = %p\n", mod, ev);
		return -EINVAL;
	}
	
	for(i = 0; i< UVIOT_EVENT_SLOT_SIZE; i++){
		INIT_HLIST_HEAD(&mod->ev_head[i]);
	}
	/*
	 * event hash
	 */
	for(i = 0; i< size; i++){
		hash = u32_hash(ev[i].id) & (UVIOT_EVENT_SLOT_SIZE -1);
		uviot_event_register(&mod->ev_head[hash], &ev[i]);
	}
	
	/*
	 * mod hash
	 */
	hash = bkdr_hash(mod->name);
	hash = hash & (UVIOT_MOD_SLOT_SIZE-1);
	
	hlist_add_head(&mod->hlist, &mod_head[hash]);
	
	return 0;
}

/*
 * TODO:
 */
int uviot_unregister_module(UVIOT_MODULE *mod)
{
	return 0;
}

static int uviot_module_init(void)
{
	int size = UVIOT_MOD_SLOT_SIZE * sizeof(struct hlist_head);
	int i;
	
	mod_head = malloc(size);
	if(!mod_head){
		return -ENOMEM;
	}
	memset(mod_head, 0, size);
	
	for(i = 0; i< UVIOT_MOD_SLOT_SIZE; i++){
		INIT_HLIST_HEAD(&mod_head[i]);
	}
	return 0;
}

void uviot_module_process_msg(UVIOT_MODULE *mod, UVIOT_MSG *msg)
{
    u32 hash;
    
    hash = u32_hash(msg->id) & (UVIOT_EVENT_SLOT_SIZE -1);
    
    uviot_event_call(mod, &mod->ev_head[hash], msg);
    
    if(msg->callback){
        msg->callback(msg);
    }
    
    return;
}

UVIOT_MODULE *uviot_lookup_module(char *name)
{
    u32 hash;
    struct hlist_node *p, *n;
    UVIOT_MODULE *mod;
    
    
	hash = bkdr_hash(name);
	hash = hash & (UVIOT_MOD_SLOT_SIZE-1);
    
    hlist_for_each_safe(p, n, &mod_head[hash]){
        mod = (UVIOT_MODULE *)hlist_entry(p, UVIOT_MODULE, hlist);
        if(!strcmp(name, mod->name)){
            return mod;
        }
    }
    return NULL;
}


void uviot_list_each_module(int (*cb)(UVIOT_MODULE *, void *), void *arg)
{
    int i;
    int ret;
    struct hlist_node *p, *n;
    UVIOT_MODULE *mod;
    
    for(i = 0; i<UVIOT_MOD_SLOT_SIZE; i++){
        if(hlist_empty(&mod_head[i])){
            continue;
        }
        hlist_for_each_safe(p, n, &mod_head[i]){
            mod = (UVIOT_MODULE *)hlist_entry(p, UVIOT_MODULE, hlist);
            ret = cb(mod, arg);
            if(ret == UVIOT_LIST_MODULE_STOP){
                return;
            }
        }
    }
    return;
}

int uviot_module_start_callback(struct uviot_msg *msg)
{
    printf("UVIOT_MODULE_START msg callback\n");
    return 0;
}

/*
 * send UVIOT_MODULE_START event to every module
 */
int uviot_module_start(void)
{
    UVIOT_MSG msg;
    
    memset(&msg, 0, sizeof(UVIOT_MSG));
    msg.id = UVIOT_MODULE_START;
    msg.dst = UVIOT_BROADCAST_DST;
    msg.callback = uviot_module_start_callback;
    uviot_process_msg(&msg);
    
    return 0;
}

#ifdef UVIOT_DEBUG

static int uviot_debug_show_event(UVIOT_MODULE *mod)
{
	int i;
	
	for(i = 0; i<UVIOT_EVENT_SLOT_SIZE; i++){
		if(!hlist_empty(&mod->ev_head[i])){
			uviot_event_show(&mod->ev_head[i]);
		}
	}
	return 0;
}

static int uviot_debug(void)
{
	int i;
	struct hlist_node *p, *n;
    UVIOT_MODULE *mod;
	
	for(i = 0; i<UVIOT_MOD_SLOT_SIZE; i++){
		if(!hlist_empty(&mod_head[i])){
			hlist_for_each_safe(p, n, &mod_head[i]){
				mod = (UVIOT_MODULE *)hlist_entry(p, UVIOT_MODULE, hlist);
				printf("mod [%s] event:\n", mod->name);
				uviot_debug_show_event(mod);
			}
		}
	}
	return 0;
}

LATE_INIT(uviot_debug);
#endif



static int __uviot_event_call(UVIOT_MODULE *mod, UVIOT_EVENT **head, UVIOT_MSG *msg)
{
    UVIOT_EVENT *ev;
    int  ret = UVIOT_EVENT_CONTINUE;

    ev = *head;
    while (ev){
        uviot_log(UVIOT_LOG_DEBUG, "module[%s] process msg {dst:%s, src:%s, id:%08x, req:%p, res:%p}\n",
                  mod->name, msg->dst, msg->src, msg->id, msg->req, msg->rsp);
        
        ret = ev->handler(ev, msg);
        if (ret == UVIOT_EVENT_STOP){
            break;
        }
        ev = ev->next;
    }
    return ret;
}

int uviot_event_call(UVIOT_MODULE *mod, struct hlist_head *head, UVIOT_MSG *msg)
{
    struct hlist_node *p, *n;
    UVIOT_EVENT_LIST *el;

    hlist_for_each_safe(p, n, head){
        el = (UVIOT_EVENT_LIST *)hlist_entry(p, UVIOT_EVENT_LIST, hlist);
        if (el->head->id != msg->id){
            continue ;
        }
        return __uviot_event_call(mod, &el->head, msg);
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

BASE_INIT(uviot_module_init);

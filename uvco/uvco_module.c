#include <uvco.h>

#define UVCO_MOD_SLOT_SIZE 64

static struct hlist_head *mod_head;

int uvco_module_attach_event(UVCO_MODULE *mod, UVCO_EVENT *ev, u32 size)
{
	u32 hash;
	u32 i;
    
	/*
	 * event hash
	 */
	for(i = 0; i< size; i++){
		hash = u32_hash(ev[i].id) & (UVCO_EVENT_SLOT_SIZE -1);
		uvco_event_register(&mod->ev_head[hash], &ev[i]);
	}
	
	/*
	 * mod hash
	 */
	hash = bkdr_hash(mod->name);
	hash = hash & (UVCO_MOD_SLOT_SIZE-1);
	
	hlist_add_head(&mod->hlist, &mod_head[hash]);
    
    return 0;
}

int uvco_register_module(UVCO_MODULE *mod, UVCO_EVENT *ev, u32 size)
{
	int i;
    
	if((!mod) || (!ev)){
		uvco_log(UVCO_LOG_ERR, "arg error, mod = %p, ev = %p\n", mod, ev);
		return -EINVAL;
	}
	
	for(i = 0; i< UVCO_EVENT_SLOT_SIZE; i++){
		INIT_HLIST_HEAD(&mod->ev_head[i]);
	}

	uvco_module_attach_event(mod, ev, size);

    /*
     * module has self address
     */
    if(mod->ext_address){
    }
	return 0;
}

/*
 * TODO:
 */
int uvco_unregister_module(UVCO_MODULE *mod)
{
	return 0;
}

static int uvco_module_init(void)
{
	int size = UVCO_MOD_SLOT_SIZE * sizeof(struct hlist_head);
	int i;
	
	mod_head = malloc(size);
	if(!mod_head){
		return -ENOMEM;
	}
	memset(mod_head, 0, size);
	
	for(i = 0; i< UVCO_MOD_SLOT_SIZE; i++){
		INIT_HLIST_HEAD(&mod_head[i]);
	}
	return 0;
}

void uvco_module_recv_req(UVCO_MODULE *mod, UVCO_REQ *req)
{
    u32 hash;
    
    hash = u32_hash(req->id) & (UVCO_EVENT_SLOT_SIZE -1);
    
    uvco_event_call(mod, &mod->ev_head[hash], req);
        
    return;
}

void uvco_module_send_req(UVCO_MODULE *mod, UVCO_REQ *req)
{

    req->src = mod->name;
    uvco_send_req(req);
}

UVCO_MODULE *uvco_lookup_module(char *name)
{
    u32 hash;
    struct hlist_node *p, *n;
    UVCO_MODULE *mod;
    
    
	hash = bkdr_hash(name);
	hash = hash & (UVCO_MOD_SLOT_SIZE-1);
    
    hlist_for_each_safe(p, n, &mod_head[hash]){
        mod = (UVCO_MODULE *)hlist_entry(p, UVCO_MODULE, hlist);
        if(!strcmp(name, mod->name)){
            return mod;
        }
    }
    return NULL;
}


void uvco_list_each_module(int (*cb)(UVCO_MODULE *, void *), void *arg)
{
    int i;
    int ret;
    struct hlist_node *p, *n;
    UVCO_MODULE *mod;
    
    for(i = 0; i<UVCO_MOD_SLOT_SIZE; i++){
        if(hlist_empty(&mod_head[i])){
            continue;
        }
        hlist_for_each_safe(p, n, &mod_head[i]){
            mod = (UVCO_MODULE *)hlist_entry(p, UVCO_MODULE, hlist);
            ret = cb(mod, arg);
            if(ret == UVCO_LIST_MODULE_STOP){
                return;
            }
        }
    }
    return;
}

int uvco_module_start_callback(UVCO_REQ *req)
{
    printf("UVCO_MODULE_START req callback\n");
    return 0;
}

/*
 * send UVCO_MODULE_START event to every module
 */
int uvco_module_start(void)
{
    UVCO_REQ req;
    
    memset(&req, 0, sizeof(UVCO_REQ));
    
    req.id = UVCO_MODULE_START;
    req.dst = UVCO_BROADCAST_DST;
    req.callback = uvco_module_start_callback;
    
    uvco_send_req(&req);
    
    return 0;
}

#ifdef UVCO_DEBUG

static int uvco_debug_show_event(UVCO_MODULE *mod)
{
	int i;
	
	for(i = 0; i<UVCO_EVENT_SLOT_SIZE; i++){
		if(!hlist_empty(&mod->ev_head[i])){
			uvco_event_show(&mod->ev_head[i]);
		}
	}
	return 0;
}

static int uvco_debug(void)
{
	int i;
	struct hlist_node *p, *n;
    UVCO_MODULE *mod;
	
	for(i = 0; i<UVCO_MOD_SLOT_SIZE; i++){
		if(!hlist_empty(&mod_head[i])){
			hlist_for_each_safe(p, n, &mod_head[i]){
				mod = (UVCO_MODULE *)hlist_entry(p, UVCO_MODULE, hlist);
				printf("mod [%s] event:\n", mod->name);
				uvco_debug_show_event(mod);
			}
		}
	}
	return 0;
}

LATE_INIT(uvco_debug);
#endif



static int __uvco_event_call(UVCO_MODULE *mod, UVCO_EVENT **head, UVCO_REQ *req)
{
    UVCO_EVENT *ev;
    int  ret = UVCO_EVENT_CONTINUE;

    ev = *head;
    while (ev){
        uvco_log(UVCO_LOG_DEBUG, "module[%s] process req {dst:%s, src:%s, id:%08x, req:%p, res:%p}\n",
                  mod->name, req->dst, req->src, req->id, req->req, req->rsp);
        
        ret = ev->handler(ev, req);
        if (ret == UVCO_EVENT_STOP){
            break;
        }
        ev = ev->next;
    }
    return ret;
}

int uvco_event_call(UVCO_MODULE *mod, struct hlist_head *head, UVCO_REQ *req)
{
    struct hlist_node *p, *n;
    UVCO_EVENT_LIST *el;

    hlist_for_each_safe(p, n, head){
        el = (UVCO_EVENT_LIST *)hlist_entry(p, UVCO_EVENT_LIST, hlist);
        if (el->head->id != req->id){
            continue ;
        }
        return __uvco_event_call(mod, &el->head, req);
    }
    return -EINVAL;
}

static int __uvco_event_show(UVCO_EVENT **head)
{
    UVCO_EVENT *ev;

    ev = *head;
    while (ev){
        printf("event: id = %08x, handler = %p\n", ev->id, ev->handler);;
        ev = ev->next;
    }
    return 0;
}

int uvco_event_show(struct hlist_head *head)
{
    struct hlist_node *p, *n;
    UVCO_EVENT_LIST *el;

    hlist_for_each_safe(p, n, head){
        el = (UVCO_EVENT_LIST *)hlist_entry(p, UVCO_EVENT_LIST, hlist);
        __uvco_event_show(&el->head);
    }
    return 0;
}

static int __uvco_event_register(UVCO_EVENT **head, UVCO_EVENT *n)
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

static int __uvco_event_unregister(UVCO_EVENT **head, UVCO_EVENT *n)
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

int uvco_event_register(struct hlist_head *head, UVCO_EVENT *ev)
{
    struct hlist_node *p, *n;
    UVCO_EVENT_LIST *el;

    if (!head || !ev || (!ev->handler)){
        return -EINVAL;
    }

    hlist_for_each_safe(p, n, head){
        el = (UVCO_EVENT_LIST *)hlist_entry(p, UVCO_EVENT_LIST, hlist);
        if (el->head->id == ev->id){
            return __uvco_event_register(&el->head, ev);
        }
    }

    /*
     * new event
     */
    el = malloc(sizeof(UVCO_EVENT_LIST));
    if (!el)
    {
        return -ENOMEM ;
    }
    el->head = ev;
    el->head->next = NULL;

    hlist_add_head(&el->hlist, head);
    return 0;
}

int uvco_event_unregister(struct hlist_head *head, UVCO_EVENT *ev)
{
    struct hlist_node *p, *n;
    UVCO_EVENT_LIST *el;
    int ret;

    if (!head || !ev || (!ev->handler)){
        return -EINVAL;
    }

    hlist_for_each_safe(p, n, head){
        el = (UVCO_EVENT_LIST *)hlist_entry(p, UVCO_EVENT_LIST, hlist);
        if (el->head->id != ev->id){
            continue ;
        }

        ret = __uvco_event_unregister(&el->head, ev);
        if (el->head == NULL){
            hlist_del(&el->hlist);
            free(el);
        }
        return ret;
    }

    return -EINVAL;
}

CORE_INIT(uvco_module_init);


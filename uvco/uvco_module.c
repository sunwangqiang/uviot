#include <uvco.h>
#include <uv.h>

#define UVCO_MOD_SLOT_SIZE 64
#define UVCO_MOD_STACK_SIZE 32*1024
#define UVCO_MOD_PIPE_NAME_SIZE 1024

static struct hlist_head *mod_head;

static void uvco_module_default_entry(void *arg)
{
    UVCO_MODULE *mod = (UVCO_MODULE *)arg;
    uv_pipe_t pipe;
    char *pipe_name;
    
    /*
     * create a unix socket and wait on it
     */
    pipe_name = malloc(UVCO_MOD_PIPE_NAME_SIZE);
    if(!pipe_name){
        uvco_log(UVCO_LOG_ERR, "No memory\n");
        uvco_exit_task();
    }
    
    memset(pipe_name, 0, UVCO_MOD_PIPE_NAME_SIZE);
    snprintf(pipe_name, UVCO_MOD_PIPE_NAME_SIZE, "/tmp/%s", mod->name);
    
    uv_pipe_init(uv_default_loop(), &pipe, 1);
    uv_pipe_bind(&pipe, pipe_name);

    //uv_read_start(pipe, uv_alloc_cb alloc_cb, uv_read_cb read_cb)
    free(pipe_name);
}

int uvco_module_attach_event(UVCO_MODULE *mod, UVCO_EVENT *ev, u32 size)
{
    u32 hash;
    u32 i;
    UVCO_MODULE *m;
    struct hlist_node *n;
    struct hlist_head *h;
    
    /*
     * module hash
     */
    hash = bkdr_hash(mod->name);
    hash = hash & (UVCO_MOD_SLOT_SIZE-1);
    h = &mod_head[hash];
    
    hlist_for_each_entry(m, n, h, hlist){
        if(!strcmp(m->name, mod->name)){
            uvco_log(UVCO_LOG_ERR, "all ready has a module named %s\n", mod->name);
            return -1;
        }
    }
    
    hlist_add_head(&mod->hlist, h);
    
    /*
     * event hash
     */
    for(i = 0; i< size; i++){
        hash = bkdr_hash(ev[i].method) & (UVCO_EVENT_SLOT_SIZE -1);
        uvco_event_register(&mod->ev_head[hash], &ev[i]);
    }
	
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

    if(uvco_module_attach_event(mod, ev, size)){
        return -1;
	}

    /*
     * create a uvco_task
     */
    if(!mod->task){
         mod->task = uvco_create_task(mod->name, uvco_module_default_entry, 
                                       mod, UVCO_MOD_STACK_SIZE);
    }
    if(!mod->task){
        uvco_log(UVCO_LOG_ERR, "uvco create task failed\n");
        return -1;
    }
    
    uvco_wakeup_task(mod->task);
    
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
    
    hash = bkdr_hash(req->method) & (UVCO_EVENT_SLOT_SIZE -1);
    
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
    
    req.method = "start";
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
        uvco_log(UVCO_LOG_DEBUG, "module[%s] process req {dst:%s, src:%s, method:%s, req:%p, res:%p}\n",
                  mod->name, req->dst, req->src, req->method, req->req, req->rsp);
        
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
        if (!strcmp(el->head->method, req->method)){
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
        printf("event: method = %s, handler = %p\n", ev->method, ev->handler);;
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
        if (strcmp(el->head->method, ev->method)){
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
        if (!strcmp(el->head->method, ev->method)){
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


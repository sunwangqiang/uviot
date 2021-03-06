
#include <uv.h>
#include <uvco.h>

#include <sys/socket.h>
#include <sys/un.h>

#define UVCO_MOD_SLOT_SIZE 64
#define UVCO_MOD_STACK_SIZE 32*1024
#define UVCO_MOD_PIPE_NAME_SIZE 1024

static struct hlist_head *mod_head;

/*
  the format of req & rsp

   +-----------+
   | dst       |
   +-----------+
   | src       |
   +-----------+
   | method    |
   +-----------+
   |rpc content|
   +-----------+

 */

static int uvco_module_process_req(UVCO_MODULE *mod, char *method, json_t *req, json_t *rsp)
{
    u32 hash;
    
    hash = bkdr_hash(method) & (UVCO_EVENT_SLOT_SIZE -1);
    
    return uvco_event_call(mod, &mod->ev_head[hash], method, req, rsp);  
}

static int uvco_module_process_rsp(UVCO_MODULE *mod, char *method, json_t *req, json_t *rsp)
{
    
    return 0;  
}

/*
 * support json_object_path_get(object, "GetRouteIFInfo->ptRouteIFInfo[2]->")
 */ 
json_t *json_object_iterate_array_get(json_t *object, char *path)
{
    unsigned int index;
    char key[512];
    char pad[512];
    
    if(sscanf(path, "%[^[][%u%s", key, &index, pad) != 3){
        return NULL;
    }
    if(strcmp(pad, "]")){
        return NULL;
    }
    object = json_object_get(object, key);

    return json_array_get(object, index);
}

json_t *json_object_path_get(json_t *object, char *path)
{
    char *key;
    char delims[] = "->";
    char *keypath = strdup(path);
    json_t *child;
    
    if(!object || !keypath){
        return NULL;
    }

    key = strtok(keypath, delims);
    if(!key){
        free(keypath);
        return json_object_get(object, keypath);
    }
    while(key){
        
        child = json_object_get(object, key);
        if(!child){
            
            child = json_object_iterate_array_get(object, key);
            if(!child){
                free(keypath);
                printf("%s: path[%s] is invalid\n", __FUNCTION__, path);
                return NULL;
            }
        }
        
        key = strtok(NULL, delims);
        if(!key){//TODO: check "->" at the of the path
            free(keypath);
            return child;
        }
        
        object = child;
    }
    free(keypath);
    printf("%s: path[%s] is invalid\n", __FUNCTION__, path);
    return NULL;
}

void uvco_module_recv(json_t *req, json_t *rsp)
{
    UVCO_MODULE *mod;
    char *dst;
    char *method;
    int ret;
        
    dst = (char *)json_string_value(json_object_get(req, UVCO_DST_KEY));
    method = (char *)json_string_value(json_object_get(req, UVCO_METHOD_KEY));
    if(!dst || !method){
        json_object_set(rsp, "result", json_string("Invalid arg"));
        return;
    }
    
    mod = uvco_get_module();
    if(strcmp(dst, mod->name)){
        json_object_set(rsp, "result", json_string("Invalid module name"));
        return;
    }
    
    uvco_log(UVCO_LOG_DEBUG, "module[%s] recv req {dst:%s, src:%s, method:%s}\n",
              mod->name, json_string_value(json_object_get(req, "dst")), 
                         json_string_value(json_object_get(req, "src")), 
                         method);

    ret = uvco_module_process_req(mod, method, req, rsp);
    if(!ret){
        uvco_module_process_rsp(mod, method, req, rsp);
    }
}

static void uvco_accept_client(uv_stream_t* stream)
{
    char pipe_name[UVCO_MOD_PIPE_NAME_SIZE];
    size_t len;
    uv_pipe_t client;
    uv_buf_t ibuf, obuf;
    json_t *req, *rsp;
    json_error_t err;
    char *result;
    int status;
    int r;
    
    r = uv_pipe_init(uv_default_loop(), &client, 1);
    assert(r == 0);

    r = uv_accept(stream, (uv_stream_t*)&client);
    assert(r == 0);
    
    while(1){
        uvco_stream_read((uv_stream_t*)&client, &ibuf);
        if(ibuf.len == 0){
            perror(__FUNCTION__);
            continue;
        }
        uvco_log(UVCO_LOG_DEBUG, "recv: %s\n", ibuf.base);

        uv_pipe_getpeername(&client, pipe_name, &len);
        uvco_log(UVCO_LOG_DEBUG, "recv: %s msg\n", pipe_name);
        
        /*
         * decode buf
         */
        req = json_loadb(ibuf.base, ibuf.len, 0, &err);
        rsp = json_object();
        if(req && rsp){
            uvco_module_recv(req, rsp);
        }else{
            json_object_set(rsp, "result", json_string("Invalid arg"));
        }
        
        result = json_dumps(rsp, 0);
        obuf = uv_buf_init(result, strlen(result));//TODO: dump to uv_buf_t
        
        status = uvco_stream_write((uv_stream_t*)&client, &obuf);
        uvco_log(UVCO_LOG_DEBUG, "send: %s, status:%d\n", obuf.base, status);
        
        json_decref(rsp);
        json_decref(req);
        uvco_free_buf(&ibuf);
    }
}

static void uvco_module_main(void *arg)
{
    UVCO_MODULE *mod = (UVCO_MODULE *)arg;
    uv_pipe_t pipe;
    int r;
    uv_loop_t *loop;
    char pipe_name[UVCO_MOD_PIPE_NAME_SIZE];
    
    /*
     * create a unix socket and wait for it
     */    
    memset(pipe_name, 0, UVCO_MOD_PIPE_NAME_SIZE);
    snprintf(pipe_name, UVCO_MOD_PIPE_NAME_SIZE, "/tmp/%s", mod->name);

    unlink(pipe_name);
    
    loop = uv_default_loop();
    r = uv_pipe_init(loop, &pipe, 0);
    assert(r == 0);
  
    r = uv_pipe_bind(&pipe, pipe_name);
    assert(r == 0);
    r = uvco_listen((uv_stream_t*)&pipe, 1, uvco_accept_client);
    assert(r == 0); 
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
        if(!ev[i].method){
            uvco_log(UVCO_LOG_ERR, "%s register a null method\n", mod->name);
            continue;
        }
        hash = bkdr_hash(ev[i].method) & (UVCO_EVENT_SLOT_SIZE -1);
        uvco_event_register(&mod->ev_head[hash], &ev[i]);
    }
	
    return 0;
}

int uvco_register_module(UVCO_MODULE *mod, UVCO_EVENT *ev, u32 size)
{
    int i;
    
    if((!mod) || (!mod->name)|| (!ev)){
        uvco_log(UVCO_LOG_ERR, "arg error, mod = %p, name = %p, ev = %p\n", 
                 mod, mod->name, ev);
        return -EINVAL;
    }

	if(strlen(mod->name) > 512){
        uvco_log(UVCO_LOG_ERR, "mod name too long!\n");
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
         mod->task = uvco_create_task(mod->name, uvco_module_main, 
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

int uvco_module_core_init(void)
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

int uvco_debug(void)
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

#endif

static int __uvco_event_call(UVCO_MODULE *mod, UVCO_EVENT **head,  
                             json_t *req, json_t *rsp)
{
    UVCO_EVENT *ev;
    int  ret = UVCO_EVENT_CONTINUE;

    ev = *head;
    while (ev){
        ret = ev->handler(ev, req, rsp);
        if (ret == UVCO_EVENT_STOP){
            break;
        }
        ev = ev->next;
    }
    return ret;
}

int uvco_event_call(UVCO_MODULE *mod, struct hlist_head *head, char *method,
                    json_t *req, json_t *rsp)
{
    struct hlist_node *p, *n;
    UVCO_EVENT_LIST *el;

    hlist_for_each_safe(p, n, head){
        el = (UVCO_EVENT_LIST *)hlist_entry(p, UVCO_EVENT_LIST, hlist);
        if (strcmp(el->head->method, method)){
            continue ;
        }
        return __uvco_event_call(mod, &el->head, req, rsp);
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

//CORE_INIT(uvco_module_init);


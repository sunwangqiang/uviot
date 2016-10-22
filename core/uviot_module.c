#include <uviot.h>

#define UVIOT_MOD_SLOT_SIZE 64

static struct hlist_head *mod_head;

// BKDR Hash Function
static u32 bkdr_hash(char *str)
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

BASE_INIT(uviot_module_init);

int uviot_module_process_msg(UVIOT_MODULE *mod, UVIOT_MSG *msg)
{
    u32 hash;
    
    hash = u32_hash(msg->id) & (UVIOT_EVENT_SLOT_SIZE -1);
    
    return uviot_event_call(&mod->ev_head[hash], msg);
}

int uviot_broadcast_msg_cb(UVIOT_MODULE *mod, void *arg)
{
    UVIOT_MSG *msg = (UVIOT_MSG *)arg;
    
    uviot_module_process_msg(mod, msg);
    
    return UVIOT_LIST_MODULE_CONTINUE;
}

void uviot_broadcast_msg(UVIOT_MSG *msg)
{
    uviot_list_each_module(uviot_broadcast_msg_cb, msg);
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

/*
 * send UVIOT_MODULE_START event to module
 */
int uviot_module_start(void)
{
    UVIOT_MSG msg;
    
    memset(&msg, 0, sizeof(UVIOT_MSG));
    msg.id = UVIOT_MODULE_START;
    
    uviot_broadcast_msg(&msg);
    
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


/*
 * the following code is for section init
 */


#if defined(__APPLE__) && defined(__MACH__)

#include <mach-o/getsect.h>

void uviot_get_sect_addr(char *sect, initcall_t **start, initcall_t **end)
{
    unsigned long size;

    *start = (initcall_t *)getsectdata("__TEXT", sect, &size);
    *end = (initcall_t *)((char *)(*start) + size);
}

#else // for gcc

int uviot_get_sect_addr(char *sect, initcall_t **start, initcall_t **end)
{
    if(!memcmp(sect, "module_section")){
        *start = &__start_module_section;
        *end = &__stop_module_section;
    }else if(!memcmp(sect, "core_section")){
        *start = &__start_core_section;
        *end = &__stop_core_section;
    }else if(!memcmp(sect, "late_section")){
        *start = &__start_late_section;
        *end = &__stop_late_section;
    }else if(!memcmp(sect, "base_section")){
        *start = &__start_base_section;
        *end = &__stop_base_section;
    }
}

#endif

int uviot_section_init(void)
{
    int result;
    initcall_t *call, *end;

	uviot_get_sect_addr("base_section", &call, &end);
    if(call){
        for (; call<end; call++) {
            result = (*call)();
            if(result){
                uviot_log(UVIOT_LOG_ERR, "failed %p\n", *call);
                return result;
            }
        }
    }
	
    uviot_get_sect_addr("core_section", &call, &end);
    if(call){
        for (; call<end; call++) {
            result = (*call)();
            if(result){
                uviot_log(UVIOT_LOG_ERR, "failed %p\n", *call);
                return result;
            }
        }
    }

    uviot_get_sect_addr("module_section", &call, &end);
    if(call){
        for (; call<end; call++) {
            result = (*call)();
            if(result){
                uviot_log(UVIOT_LOG_ERR, "failed %p\n", *call);
                return result;            
            }        
        }
    }

    uviot_get_sect_addr("late_section", &call, &end);
    if(call){
        for (; call<end; call++) {
            result = (*call)();
            if(result){
                uviot_log(UVIOT_LOG_ERR, "failed %p\n", *call);
                return result;            
            }        
        }
    }
    return 0;
}

static int base_init_dummy(void)
{
    return 0;
}

static int core_init_dummy(void)
{
    return 0;
}
static int module_init_dummy(void)
{
    return 0;
}
static int late_init_dummy(void)
{
    return 0;
}
BASE_INIT(base_init_dummy);
CORE_INIT(core_init_dummy);
MODULE_INIT(module_init_dummy);
LATE_INIT(late_init_dummy);

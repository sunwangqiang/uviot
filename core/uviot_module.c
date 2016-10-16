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


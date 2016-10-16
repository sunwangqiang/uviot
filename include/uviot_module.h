/*
 * uviot sdk
 * author:sun.wangqiang@qq.com
 */
#ifndef __UVIOT_MODULE_H
#define __UVIOT_MODULE_H

#include <uviot_event.h>

#define UVIOT_MODULE_NAME_SIZE 16
#define UVIOT_EVENT_SLOT_SIZE 8

typedef struct uviot_module{
    char name[UVIOT_MODULE_NAME_SIZE];
	struct hlist_node hlist;
	struct list_head ev_list;//TODO: move to hlist
	struct hlist_head ev_head[UVIOT_EVENT_SLOT_SIZE];
}UVIOT_MODULE;

int uviot_register_module(UVIOT_MODULE *mod, UVIOT_EVENT *ev, u32 size);
int uviot_unregister_module(UVIOT_MODULE *mod);

/*
 * the following code is for init calls
 */
typedef int (*initcall_t)(void);
typedef void (*exitcall_t)(void);

#if defined(__APPLE__) && defined(__MACH__)

#define __define_base_initcall(fn) \
static initcall_t __initcall_##fn  \
__attribute__((used, __section__("__TEXT, base_section"))) = fn

#define __define_core_initcall(fn) \
static initcall_t __initcall_##fn  \
__attribute__((used, __section__("__TEXT, core_section"))) = fn

#define __define_module_initcall(fn) \
static initcall_t __initcall_##fn  \
__attribute__((used, __section__("__TEXT, module_section"))) = fn

#define __define_late_initcall(fn) \
static initcall_t __initcall_##fn  \
__attribute__((used, __section__("__TEXT, late_section"))) = fn

#else

#define __define_base_initcall(fn) \
static initcall_t __initcall_##fn  \
__attribute__((used, __section__("base_section"))) = fn

#define __define_core_initcall(fn) \
static initcall_t __initcall_##fn  \
__attribute__((used, __section__("core_section"))) = fn

#define __define_module_initcall(fn) \
static initcall_t __initcall_##fn  \
__attribute__((used, __section__("module_section"))) = fn

#define __define_late_initcall(fn) \
static initcall_t __initcall_##fn  \
__attribute__((used, __section__("late_section"))) = fn


extern initcall_t __start_base_section;
extern initcall_t __stop_base_section;

extern initcall_t __start_core_section;
extern initcall_t __stop_core_section;

extern initcall_t __start_module_section;
extern initcall_t __stop_module_section;

extern initcall_t __start_late_section;
extern initcall_t __stop_late_section;

#endif

#define BASE_INIT(fn)  __define_base_initcall(fn)
#define CORE_INIT(fn) __define_core_initcall(fn)
#define MODULE_INIT(fn)  __define_module_initcall(fn)
#define LATE_INIT(fn) __define_late_initcall(fn)

#endif

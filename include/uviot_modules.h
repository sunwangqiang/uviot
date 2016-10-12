/*
 * uviot sdk
 * author:sun.wangqiang@qq.com
 */
#ifndef __UVIOT_MODULE_H
#define __UVIOT_MODULE_H

typedef int (*initcall_t)(void);
typedef void (*exitcall_t)(void);

#if defined(__APPLE__) && defined(__MACH__)

#define __define_module_initcall(fn) \
 static initcall_t __initcall_##fn  \
 __attribute__((used, __section__("__TEXT, module_section"))) = fn

#define __define_core_initcall(fn) \
 static initcall_t __initcall_##fn  \
 __attribute__((used, __section__("__TEXT, core_section"))) = fn

#define __define_late_initcall(fn) \
 static initcall_t __initcall_##fn  \
 __attribute__((used, __section__("__TEXT, late_section"))) = fn

#else

#define __define_module_initcall(fn) \
 static initcall_t __initcall_##fn  \
 __attribute__((used, __section__("module_section"))) = fn

#define __define_core_initcall(fn) \
 static initcall_t __initcall_##fn  \
 __attribute__((used, __section__("core_section"))) = fn

#define __define_late_initcall(fn) \
 static initcall_t __initcall_##fn  \
 __attribute__((used, __section__("late_section"))) = fn

#endif

extern initcall_t __start_module_section;
extern initcall_t __stop_module_section;
extern initcall_t __start_core_section;
extern initcall_t __stop_core_section;
extern initcall_t __start_late_section;
extern initcall_t __stop_late_section;

#define MODULE_INIT(fn)  __define_module_initcall(fn)
#define CORE_INIT(fn) __define_core_initcall(fn)
#define LATE_INIT(fn) __define_late_initcall(fn)

#endif

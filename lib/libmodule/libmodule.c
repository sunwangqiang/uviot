/*
 * uviot sdk
 * author:sun.wangqiang@qq.com
 */
#include <uviot.h>

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

#if 1
int uviot_init(void)
{
    int result;
    initcall_t *call, *end;

	uviot_get_sect_addr("base_sectiddon", &call, &end);
    if(call){
        for (; call<end; call++) {
            result = (*call)();
            if(result){
                uviot_log(UVIOT_LOG_ERR, "failed %p\n", *call);
                return result;
            }
        }
    }
	
    uviot_get_sect_addr("core_sectiddon", &call, &end);
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


    uviot_log(UVIOT_LOG_INFO, "ok\n");
    
    return result;
}
#endif

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



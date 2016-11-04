
#include <uvco.h>

#if defined(__APPLE__) && defined(__MACH__)

#include <mach-o/getsect.h>

void uvco_get_sect_addr(char *sect, initcall_t **start, initcall_t **end)
{
    unsigned long size;

    *start = (initcall_t *)getsectdata("__TEXT", sect, &size);
    *end = (initcall_t *)((char *)(*start) + size);
}

#else // for gcc

void uvco_get_sect_addr(char *sect, initcall_t **start, initcall_t **end)
{
    if(!strcmp(sect, "module_section")){
        *start = &__start_module_section;
        *end = &__stop_module_section;
    }else if(!strcmp(sect, "core_section")){
        *start = &__start_core_section;
        *end = &__stop_core_section;
    }else if(!strcmp(sect, "late_section")){
        *start = &__start_late_section;
        *end = &__stop_late_section;
    }else if(!strcmp(sect, "base_section")){
        *start = &__start_base_section;
        *end = &__stop_base_section;
    }else{
        uvco_log(UVCO_LOG_ERR, "error!!");
        *start = NULL;
        *end = NULL;
    }
}

#endif

int uvco_section_init(void)
{
    int result;
    initcall_t *call, *end;

	uvco_get_sect_addr("base_section", &call, &end);
    if(call){
        for (; call<end; call++) {
            result = (*call)();
            if(result){
                uvco_log(UVCO_LOG_ERR, "failed %p\n", *call);
                return result;
            }
        }
    }
	
    uvco_get_sect_addr("core_section", &call, &end);
    if(call){
        for (; call<end; call++) {
            result = (*call)();
            if(result){
                uvco_log(UVCO_LOG_ERR, "failed %p\n", *call);
                return result;
            }
        }
    }

    uvco_get_sect_addr("module_section", &call, &end);
    if(call){
        for (; call<end; call++) {
            result = (*call)();
            if(result){
                uvco_log(UVCO_LOG_ERR, "failed %p\n", *call);
                return result;            
            }        
        }
    }

    uvco_get_sect_addr("late_section", &call, &end);
    if(call){
        for (; call<end; call++) {
            result = (*call)();
            if(result){
                uvco_log(UVCO_LOG_ERR, "failed %p\n", *call);
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


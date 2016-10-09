/*
 * uviot sdk
 * author:sun.wangqiang@qq.com
 */
#include <uviot.h>

int vos_init(void)
{
    int result;
    initcall_t *call;

    call = &__start_core_section;
    for (; call<&__stop_core_section; call++) {
        result = (*call)();
        if(result){
            vos_log(VOS_LOG_ERR, "failed %p\n", *call);
            return result;
        }
    }

    call = &__start_module_section;
    for (; call<&__stop_module_section; call++) {
        result = (*call)();
        if(result){
            vos_log(VOS_LOG_ERR, "failed %p\n", *call);
            return result;            
        }        
    }

    call = &__start_late_section;
    for (; call<&__stop_late_section; call++) {
        result = (*call)();
        if(result){
            vos_log(VOS_LOG_ERR, "failed %p\n", *call);
            return result;            
        }        
    }
    vos_log(VOS_LOG_INFO, "ok");
    
    return result;
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

CORE_INIT(core_init_dummy);
MODULE_INIT(module_init_dummy);
LATE_INIT(late_init_dummy);



#include <uvco.h>

static void sleep1_loop(void *arg)
{
    int i = 3;
    
    while(i--){
        uvco_log(UVCO_LOG_INFO, "%s start.....\n", current->name);
        uvco_task_sleep(8000);
        uvco_log(UVCO_LOG_INFO, "%s end  .....\n\n", current->name);
    }    
}

static void sleep2_loop(void *arg)
{
    int i = 3;
    
    while(i--){
        uvco_log(UVCO_LOG_INFO, "%s start.....\n", current->name);
        uvco_task_sleep(3000);
        uvco_log(UVCO_LOG_INFO, "%s end  .....\n\n", current->name);
    }    
}

int uvco_task_test_init(void)
{
    UVCO_TASK *sleep1, *sleep2;
    
    sleep1 = uvco_create_task("sleep1", sleep1_loop, NULL, 32*1024);
    uvco_wakeup_task(sleep1);

    sleep2 = uvco_create_task("sleep2", sleep2_loop, NULL, 32*1024);
    uvco_wakeup_task(sleep2);
    
    return 0;
}

MODULE_INIT(uvco_task_test_init);


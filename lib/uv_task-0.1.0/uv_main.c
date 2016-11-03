#include <uv_task.h>


static void idle_loop(void *arg)
{
    printf("idle run....%d\n", (u32)arg);
}

int main(int argc, char **argv)
{
    UV_TASK *idle;
    
    idle = uv_create_task("idle1", idle_loop, (void *)1, 32*1024);
    uv_wakeup_task(idle);

    idle = uv_create_task("idle2", idle_loop, (void *)2, 32*1024);
    uv_wakeup_task(idle);

    idle = uv_create_task("idle3", idle_loop, (void *)3, 32*1024);
    uv_wakeup_task(idle);
    
    uv_run_scheduler();
    
    printf("schedule returned in main\n");

    return 0;
}


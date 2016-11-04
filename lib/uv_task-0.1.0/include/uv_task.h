#ifndef __UV_TASK_H
#define __UV_TASK_H

#include <uv_context.h>
#include <uviot_list.h>
#include <uviot_type.h>

typedef struct
{
	ucontext_t	uc;
}Context;


#define UV_TASK_RUNNABLE 0x00
#define UV_TASK_SLEEP 0x01
#define UV_TASK_STOP 0x02
#define UV_TASK_EXIT 0x03

typedef struct uv_task{
    char *name;
    struct list_head list;
    void *stack;
    u32 stack_size;
    u32 state;
    Context context;
    void (*entry)(void*);
    void *startarg;
    void *priv;
}UV_TASK;

extern UV_TASK *current;

UV_TASK *uv_create_task(char *name, void (*entry)(void*), void *arg, u32 stack_size);
void uv_wakeup_task(UV_TASK *);
void uv_yield_task(void);
void schedule(void);
UV_TASK *uv_dequeue_task(struct list_head *q);
void uv_enqueue_task(struct list_head *, UV_TASK *);
void uv_free_task(UV_TASK *);
void uv_exit_task(void);
void uv_run_scheduler(void);
void uv_task_sleep(u64 ms);

#endif

#ifndef __UVCO_TASK_H
#define __UVCO_TASK_H

#include <uvco_context.h>
#include <uvco_list.h>
#include <uvco_type.h>

typedef struct
{
	ucontext_t	uc;
}Context;


#define UVCO_TASK_RUNNABLE 0x00
#define UVCO_TASK_SLEEP 0x01
#define UVCO_TASK_STOP 0x02
#define UVCO_TASK_EXIT 0x03

typedef struct uvco_task{
    char *name;
    struct list_head list;
    void *stack;
    u32 stack_size;
    u32 state;
    Context context;
    void (*entry)(void*);
    void *startarg;
    void *priv;
}UVCO_TASK;

extern UVCO_TASK *current;

UVCO_TASK *uvco_create_task(char *name, void (*entry)(void*), void *arg, u32 stack_size);
void uvco_wakeup_task(UVCO_TASK *);
void uvco_yield_task(void);
void schedule(void);
UVCO_TASK *uvco_dequeue_task(struct list_head *q);
void uvco_enqueue_task(struct list_head *, UVCO_TASK *);
void uvco_free_task(UVCO_TASK *);
void uvco_exit_task(void);
void uvco_run_scheduler(void);
void uvco_task_sleep(u64 ms);

#endif

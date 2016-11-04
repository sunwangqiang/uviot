#include <uv.h>
#include <uvco.h>

UVCO_TASK *current;

static LIST_HEAD(running_queue);
static LIST_HEAD(sleep_queue);
static UVCO_TASK init_task = {
    .name = "inittask"
};
static UVCO_TASK *idle_task;

/*
 * This is the TRUE block entry
 */
static void uvco_idle_task(void *arg)
{
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    printf("idle task exit\n");
    exit(0);
}

void uvco_run_scheduler(void)
{
    idle_task = uvco_create_task("idletask", uvco_idle_task, NULL, 16*1024);
    
    current = &init_task;
    schedule();
}

static void uvco_start_task(u32 y, u32 x)
{
	UVCO_TASK *task;
	ulong z;

	z = x<<16;	/* hide undefined 32-bit shift from 32-bit compilers */
	z <<= 16;
	z |= y;
	task = (UVCO_TASK*)z;

    task->entry(task->startarg);
    uvco_exit_task();
    printf("all task exit\n");
}

UVCO_TASK *uvco_create_task(char *name, void (*entry)(void*), void *arg, u32 stack_size)
{
    UVCO_TASK *task;
    sigset_t zero;
	u32 x, y;
	unsigned long z;
    
    task = (UVCO_TASK *)malloc(sizeof(UVCO_TASK) + stack_size);
    if(!task){
        return NULL;
    }
    memset(task, 0, sizeof(UVCO_TASK));
    task->stack = (char *)(task+1);
    task->stack_size = stack_size;
    task->entry = entry;
    task->startarg = arg;
    task->name = strdup(name);
    INIT_LIST_HEAD(&task->list);
    
    sigemptyset(&zero);
    sigprocmask(SIG_BLOCK, &zero, &task->context.uc.uc_sigmask);
    if(getcontext(&task->context.uc) < 0){
        printf("getcontext error\n");
        exit(0);
    }

    /* call makecontext to do the real work. */
    /* leave a few words open on both ends */
    task->context.uc.uc_stack.ss_sp = task->stack+8;
    task->context.uc.uc_stack.ss_size = task->stack_size-64;

	/*
	 * All this magic is because you have to pass makecontext a
	 * function that takes some number of word-sized variables,
	 * and on 64-bit machines pointers are bigger than words.
	 */
    z = (ulong)task;
    y = z;
    z >>= 16;	/* hide undefined 32-bit shift from 32-bit compilers */
    x = z>>16;
    makecontext(&task->context.uc, (void(*)(void))uvco_start_task, 2, y, x);
    
    return task;
}

void uvco_wakeup_task(UVCO_TASK *task)
{
    list_add(&task->list, &running_queue);
}

void uvco_yield_task(void)
{
    list_add_tail(&current->list, &running_queue);
    schedule();
}

UVCO_TASK *uvco_dequeue_task(struct list_head *q)
{
    struct list_head *element;
    
    if(!list_empty(q)){
        element = q->next;
        list_del(element);
        return list_entry(element, UVCO_TASK, list);
    }
    return NULL;
}

void uvco_task_waiton_queue(struct list_head *q)
{    
    uvco_enqueue_task(q, current);
    schedule();
}

void uvco_task_wakeup_queue(struct list_head *q)
{    
    uvco_enqueue_task(q, current);
}

static void uvco_task_sleep_cb(uv_timer_t* timer)
{
    UVCO_TASK *task;

    task = (UVCO_TASK *)timer->data;
    list_del(&task->list);//del from sleep_queue
    
    uvco_wakeup_task(task);
    schedule();
}

void uvco_task_sleep(u64 ms)
{
    uv_timer_t timer;

    timer.data = current;
    uvco_enqueue_task(&sleep_queue, current);
    
    uv_timer_init(uv_default_loop(), &timer);
    uv_timer_start(&timer, uvco_task_sleep_cb, ms, 0);
    
    schedule();
}

UVCO_TASK *uvco_dequeue_running_task(void)
{
    return uvco_dequeue_task(&running_queue);
}

void uvco_enqueue_task(struct list_head *q, UVCO_TASK *task)
{
    list_add(&task->list, q);
}

void uvco_free_task(UVCO_TASK *task)
{
    free(task->name);
    free(task);
}

void uvco_exit_task(void)
{
    current->state = UVCO_TASK_EXIT;
    schedule();
}

static void contextswitch(Context *from, Context *to)
{
	if(swapcontext(&from->uc, &to->uc) < 0){
		printf("swapcontext failed!\n");
		assert(0);
	}
}

void schedule(void)
{
    UVCO_TASK *next;
    UVCO_TASK *prev;
    
    next = uvco_dequeue_running_task();
    if(!next){
        next = idle_task;
    }
    
    prev = current;
    if(prev->state == UVCO_TASK_EXIT){
        uvco_free_task(prev);
        prev = &init_task;
    }
    current = next;
    
    contextswitch(&prev->context, &next->context);
}



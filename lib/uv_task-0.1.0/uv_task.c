#include <uviot.h>

#include <uv_task.h>

static LIST_HEAD(running_queue);
UV_TASK *current;


static void uv_start_task(u32 y, u32 x)
{
	UV_TASK *task;
	ulong z;

	z = x<<16;	/* hide undefined 32-bit shift from 32-bit compilers */
	z <<= 16;
	z |= y;
	task = (UV_TASK*)z;

    printf("taskstart %s\n", task->name);
	task->entry(task->startarg);
    printf("taskexits %s\n", task->name);
	uv_exit_task(task);
    printf("should not reached\n");
}

UV_TASK *uv_create_task(char *name, void (*entry)(void*), void *arg, u32 stack_size)
{
    UV_TASK *task;
    sigset_t zero;
	u32 x, y;
	unsigned long z;
    
    task = (UV_TASK *)malloc(sizeof(UV_TASK) + stack_size);
    if(!task){
        return NULL;
    }
    memset(task, 0, sizeof(UV_TASK));
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
    makecontext(&task->context.uc, (void(*)(void))uv_start_task, 2, y, x);
    
    return task;
}

void uv_wakeup_task(UV_TASK *task)
{
    list_add(&task->list, &running_queue);
}

void uv_yield_task(UV_TASK *task)
{
    list_add_tail(&task->list, &running_queue);
}

UV_TASK *uv_dequeue_task(struct list_head *q)
{
    struct list_head *element;
    
    if(!list_empty(q)){
        element = q->next;
        list_del(element);
        return list_entry(element, UV_TASK, list);
    }
    return NULL;
}

UV_TASK *uv_dequeue_running_task(void)
{
    return uv_dequeue_task(&running_queue);
}

void uv_enqueue_task(struct list_head *q, UV_TASK *task)
{
    if(task->list.next){
        list_del(&task->list);
    }
    list_add(&task->list, q);
}

void uv_free_task(UV_TASK *task)
{
    free(task->name);
    free(task);
}

void uv_exit_task(UV_TASK *task)
{
    task->state = UV_TASK_EXIT;
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
    UV_TASK *next;
    UV_TASK *prev;
    
    next = uv_dequeue_running_task();
    if(!next){
        printf("no runnable task, exit\n");
        exit(1);
    }
    
    prev = current;
    current = next;
    contextswitch(&prev->context, &next->context);
    if(next->state == UV_TASK_EXIT){
        uv_free_task(next);
    }
}

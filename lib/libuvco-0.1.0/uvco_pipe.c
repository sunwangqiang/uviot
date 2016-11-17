
#include <uvco.h>

struct uvco_pipe_connect_ctx{
    UVCO_TASK *task;
    int status;
};
    
static void uvco_pipe_connect_cb(uv_connect_t* req, int status)
{
    UVCO_TASK *task;
    struct uvco_pipe_connect_ctx *ctx;
    
    ctx = (struct uvco_pipe_connect_ctx  *)req->data;

    task = ctx->task;
    ctx->status = status;
    
    uvco_unblock_task(task);
}

int uvco_pipe_connect(uv_pipe_t *handle, const char* name)
{
    uv_connect_t req;
    struct uvco_pipe_connect_ctx ctx;

    ctx.task = current;
    ctx.status = 0;
    
    req.data = &ctx;
    uv_pipe_connect(&req, handle, name, uvco_pipe_connect_cb);
    
    uvco_block_task(current);

    return ctx.status;
}


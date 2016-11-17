#include <uv.h>
#include <uvco.h>



void uvco_simple_open_cb(uv_fs_t *req) 
{
    UVCO_TASK *task;

    task = (UVCO_TASK *)req->data;
    uvco_unblock_task(task);
}

void uvco_simple_read_cb(uv_fs_t *req) 
{
    UVCO_TASK *task;

    task = (UVCO_TASK *)req->data;
    uvco_unblock_task(task);
}

int uvco_simple_read(const char *path, int flags, int mode, 
                     char *buf, unsigned int len, int64_t off)
{
    uv_fs_t open_req;
    uv_fs_t read_req;
    uv_fs_t close_req;
    uv_buf_t iov;
    
    open_req.data = current;
    uv_fs_open(uv_default_loop(), &open_req, path, 
               flags, mode, uvco_simple_open_cb);
    uvco_block_task(current);

    if(open_req.result < 0){
        goto openfail;
    }

    read_req.data = current;
    iov = uv_buf_init(buf, len);
    
    uv_fs_read(uv_default_loop(), &read_req, open_req.result,
        &iov, 1, off, NULL);
    //uvco_block_task(current);

    if (read_req.result < 0) {
        uv_fs_close(uv_default_loop(), &close_req, open_req.result, NULL);
        uv_fs_req_cleanup(&close_req);
        goto readfail;
    }

    return 0;
    
readfail:
    uv_fs_req_cleanup(&read_req);
openfail:
    uv_fs_req_cleanup(&open_req);
    return -1;
}

struct uvco_stream_read_ctx{
    UVCO_TASK *task;
    uv_buf_t *buf;
    ssize_t nread;
};

static void uvco_stream_read_alloc_cb(uv_handle_t* handle, size_t suggested_size,
                            uv_buf_t* buf)
{

    struct uvco_stream_read_ctx *ctx = (struct uvco_stream_read_ctx *)handle->data;
    
    buf->base = ctx->buf->base = malloc(suggested_size);
    buf->len = ctx->buf->len = buf->base?suggested_size:0;
}

static void uvco_stream_read_cb(uv_stream_t* stream, ssize_t nread, const uv_buf_t *buf)
{
    struct uvco_stream_read_ctx *ctx = (struct uvco_stream_read_ctx *)stream->data;
    UVCO_TASK *task;

    uv_read_stop(stream);
    task = ctx->task;
    ctx->nread = nread;
    
    uvco_unblock_task(task);
}

int uvco_stream_read(uv_stream_t* stream, uv_buf_t *buf)
{
    struct uvco_stream_read_ctx ctx;
    
    ctx.task = current;
    ctx.buf = buf;
    ctx.nread = 0;
    
    stream->data = &ctx;
    uv_read_start(stream, uvco_stream_read_alloc_cb, uvco_stream_read_cb);
    uvco_block_task(current);

    if(ctx.nread >= 0){
        buf->base[ctx.nread] = 0;
    }
    return ctx.nread;
}

struct uvco_stream_write_ctx{
    UVCO_TASK *task;
    int status;
};

void uvco_stream_write_cb(uv_write_t* req, int status)
{
    struct uvco_stream_write_ctx *ctx;

    ctx = (struct uvco_stream_write_ctx *)req->data;
    ctx->status = status;

    uvco_unblock_task(ctx->task);
}

int uvco_stream_write(uv_stream_t* stream, uv_buf_t *buf)
{
    uv_write_t req;
    struct uvco_stream_write_ctx ctx;

    ctx.task = current;
    ctx.status = 0;
    
    req.data = &ctx;
    uv_write(&req, stream, buf, 1, uvco_stream_write_cb);
    
    uvco_block_task(current);

    return ctx.status;
}

void uvco_free_buf(uv_buf_t *buf)
{
    if(buf->base){
        free(buf->base);
        buf->base = NULL;
    }
}



struct uvco_listen_ctx{
    UVCO_TASK *task;
    void (*accept)(uv_stream_t *);
    int status;
    uv_stream_t* stream;
};

static void uvco_client_task(void *arg)
{
    struct uvco_listen_ctx *ctx = (struct uvco_listen_ctx *)arg;

    ctx->accept(ctx->stream);
}

void uvco_listen_cb(uv_stream_t* stream, int status)
{
    struct uvco_listen_ctx *ctx = (struct uvco_listen_ctx *)stream->data;
    UVCO_TASK *task;
    
    if(status){
       return; 
    }
    task = uvco_create_task("temp", uvco_client_task, ctx, 0);
    
    uvco_wakeup_task(task);
    schedule();
    //don't wakeup listen task
    return;
}


int uvco_listen(uv_stream_t* stream, int backlog, void (*accept)(uv_stream_t *))
{
    struct uvco_listen_ctx ctx;

    if(!accept){
        return -1;
    }
    
    ctx.task = current;
    ctx.accept = accept;
    ctx.stream = stream;
    
    stream->data = &ctx;
    uv_listen(stream, backlog, uvco_listen_cb);

    //listen task allways sleep
    uvco_block_task(current);

    return 0;
}

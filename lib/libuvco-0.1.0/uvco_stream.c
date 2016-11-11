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
    uvco_unblock_task(task);
}

int uvco_stream_read(uv_stream_t* stream, uv_buf_t *buf)
{
    struct uvco_stream_read_ctx ctx;
    
    ctx.task = current;
    ctx.buf = buf;
    
    stream->data = &ctx;
    uv_read_start(stream, uvco_stream_read_alloc_cb, uvco_stream_read_cb);
    uvco_block_task(current);

    return buf->len;
}

void uvco_free_buf(uv_buf_t *buf)
{
    if(buf->base){
        free(buf->base);
        buf->base = NULL;
    }
}


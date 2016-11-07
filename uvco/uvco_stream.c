#include <uv.h>
#include <uvco_stream.h>

static uvco_fs_alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    
}

static uvco_fs_read_cb(uv_stream_t *stream, ssize_t nread, uv_buf_t *buf);
{
    
}

void uvco_simple_open_cb(uv_fs_t *req) 
{
    UVCO_TASK *task;

    task = (UVCO_TASK *)req->data;
    uvco_unblock_task(task);
}

int uvco_simple_read(const char *path, int flags, int mode, uv_buf_t *buf, int64_t off)
{
    uv_fs_t open_req;
    uv_fs_t read_req;

    open_req.data = current;
    uv_fs_open(uv_default_loop(), &open_req, path, flags, mode, uvco_simple_open_cb);
    uvco_block_task();

    if(open_req->result < 0){
        goto failed;
    }
    
    if(open_req->result >= 0) {
        iov = uv_buf_init(buffer, sizeof(buffer));
        uv_fs_read(uv_default_loop(), &read_req, req->result,
            &iov, 1, -1, file_on_read);
    }

failed:
    uv_fs_req_cleanup(&open_req);
    return -1;
}

int uvco_fs_open(const char * path, int flags, int mode)
{
}

int uvco_fs_read(int fd, uv_buf_t bufs[], unsigned int nbufs)
{
}


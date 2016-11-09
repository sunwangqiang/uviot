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



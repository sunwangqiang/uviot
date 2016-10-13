#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>
#include <sys/types.h>

static uv_loop_t* loop;
static uv_pipe_t stdin_pipe;
static uv_pipe_t stdout_pipe;
static uv_pipe_t file_pipe;

typedef struct {
    uv_write_t req;
    uv_buf_t buf;
} write_req_t;

void free_write_req(uv_write_t *req) 
{
    write_req_t *wr = (write_req_t*) req;
    free(wr->buf.base);
    free(wr);
}

void on_stdout_write(uv_write_t *req, int status)
{
    free_write_req(req);
}
void on_file_write(uv_write_t *req, int status) 
{
    free_write_req(req);
}

void write_data(uv_stream_t *dest, size_t size, uv_buf_t buf, uv_write_cb cb) 
{
    write_req_t *req = (write_req_t*) malloc(sizeof(write_req_t));
    req->buf = uv_buf_init((char*) malloc(size), size);
    memcpy(req->buf.base, buf.base, size);
    uv_write((uv_write_t*) req, (uv_stream_t*)dest, &req->buf, 1, cb);
}

/*
#0  alloc_buffer (handle=0x8049c20, suggested_size=65536, buf=0xbfffb63c) at uv_pipe_stream.c:44
#1  0x00d1f532 in uv__read (loop=0xd29600, w=0x8049c64, events=<value optimized out>) at src/unix/stream.c:1125
#2  uv__stream_io (loop=0xd29600, w=0x8049c64, events=<value optimized out>) at src/unix/stream.c:1259
#3  0x00d2457e in uv__io_poll (loop=0xd29600, timeout=-1) at src/unix/linux-core.c:380
#4  0x00d160d7 in uv_run (loop=0xd29600, mode=UV_RUN_DEFAULT) at src/unix/core.c:354
#5  0x080489aa in main (argc=2, argv=0xbfffe994) at uv_pipe_stream.c:87

*/
void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {

    printf("alloc %d byte memory\n", suggested_size);
    *buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
}


/*

#0  read_stdin (stream=0x8049c20, nread=13, buf=0xbfffb63c) at uv_pipe_stream.c:50
#1  0x00d1f8a9 in uv__read (loop=0xd29600, w=0x8049c64, events=<value optimized out>) at src/unix/stream.c:1192
#2  uv__stream_io (loop=0xd29600, w=0x8049c64, events=<value optimized out>) at src/unix/stream.c:1259
#3  0x00d2457e in uv__io_poll (loop=0xd29600, timeout=-1) at src/unix/linux-core.c:380
#4  0x00d160d7 in uv_run (loop=0xd29600, mode=UV_RUN_DEFAULT) at src/unix/core.c:354
#5  0x080489aa in main (argc=2, argv=0xbfffe994) at uv_pipe_stream.c:87

*/
void read_stdin(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) 
{
    if (nread < 0){
        if (nread == UV_EOF){
            // end of file
            uv_close((uv_handle_t *)&stdin_pipe, NULL);
            uv_close((uv_handle_t *)&stdout_pipe, NULL);
            uv_close((uv_handle_t *)&file_pipe, NULL);
        }
    } else if (nread > 0) {
        write_data((uv_stream_t *)&stdout_pipe, nread, *buf, on_stdout_write);
        write_data((uv_stream_t *)&file_pipe, nread, *buf, on_file_write);
    }
    
    if (buf->base){
        free(buf->base);
    }
}


int main(int argc, char **argv) 
{
    uv_fs_t file_req;
    int fd;

    loop = uv_default_loop();

    uv_pipe_init(loop, &stdin_pipe, 0);
    uv_pipe_open(&stdin_pipe, 0);

    uv_pipe_init(loop, &stdout_pipe, 0);
    uv_pipe_open(&stdout_pipe, 1);

    fd = uv_fs_open(loop, &file_req, argv[1], O_CREAT | O_RDWR, 0644, NULL);
    uv_pipe_init(loop, &file_pipe, 0);
    uv_pipe_open(&file_pipe, fd);

    uv_read_start((uv_stream_t*)&stdin_pipe, alloc_buffer, read_stdin);

    uv_run(loop, UV_RUN_DEFAULT);
    return 0;
}


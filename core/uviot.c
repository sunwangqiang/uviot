#include <uviot.h>
#include <uv.h>

static uv_pipe_t uviot_pipe;
json_t *uviot_cfg;

void uviot_pipe_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf) {
    *buf = uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

void uviot_pipe_recv_data(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf) 
{
    if (nread < 0){
        if (nread == UV_EOF){
            // end of file
            // uv_close((uv_handle_t *)&stdin_pipe, NULL);
            // uv_close((uv_handle_t *)&stdout_pipe, NULL);
            // uv_close((uv_handle_t *)&file_pipe, NULL);
        }
    } else if (nread > 0) {
        // write_data((uv_stream_t *)&stdout_pipe, nread, *buf, on_stdout_write);
        // write_data((uv_stream_t *)&file_pipe, nread, *buf, on_file_write);
		printf("recv data:\n");
		//uviot_log_dump(UVIOT_LOG_DEBUG, buf->base, nread);
    }
    
    if (buf->base){
        free(buf->base);
    }
}

json_t *uviot_load_default_cfg(void)
{
    int ret;
    json_t *cfg;
    
    cfg = json_object();
    if(!cfg){
        return NULL;
    }
    
    ret = json_object_set(cfg, "address", json_string("ipc://"UVIOT_PIPENAME));
    if(ret){
        json_decref(cfg);
        return NULL;
    }
    return cfg;
}

static int uviot_load_cfg(int argc, char *argv[])
{
    json_error_t error;
    
    if(argc < 2){
        uviot_cfg = uviot_load_default_cfg();
    }else{
        uviot_cfg = json_load_file(argv[1], 0, &error);
    }
    if(!uviot_cfg){
        uviot_log(UVIOT_LOG_ERR, "%s\n", error.text);
        return -1;
    }    
    uviot_log(UVIOT_LOG_INFO, "uviot working on %s\n", 
              json_string_value(json_object_get(uviot_cfg, "address")));
    return 0;
}

int uviot_init(int argc, char *argv[])
{
    int result;

    result = uviot_load_cfg(argc, argv);
    if(result){
        return result;
    }
    
    result = uviot_section_init();
    if(result){
        return result;
    }
    result = uviot_module_start();
    if(result){
        return result;
    }
    
    uviot_log(UVIOT_LOG_INFO, "ok\n");
    
    return 0;
}

void my_connection_cb(uv_stream_t* server, int status) {

}

static void main_loop(void *arg)
{
    int ret;
    int i = 20;
    
    unlink(UVIOT_PIPENAME);
    uv_pipe_init(uv_default_loop(), &uviot_pipe, 1);
    ret = uv_pipe_bind(&uviot_pipe, UVIOT_PIPENAME);
    if(ret){
        uviot_log(UVIOT_LOG_ERR, "uv_pipe_bind error\n");
        uv_exit_task();
    }

    printf("block on uv_listen\n");
    
    uv_listen((uv_stream_t*)&uviot_pipe, 128, NULL);
    
    printf("block on uv_run\n");
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);

    while(i--){
        uviot_log(UVIOT_LOG_INFO, "run.....\n");
        uv_yield_task();
    }
}

static void idle_loop(void *arg)
{
    int i = 30;
    
    while(i--){
        uviot_log(UVIOT_LOG_INFO, "run.....\n");
        uv_yield_task();
    }    
}

int main(int argc, char *argv[])
{
    uviot_init(argc, argv);

    UV_TASK *idle;
    
    idle = uv_create_task("mainloop", main_loop, (void *)1, 32*1024);
    uv_wakeup_task(idle);

    idle = uv_create_task("idle2", idle_loop, (void *)2, 32*1024);
    uv_wakeup_task(idle);

    idle = uv_create_task("idle3", idle_loop, (void *)3, 32*1024);
    uv_wakeup_task(idle);
    
    uv_run_scheduler();
    
    //never return
    return 0;
}


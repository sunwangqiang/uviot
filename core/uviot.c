#include <uviot.h>
#include <uv.h>

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

static void sleep1_loop(void *arg)
{
    int i = 3;
    
    while(i--){
        uviot_log(UVIOT_LOG_INFO, "%s start.....\n", current->name);
        uv_task_sleep(8000);
        uviot_log(UVIOT_LOG_INFO, "%s end  .....\n\n", current->name);
    }    
}

static void sleep2_loop(void *arg)
{
    int i = 3;
    
    while(i--){
        uviot_log(UVIOT_LOG_INFO, "%s start.....\n", current->name);
        uv_task_sleep(3000);
        uviot_log(UVIOT_LOG_INFO, "%s end  .....\n\n", current->name);
    }    
}

int main(int argc, char *argv[])
{
    UV_TASK *sleep1, *sleep2;

    uviot_init(argc, argv);
    
    sleep1 = uv_create_task("sleep1", sleep1_loop, NULL, 32*1024);
    uv_wakeup_task(sleep1);

    sleep2 = uv_create_task("sleep2", sleep2_loop, NULL, 32*1024);
    uv_wakeup_task(sleep2);
    
    uv_run_scheduler();
    
    //never return
    return 0;
}


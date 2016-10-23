#include <uviot.h>
#include <uv.h>

extern int uviot_init(void);
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

static int uviot_load_default_cfg()
{
    int ret;
    
    uviot_cfg = json_object();
    if(!uviot_cfg){
        return -1;
    }
    
    ret = json_object_set(uviot_cfg, "address", json_string("ipc://"UVIOT_PIPENAME));
    if(ret){
        return -1;
    }
    return 0;
}

static int uviot_load_cfg(int argc, char *argv[])
{
    json_error_t error;
    
    if(argc < 2){
        uviot_load_default_cfg();
    }else{
        uviot_cfg = json_load_file(argv[1], 0, &error);
        if(!uviot_cfg){
            uviot_log(UVIOT_LOG_ERR, "%s\n", error.text);
            return -1;
        }
    }
    uviot_log(UVIOT_LOG_INFO, "uviot working on %s\n", 
              json_string_value(json_object_get(uviot_cfg, "address")));
    return 0;
}

int uviot_init(void)
{
    int result;
    
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

int main(int argc, char *argv[])
{
    uviot_load_cfg(argc, argv);
    uviot_init();

#if 1
	unlink(UVIOT_PIPENAME);
	uv_pipe_init(uv_default_loop(), &uviot_pipe, 1);
	uv_pipe_bind(&uviot_pipe, UVIOT_PIPENAME);
	
    printf("hello world\n");
    uv_read_start((uv_stream_t*)&uviot_pipe, uviot_pipe_buffer, uviot_pipe_recv_data);
	
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
#endif	
	return 0;
}


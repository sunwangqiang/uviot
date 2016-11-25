#include <uvco.h>
#include <sys/socket.h>
#include <sys/un.h>

#define UVCO_CLI_PIPE_NAME "/tmp/uvco.cli"

void uvco_cli_main(void *startarg)
{
    uv_pipe_t stdin_pipe;
    uv_pipe_t sock_pipe;
    int status;
    uv_buf_t stdin_buf;
    uv_buf_t pipein_buf;
    char *path;
    MAIN_ARG *marg = (MAIN_ARG *)startarg;

    if(marg->argc >= 2){
        path = marg->argv[1];
    }else{
        path = "/tmp/uvco.diag";
    }
    unlink(UVCO_CLI_PIPE_NAME);
    
    uv_pipe_init(uv_default_loop(), &stdin_pipe, 0);
    uv_pipe_open(&stdin_pipe, 0);
  
    uv_pipe_init(uv_default_loop(), &sock_pipe, 1);
    uv_pipe_bind(&sock_pipe, UVCO_CLI_PIPE_NAME);

    status = uvco_pipe_connect(&sock_pipe, path);
    if(status){
        uvco_log(UVCO_LOG_ERR, "connect to %s failed\n", path);
        return;
    }else{
        uvco_log(UVCO_LOG_INFO, "connect to %s success\n", path);
    }
    
    while(1){
        uvco_stream_read((uv_stream_t*)&stdin_pipe, &stdin_buf);
        if(!stdin_buf.len){
            continue;
        }
        uvco_log(UVCO_LOG_INFO, "pid [%d], stdin %s", getpid(), stdin_buf.base);

        status = uvco_stream_write((uv_stream_t*)&sock_pipe, &stdin_buf);
        uvco_log(UVCO_LOG_INFO, "write status = %d\n", status);
        
        status = uvco_stream_read((uv_stream_t*)&sock_pipe, &pipein_buf);
        uvco_log(UVCO_LOG_INFO, "read len = %d\n", status);
        uvco_log(UVCO_LOG_INFO, "pid [%d], pipe reply %s\n", getpid(), pipein_buf.base);
        
        uvco_free_buf(&pipein_buf);
        uvco_free_buf(&stdin_buf);
    }
}

int main(int argc, char *argv[])
{
    UVCO_TASK *task;
    MAIN_ARG arg;

    arg.argc = argc;
    arg.argv = argv;
    
    task = uvco_create_task("uvco_cli", uvco_cli_main, &arg, 0);
    uvco_wakeup_task(task);
    
    uvco_run_scheduler();
    return 0;
}


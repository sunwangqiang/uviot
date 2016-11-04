#include <uvco.h>
#include <uv.h>

json_t *uvco_cfg;

json_t *uvco_load_default_cfg(void)
{
    int ret;
    json_t *cfg;
    
    cfg = json_object();
    if(!cfg){
        return NULL;
    }
    
    ret = json_object_set(cfg, "address", json_string("ipc://"UVCO_PIPENAME));
    if(ret){
        json_decref(cfg);
        return NULL;
    }
    return cfg;
}

static int uvco_load_cfg(int argc, char *argv[])
{
    json_error_t error;
    
    if(argc < 2){
        uvco_cfg = uvco_load_default_cfg();
    }else{
        uvco_cfg = json_load_file(argv[1], 0, &error);
    }
    if(!uvco_cfg){
        uvco_log(UVCO_LOG_ERR, "%s\n", error.text);
        return -1;
    }    
    uvco_log(UVCO_LOG_INFO, "uvco working on %s\n", 
              json_string_value(json_object_get(uvco_cfg, "address")));
    return 0;
}

int uvco_init(int argc, char *argv[])
{
    int result;

    result = uvco_load_cfg(argc, argv);
    if(result){
        return result;
    }
    
    result = uvco_section_init();
    if(result){
        return result;
    }
    result = uvco_module_start();
    if(result){
        return result;
    }
    
    uvco_log(UVCO_LOG_INFO, "ok\n");
    
    return 0;
}

int main(int argc, char *argv[])
{
    uvco_init(argc, argv);
        
    uvco_run_scheduler();
    
    //never return
    return 0;
}


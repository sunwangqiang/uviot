#include <uvco.h>
#include <uv.h>

json_t *uvco_cfg;

static void uvco_load_cfg(void)
{
    json_error_t error;
        
    uvco_cfg = json_load_file(UVCO_DEFAULT_CFG, 0, &error);
    if(!uvco_cfg){
        uvco_cfg = json_object();
    }
    
    if(!json_string_value(json_object_get(uvco_cfg, "address"))){
        json_object_set(uvco_cfg, "address", json_string("ipc://"UVCO_PIPENAME));
    }

    uvco_log(UVCO_LOG_INFO, "uvco working on %s\n", 
              json_string_value(json_object_get(uvco_cfg, "address")));

}

int uvco_init(void)
{
    int result;

    uvco_load_cfg();
    
    result = uvco_section_init();
    if(result){
        return result;
    }
    
    uvco_log(UVCO_LOG_INFO, "ok\n");

    uvco_run_scheduler();//never return
    
    return 0;
}


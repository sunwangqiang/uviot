#include <uvco.h>
#include <uv.h>

extern int uvco_debug(void);

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
    uvco_node_core_init();
    uvco_module_core_init();
    uvco_node_module_init();
    
    result = uvco_section_init();
    if(result){
        return result;
    }
    uvco_diag_module_init();
    
#ifdef UVCO_DEBUG
    uvco_debug();
#endif
    uvco_log(UVCO_LOG_INFO, "ok\n");

    uvco_run_scheduler();//never return
    
    return 0;
}


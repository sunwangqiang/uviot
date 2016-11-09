#include <uvco.h>

static s32 stream_test_start(struct uvco_event *ev, json_t *req, json_t *rsp)
{
#if 0
    int ret;
    char buf[1024];
    
    /*
     * test simple read file
     */
    ret = uvco_simple_read("/home/sunwangqiang/uviot/uviot.json", O_RDONLY, 0, buf, 1024, 0);
    if(ret){
        printf("read uviot.json error\n");
        return 0;
    }
    printf("uviot.json = %s\n", buf);
#endif    
    return 0;
   
}

static UVCO_EVENT stream_ev[] = 
{
    {.method = "start", .handler = stream_test_start},
};

static UVCO_MODULE stream_mod ={
    .name = "uvco.test.stream",
};

static int uvco_stream_test_init(void)
{
    uvco_register_module(&stream_mod, stream_ev, ARRAY_SIZE(stream_ev));	
    return 0;
}

MODULE_INIT(uvco_stream_test_init);

#include <uvco.h>

/*
 * This is a buildin module for diagnose
 * and also it is a demo for how to use module
 */

static s32 uvco_diag_list(UVCO_EVENT *ev,  json_t *req, json_t *rsp)
{
    return 0;
}

static UVCO_EVENT uvco_diag_event[] = {
    {.method = "ls", .handler = uvco_diag_list},
};

static UVCO_MODULE uvco_diag_mod = {
    .name = "uvco.diag", //TODO: strcat with APP name
};

int uvco_diag_module_init(void)
{
    uvco_register_module(&uvco_diag_mod, uvco_diag_event, ARRAY_SIZE(uvco_diag_event));
    return 0;
}



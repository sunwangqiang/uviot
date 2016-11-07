#include <uvco.h>

/*
 * This is a buildin module for diagnose
 * and also it is a demo for how to use module
 */
static UVCO_EVENT uvco_diag_event = {

};

static UVCO_MODULE uvco_diag_mod = {
    .name = "uvco.diag",
};

static int uvco_diag_init(void)
{
    uvco_register_module(&uvco_diag_mod, &uvco_diag_event, sizeof(uvco_diag_event));
    return 0;
}

MODULE_INIT(uvco_diag_init);

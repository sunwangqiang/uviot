
#include <uviot.h>

static int uviot_log_level = UVIOT_LOG_DEBUG;

int uviot_log_get_level(void)
{
    return uviot_log_level;
}


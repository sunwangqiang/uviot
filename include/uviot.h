/*
 * uviot sdk
 * author:sun.wangqiang@qq.com
 */
#ifndef __UVIOT_H
#define __UVIOT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include <uviot_section.h>
#include <uviot_log.h>
#include <uviot_list.h>
#include <uviot_type.h>
#include <uviot_msg.h>
#include <uviot_module.h>
#include <uviot_event.h>
#include <uviot_obj.h>

#define UVIOT_DEBUG 1
#define UVIOT_PIPENAME "/tmp/uviot-unix-sock"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
#endif

#endif


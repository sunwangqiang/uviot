/*
 * uvco sdk
 * author:sun.wangqiang@qq.com
 */
#ifndef __UVCO_H
#define __UVCO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>

#include <uvco_section.h>
#include <uvco_log.h>
#include <uvco_list.h>
#include <uvco_type.h>
#include <uvco_module.h>
#include <uvco_node.h>
#include <uvco_conn.h>
#include <uvco_task.h>
#include <uvco_stream.h>

#define UVCO_DEBUG 1
#define UVCO_PIPENAME "/tmp/uvco-unix-sock"
#define UVCO_DEFAULT_CFG "./uvco.json"

int uvco_init(void);

#endif


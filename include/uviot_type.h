/*
 * the base data structs
 * author:
 * sun.wangqiang@qq.com
 */
#ifndef _UVIOT_TYPE_H
#define _UVIOT_TYPE_H

#undef NULL
#define NULL ((void *)0)

#if 0
typedef _Bool			bool;
enum {
	false	= 0,
	true	= 1
};
#else
#include <stdbool.h>
#endif

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

#endif

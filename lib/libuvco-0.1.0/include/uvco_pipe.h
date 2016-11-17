#ifndef __UVCO_PIPE_H
#define __UVCO_PIPE_H
#include <uv.h>

int uvco_pipe_connect(uv_pipe_t* handle, const char* name);

#endif

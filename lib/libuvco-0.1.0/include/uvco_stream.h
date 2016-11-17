#ifndef __UVCO_STREAM_H
#define __UVCO_STREAM_H
#include <uv.h>

int uvco_simple_read(const char *path, int flags, int mode, 
                     char *buf, unsigned int len, int64_t off);

int uvco_stream_read(uv_stream_t* stream, uv_buf_t *buf);
int uvco_stream_write(uv_stream_t* stream, uv_buf_t *buf);
void uvco_free_buf(uv_buf_t *);
int uvco_listen(uv_stream_t* stream, int backlog, void (*accept)(uv_stream_t *));

#endif

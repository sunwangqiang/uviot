#ifndef __UVCO_STREAM_H
#define __UVCO_STREAM_H


void uvco_read(uv_stream_t* stream, ssize_t nread,  uv_buf_t *buf);

#endif

#ifndef __UVCO_STREAM_H
#define __UVCO_STREAM_H


int uvco_simple_read(const char *path, int flags, int mode, 
                     char *buf, unsigned int len, int64_t off);

#endif

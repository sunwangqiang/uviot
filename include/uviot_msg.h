#ifndef __UVIOT_MSG_H
#define __UVIOT_MSG_H

#include <uviot_module.h>

struct uviot_msg_hdr
{
	char dst[UVIOT_MODULE_NAME_SIZE];
	char src[UVIOT_MODULE_NAME_SIZE];
	u16 format;
};

struct uviot_msg_buffer
{
	struct uviot_msg_hdr *umh;
	char *head;
	char *data;
	char *tail;
	u32 len;
};

struct uviot_msg_buffer *uviot_alloc_msg_buffer(u32 size);
void uviot_free_msg_buffer(struct uviot_msg_buffer *umb);

#endif

#ifndef __UVIOT_MSG_H
#define __UVIOT_MSG_H

#include <uviot_module.h>

typedef struct uviot_msg_hdr
{
	char dst[UVIOT_MODULE_NAME_SIZE];
	char src[UVIOT_MODULE_NAME_SIZE];
	u16 format;
}UVIOT_MSG_HDR;

typedef struct uviot_buffer
{
	struct uviot_msg_hdr *hdr;
	char *head;
	char *data;
	char *tail;
	u32 len;
}UVIOT_BUFFER;

struct uviot_buffer *uviot_alloc_buffer(u32 size);
void uviot_free_buffer(UVIOT_BUFFER *umb);

static inline char *uviot_buff_push(UVIOT_BUFFER *uvb, u32 len)
{
	uvb->data -= len;
	uvb->len += len;
	assert(uvb->data >= uvb->head);
	
	return uvb->data;
}

static inline char *uviot_buff_put(UVIOT_BUFFER *uvb, u32 len)
{
	char *data = uvb->data;
	
	uviot_buff_push(uvb, len);
	
	return data;
}

static inline char *uviot_buff_pull(UVIOT_BUFFER *uvb, u32 len)
{
	uvb->data += len;
	uvb->len -= len;
	assert(uvb->data <= uvb->tail);

	return uvb->data;
}

static inline u32 uviot_buff_roomsize(UVIOT_BUFFER *uvb)
{
	return uvb->tail - uvb->data;
}

static inline int uviot_buff_add_data(UVIOT_BUFFER *uvb, char *data, u32 len)
{
	char *start;
	
	if(uviot_buff_roomsize(uvb) < len){
		return -1;
	}
	start = uviot_buff_put(uvb, len);
	memcpy(start, data, len);
	
	return 0;
}

#endif

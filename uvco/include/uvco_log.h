/*
 * vos sdk
 * author:sun.wangqiang@qq.com
 */
#ifndef __UVCO_LOG_H
#define __UVCO_LOG_H

#define UVCO_LOG_EMERG	0
#define UVCO_LOG_ALERT	1
#define UVCO_LOG_CRIT	2
#define UVCO_LOG_ERR	    3
#define UVCO_LOG_WARNING	4
#define UVCO_LOG_NOTICE	5
#define UVCO_LOG_INFO	6
#define UVCO_LOG_DEBUG	7

#define isprintchar(c)  ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))

int uvco_log_get_level(void);

#define uvco_log(level, fmt, arg...) \
do{ \
    if(level <= uvco_log_get_level()){ \
        printf("[%s:%d] "fmt, __func__, __LINE__, ##arg); \
    } \
}while(0)    

#define uvco_log_dump(level, data, len)                   \
do{                                                           \
    unsigned char *buf = (unsigned char*)(data);                \
    int i, j;                                                 \
    int data_len = (len);                                      \
    if(level <= uvco_log_get_level()){                 \
        printf("%s:%d data = %p, len = %d\n", __func__, __LINE__, buf, data_len);\
        for(i = 0; i < data_len; i += 16) {                            \
            printf("%06x: ", (unsigned int)(i+data));             \
            for (j = 0; j < 16; j++){                             \
                if ((i+j) < data_len)                                  \
                  printf("%02x ", buf[i+j]);                      \
                else                                              \
                  printf("   ");                                  \
            }                                                     \
            printf(" ");                                          \
            for (j=0; j<16; j++) {                                \
                if (i+j < data_len)                                    \
                    printf("%c", isprint(buf[i+j]) ? buf[i+j] : '.');\
            }                                                     \
            printf("\n");                                         \
        }                                                         \
    }                                                             \
}while(0)

#endif

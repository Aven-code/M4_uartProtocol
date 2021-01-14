#ifndef _GLOBAL_H
#define _GLOBAL_H
 
 
#define TRUE 1
#define FALSE 0
/**
 *  功能：
 *  参数：msg_len 消息体长度
 *  返回：
*/
typedef unsigned char u8 ;
typedef unsigned short u16 ;
typedef unsigned int  u32;
typedef  unsigned long u64;
 
#define RECEIVE_MAX 256     //串口接收的字节最大长度
#define RECEV_MSGTYPE_MAX 20   //接收的消息类型种数

#define DBG_PRINTF(fmt, args...)  \
{\
 printf("<<File:%s  Line:%d  Function:%s>> ", __FILE__, __LINE__, __FUNCTION__);\
 printf(fmt, ##args);\
}
#endif





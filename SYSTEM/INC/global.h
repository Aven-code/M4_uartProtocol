#ifndef _GLOBAL_H
#define _GLOBAL_H
 
 
#define TRUE 1
#define FALSE 0
/**
 *  ���ܣ�
 *  ������msg_len ��Ϣ�峤��
 *  ���أ�
*/
typedef unsigned char u8 ;
typedef unsigned short u16 ;
typedef unsigned int  u32;
typedef  unsigned long u64;
 
#define RECEIVE_MAX 256     //���ڽ��յ��ֽ���󳤶�
#define RECEV_MSGTYPE_MAX 20   //���յ���Ϣ��������

#define DBG_PRINTF(fmt, args...)  \
{\
 printf("<<File:%s  Line:%d  Function:%s>> ", __FILE__, __LINE__, __FUNCTION__);\
 printf(fmt, ##args);\
}
#endif





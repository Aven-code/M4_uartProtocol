 /**
  ******************************************************************************
  * @file    ./protocol.h 
  * @author  Aven
  * @version V1.0
  * @date    17-July-2020
  * @brief   有关协议的函数声明
  *   ******************************************************************************
  */
#ifndef _PROTOCOL_H
#define _PROTOCOL_H 
#include "global.h"

 //协议数据结构(暂时用不上)
typedef struct 
{
    u16 head;      //头部
    u16 len;       //长度
    u8 sn;         //序列号
    union
    { 
        u16 word;              //一个字，判断可以根据这个判断
        struct
        {
            u8 type_dire;     //类型和方向
            u8 func_code;    //功能码，读取这个变量相当于解析word
        }word_compos;
    }complex_data;  

    u8 check_code;          //校验码
}Data_Pack;
 
/*
    功能：接收数据，模拟接收中断处理函数
    参数：无
    返回：0 正常接收继续下一个字节
          -1 包协议有误
          1 这个包接收结束，进入下一个包
*/
int scanfData(void);

/*
    功能： 与数组自带的效验码进行匹配 
    参数： arr    需要匹配的数组
          length 协议的字节长度
    返回：无
*/
int isRealCheckCode(u8 const * arr,u16 length);

 
/*
    功能：对数组协议解析
    参数：无
    返回：成功解析返回 TRUE , 失败 FALSE
*/
int parsingAarry(void);



/**
 *  功能：生成校验码
 *  参数：byteArr  字节数组
 *       len 数组长度
 *  返回：这段字节数组的检验码
*/
u8 generateCheckCode(u8 const * byteArr,u8 len);



/**
 *  功能：获得范围的步长
 *  参数：range 等分的值
 *  返回：返回步长　
 **/
float getStride(u8 range);


/**
 *  功能：传入实际值算出比例值
 *  参数：val 实际值
 *  返回：返回比例值
 **/
u8 getProport(float val);


/**
 *  功能：传入比例值算出实际值
 *  参数：val 比例值
 *  返回：返回实际值
 **/
float getSrc(u8 val);



#endif


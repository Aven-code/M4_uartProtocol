/**
*   date： 2020/7/17
*   another: aven
*   time: 15:13
*
**/

#include "global.h"
#include "protocol.h"
#include "funcode.h"
#include "data.h"
#include "stm32f4xx_usart.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern Func_Code receiveMsgType[RECEV_MSGTYPE_MAX];
extern u8 receiveDataArray[RECEIVE_MAX];
extern Stru_Data allConfig;

//如何判断 数据接收停止，1.在尾部增加一个结束符，
//2.读取到长度，按照长度来决定是否读取完了
//（原本的想法是，读取到长度的时候 按照长度来决定读取到的字节,要区分引导头和长度，这么一来就有一个rece_flag标志位）
//新版scanf
//模拟接收数据，
int scanfData() //这个放在中断处理函数中
{
    static u16 i = 0;
    static u8 rece_flag = 0;
     
    if (!rece_flag) //为0表示读引导头，讲引导头放在数组中
    {
        receiveDataArray[i] =  USART_ReceiveData(USART1); 

        if (i == 0 && receiveDataArray[0] != 0xee)
        {
            i = 0;
            rece_flag = 0;                            //下次接收又是读引导头
            memset(receiveDataArray, 0, RECEIVE_MAX); //清空接收数据数组
            return -1;
        }
        else if (i == 1 && receiveDataArray[1] != 0xee)
        {
            i = 0;
            rece_flag = 0;                            //下次接收又是读引导头
            memset(receiveDataArray, 0, RECEIVE_MAX); //清空接收数据数组
            return -1;
        }

        if (i++)
        {
            rece_flag = 1; //表示引导头已读取完成
        }
    }
    else // else 读取剩下的字节
    {
        if (i < 4) //这个判断是为了长度，可不可以字节用长度
        {

           /*执行到这里说明,引导头都正确，这时候再去得到长度的值,长度占两个字节， 取2次*/
           receiveDataArray[i++] = USART_ReceiveData(USART1);
           
        }
        else /*当i == 4 时，说明后面就按照 长度-4来循环获取数据*/
        {
            receiveDataArray[i++] = USART_ReceiveData(USART1);
            //第九个的下标长度是10，所以i++ 如果== 长度表示这个包已经接收完毕
            /*如果长时间没等到来自蓝牙发送的长度，则退出。这里需要创建一个定时器*/
            if (i == ((receiveDataArray[2] << 8) | receiveDataArray[3]))
            {
                u8 sign = parsingAarry(); //调用判断检验码，分析协议的函数,
                i = 0;
                rece_flag = 0;
                memset(receiveDataArray, 0, RECEIVE_MAX); //清空接收数据数组
                return sign ? 1 : -1;                     //sign为TRUE表示数据正常结束，为FALSE表示包协议有误 ,返回-1
            }
            // 反之 如果i 大于了 当前协议的长度，则重新开始,这里加个禁止通信什么的都可以。如果超过三次大于，可以试着重置设备，并告诉用户当前收到干扰
            else if (i > ((receiveDataArray[2] << 8) | receiveDataArray[3]))
            {
                i = 0;
                rece_flag = 0;                            //下次接收又是读引导头
                memset(receiveDataArray, 0, RECEIVE_MAX); //清空接收数据数组
                return -1;
            }
        }
    }
    return 0; //返回0，表示继续接收下一个字节
}

/*协议解析部分*************************************/

int parsingAarry()
{
    u8 i;
    u16 src_funcode, len = (receiveDataArray[2] << 8) | receiveDataArray[3];
    
    if (!isRealCheckCode(receiveDataArray, len)) //如果校验码不对，直接返回
        return FALSE;                            // 0
    src_funcode = (receiveDataArray[5] << 8) | receiveDataArray[6];

    /*分析数组*/
    //整一个最省时的查找算法----插值查找，查找功能码数组中的是否存在
    //先用遍历实现查找，看看效果
    for (i = 0; i < RECEV_MSGTYPE_MAX; i++)
    {
        if (receiveMsgType[i].funcode == src_funcode) //找到相同的
        {
            receiveMsgType[i].fun_ptr(); //消息体长度等与 整长度减去8个字节
            return TRUE;
        }
    }
    return FALSE;
}

//
//自己生成的校验码是否匹配的上协议中的，相同返回1，不同返回0
int isRealCheckCode(u8 const *arr, u16 length)
{
    if (generateCheckCode(arr, length - 1) == *(arr + length - 1))
        return TRUE;
    return FALSE;
}

/**
 *  功能：生成校验码
 *  参数：byteArr  字节数组
 *       len 数组长度
 *  返回：这段字节数组的检验码
*/
u8 generateCheckCode(u8 const *byteArr, u8 len)
{
    u8 sum = 0, i;
    for (i = 0; i < len; i++)
    {
        sum += *(byteArr + i);
    }
    return (0xff - sum + 1);
}






/**
 *  功能：获得范围的步长
 *  参数：range 等分的值
 *  返回：返回步长　
 **/
float getStride(u8 range)
{
    float byte = 255;
    float stride = byte / range; // 得到步长
    return stride;
}

/**
 *  功能：传入实际值算出比例值
 *  参数：val 实际值
 *  返回：返回比例值
 **/
u8 getProport(float val)
{
    u8 ret = val * getStride(24);
    return ret;
}

/**
 *  功能：传入比例值算出实际值
 *  参数：val 比例值
 *  返回：返回实际值
 **/
float getSrc(u8 val)
{
    return val / getStride(24);
}

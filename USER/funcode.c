
/**
  ******************************************************************************
  * @file    ./funcode.c
  * @author  Aven
  * @version V2.0
  * @date    02-August-2020
  * @brief   功能码相关处理函数定义
  *   ******************************************************************************
  */
#include "funcode.h"
#include "protocol.h"
#include <stdio.h>
#include "usart.h"
#include "data.h" 

extern Func_Code receiveMsgType[RECEV_MSGTYPE_MAX];
extern u8 receiveDataArray[RECEIVE_MAX];
extern Stru_Data allConfig;

/**
 * 
 *  功能：状态设备的查询 0x1001 - 0x1801
 *  参数：无 
 *  返回：无
*/
void getStatusInfo(void)
{
    //负责解析出消息体数据即可
}

/**
 *  功能：设置通道参数，用于设定DSP功放板的通道参数 0x1002-0x1802
 *  参数：无
 *  返回：无
*/
void setChannel(void)
{
    /*
        0 表示6音数据
        1 表示EQ 类型的参数
        2 代表低通滤波器参数
        3 代表高通滤波器参数
        4 延时数据
        5 相位数据
        6 静音
        7 全局音量控制
        8 混音音量
        9 通道均衡设置
    */
    u8 argType = receiveDataArray[8]; //通道参数类别

    u8 ch = receiveDataArray[7]; //获取通道
    ch--;                        //发送从0x01开始

    /*判断是什么类型的*/
    if (argType == 0x00)
    {
        argType = 0;
    }
    else if (argType >= 1 && argType <= 31) //EQ类型
    {
        argType = 1;

        allConfig.CH[ch].eq->agin = receiveDataArray[9];                                      //增益,这是一个小数
        allConfig.CH[ch].eq->Center_fre = (receiveDataArray[11] << 8) | receiveDataArray[10]; //中心频率
        allConfig.CH[ch].eq->valueQ = (receiveDataArray[13] << 8) | receiveDataArray[12];     //Q值
    }
    else if (argType == 32) //低通滤波器参数
    {
        argType = 2;
        allConfig.CH[ch].passA.low.type = receiveDataArray[9];                                  //滤波器类型
        allConfig.CH[ch].passA.low.freque = (receiveDataArray[11] << 8) | receiveDataArray[10]; //转折频率
        allConfig.CH[ch].passA.low.Order = receiveDataArray[12];                                //阶数
    }
    else if (argType == 33) //高通滤波器参数
    {
        argType = 3;
        allConfig.CH[ch].passA.hign.type = receiveDataArray[9];                                  //滤波器类型
        allConfig.CH[ch].passA.hign.freque = (receiveDataArray[11] << 8) | receiveDataArray[10]; //转折频率
        allConfig.CH[ch].passA.hign.Order = receiveDataArray[12];                                //阶数
    }
    else if (argType == 34) //延时数据
    {
        argType = 4;
        //没找到相关的信号
    }
    else if (argType == 35) //通道相位
    {
        argType = 5;
        allConfig.CH[ch].passA.Phase = receiveDataArray[9];
        allConfig.CH[ch].passA.value = receiveDataArray[10]; //相位数值，这是小数
    }
    else if (argType == 36) //静音
    {
        argType = 6;
        allConfig.CH[ch].passA.passMute = receiveDataArray[9]; //静音与否
    }
    else if (argType == 37) //全局音量控制
    {
        argType = 6;
        //这个定义很模糊，应指混响
    }
    else if (argType == 38) //混音音量
    {
        u8 i,j = 9;
        argType = 6;
        for (i = 0; i < 16; i++, j++)
        {
            allConfig.CH[ch].mix_all[i] = receiveDataArray[j];
        }
        //后面讲16字节的混音传过去
    }
    else if (argType == 39) //通道均衡设置
    {
        argType = 6;
        //模糊定义
    }

    //回复协议包,默认成功
    sendChannel(1, receiveDataArray[4]);
}

/**
 *  功能：设置DSP的总音量 0x1003-0x1803
 *  参数：无
 *  返回：无
*/
void setTotalVolume(void)
{
    //解析数据
    allConfig.volume = receiveDataArray[7]; //音量0-255
    if(allConfig.volume == 0x00)
        allConfig.Mute = 0x02;  //当声音为0时，则静音
    sendTotalVolume(receiveDataArray[4]);
}

/**
 *  功能：设置重置均衡 0x1004-0x1804
 *  参数：无
 *  返回：无
 *  备注： 重置均衡功能只对当前的通道管用 
*/
void setResetBalance(void)
{
    //eq_stru default_eq = {0x00, 0x02, 0x32}; //默认值

    //只重置当前的均衡器
    u8 j, ch = receiveDataArray[7]; //通道
    ch--;
    if (receiveDataArray[8] == 0x01) //重置均衡到出厂的设定值
    {
        for (j = 0; j < 31; j++)
        {
            allConfig.CH[ch].eq[j].agin = 0x00;       //增益 默认0.0
            allConfig.CH[ch].eq[j].valueQ = 0x02;     //Q值：默认2.525 ，我向下取整，后期再进行小数化
            allConfig.CH[ch].eq[j].Center_fre = 0x32; //频率是由机器随机生成，所以我这里字节默认50Khz
        }

        sendResetBalance(0x01, receiveDataArray[7], receiveDataArray[4]);
    }
    else if (receiveDataArray[8] == 0x02) //恢复均衡器到 预置音效的对应值
    {
        //暂无预置音效数据
        sendResetBalance(0x02,  receiveDataArray[7], receiveDataArray[4]);
    }
}

/**
 *  功能：恢复均衡 0x1005-0x1805
 *  参数：无
 *  返回：无
*/
void setEqualizerRec(void)
{

    if (receiveDataArray[7] == 0x01) //如果要恢复上一次，直接调用allconfig
    {
        //调用allconfig,将allconfig 作为最后一次修改的数据
    }
    else if (receiveDataArray[7] == 0x02) //恢复所有均衡器到预置参数就不要修改allconfig
    {
        //调用函数时，传入默认固定数据即可。
    }
    //回复包
    sendEqualizerRec(receiveDataArray[7], receiveDataArray[4]);
}

/**
 *  功能：静音控制 0x1006 - 0x1806
 *  参数：无
 *  返回：无
*/
void setMute(void)
{
    allConfig.Mute = receiveDataArray[7];
    sendMute(receiveDataArray[4]);
}

/**
 *  功能：设置系统的预置音效 0x1007 - 0x1807
 *  参数：无
 *  返回：无
*/
void setPreSoundEffe(void)
{
}

/**
 *  功能：系统复位 0x1008 - 0x1808
 *  参数：无
 *  返回：无
*/
void systemReset(void)
{
    //判断是否复位
    if (receiveDataArray[7] == 0x01)
    {
        /*系统复位*/
        //这里可以加一个判断，判断复位是否成功
        sendSystemReset(0x01, receiveDataArray[4]);
    }
    else if (receiveDataArray[7] == 0x02)
    {
        sendSystemReset(0x02, receiveDataArray[4]);
    }
}

/**
 *  功能：参量均衡控制 0x1009 - 0x1809
 *  参数：无
 *  返回：无
*/
void setEqualizerCtr(void)
{

    sendEqualizerCtr(receiveDataArray[8], receiveDataArray[7], receiveDataArray[4]);
}

/**
 *  功能：图示均衡 0x100A - 0x180A
 *  参数：无
 *  返回：无
*/
void setGraphicEqualizer(void)
{

    sendGraphicEqualizer(receiveDataArray[8], receiveDataArray[7], receiveDataArray[4]);
}

/**
 *  功能：联调控制 0x100B - 0x180B
 *  参数：无
 *  返回：无
*/
void setJointCtr(void)
{
    //判断联调是否关闭
    if (receiveDataArray[7])
    {
        //关闭
    }

    sendJointCtr(receiveDataArray[4]);
}

/**
 *  功能：延时控制 0x100C - 0x180C
 *  参数：无
 *  返回：无
*/
void setDelayCtr(void)
{
    u8 ch = receiveDataArray[7]; //通道
    ch--;
    /*注意：这里毫秒 厘米 英寸 任何一个数值发生变化其余两个都要发生变化*/
    if (receiveDataArray[10] == 0) //毫秒
    {
        allConfig.CH[ch].ms = (receiveDataArray[9] << 8) | receiveDataArray[8];
        sendDelayCtr(0x00,allConfig.CH[ch].ms,receiveDataArray[7], receiveDataArray[4]);
    }
    else if (receiveDataArray[10] == 1) //厘米
    {
        allConfig.CH[ch].cm = (receiveDataArray[9] << 8) | receiveDataArray[8];
        sendDelayCtr(0x01,allConfig.CH[ch].cm,receiveDataArray[7], receiveDataArray[4]);
    }
    else if (receiveDataArray[10] == 2) //英寸
    {
        allConfig.CH[ch].inch = (receiveDataArray[9] << 8) | receiveDataArray[8];
        sendDelayCtr(0x02,allConfig.CH[ch].inch,receiveDataArray[7], receiveDataArray[4]);
    }
  
}

/**
 *  功能：预留1 0x100D - 0x180D
 *  参数：无
 *  返回：无
*/
void reserve1(void)
{
}

/**
 *  功能：预留2 0x100E - 0x180E
 *  参数：无
 *  返回：无
*/
void reserve2(void)
{
}

/**
 *  功能：预留3 0x100F - 0x180F
 *  参数：无
 *  返回：无
*/
void reserve3(void)
{
}

/**
 *  功能：上报设别异常状态信息（主动）0x2000
 *  参数：无
 *  返回：无
*/
void sendErrorStatusInfo(void)
{
}

/*
 *  功能：请求设备信息 0x5001-0x5801
 *  参数：无
 *  返回：无
*/
void sendEquipmentInfo(void)
{
    u16 i;

    //信息返回(模拟串口)
    u8 send_data[] = {0xee, 0xee, 0x00, 0x13, 0x00,
                      0x58, 0x01, 0x12, 0x34, 0x45,
                      0x01, 0x01, 0x02, 0x00, 0x00,
                      0x00, 0x00, 0x06};
    u16 len = (u16)sizeof(send_data);
    for (i = 0; i < len; i++)
    {
      usart1_putchar(send_data[i]); 
    }
  
}

/**
 *  功能：蓝牙配对 0x5007- 0x5807
 *  参数：无
 *  返回：无
*/
void BluetoothMatch(void)
{
    //5007 应该是没有是没有消息体的，所以参数msg_len用不上
    //进入来该函数用于将当前底层的值传给蓝牙
    u8 i;
    //声明固定的内容字节
    u8 info[12] = {0xee, 0xee, 0x00, 0x0C, 0x00, 0x58, 0x07, 0x51, 0x51, 0x44, 0x4d, 0x00};
    info[4] = receiveDataArray[4];               //来自蓝牙的SN序列号
    info[11] = generateCheckCode(info, info[3]); //校验码
    for (i = 0; i < 12; i++)
    {
			usart1_putchar(info[i]); 
    }

    //配对成功发送对应协议包之后初始化上位机
    initApp();
}

/*相关返回包函数 ******************************************************************************************************************/
//所有的返回包会在MCU上电和连接蓝牙APP时发送一遍(除了系统复位)
/********************************************************************************************************************************/

/**
 *  功能：发送状态设备的查询数据包 0x1001 - 0x1801
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendStatusInfo(u8 sn_code)
{
}

/**
 *  功能：发送设置通道参数的查询数据包  0x1002-0x1802
 *  参数：falg 成功1 失败0 
 *        sn_code 序号码
 *  返回：无
*/
void sendChannel(u8 flag, u8 sn_code)
{
    //根据 receiveDataArray  重新构建一个数组
    u8 i, reply[11] = {0xee, 0xee, 0x00, 0x0B, 0x00, 0x18, 0x02, 0x00, 0x00, 0x00, 0x00};
		reply[4] = sn_code;
    reply[7] = receiveDataArray[7];           //第几通道
    reply[8] = receiveDataArray[8];           //参数类别
		reply[9] = flag;
    reply[10] = generateCheckCode(reply, 10); //校验码

    //发送
    for (i = 0; i < 11; i++)
    {
			usart1_putchar(reply[i]); 
    }
}

/**
 *  功能：发送DSP的总音量数据包 0x1003-0x1803
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendTotalVolume(u8 sn_code)
{
    u8 i, reply[9] = {0xee, 0xee, 0x00, 0x09, 0x00, 0x18, 0x03, 0x00, 0x00};
		reply[4] = sn_code; 
    reply[7] = allConfig.volume;            //设定成功之后的音量
    reply[8] = generateCheckCode(reply, 8); //校验码

    for (i = 0; i < 9; i++)
    {
			usart1_putchar(reply[i]);	
    }
}

/**
 *  功能：发送设置重置均衡数据包 0x1004-0x1804
 *  参数：type 重置类型
 *        ch 通道 
 *        sn_code 序号码
 *  返回：无
*/
void sendResetBalance(u8 type, u8 ch, u8 sn_code)
{
    u8 i, reply[10] = {0xee, 0xee, 0x00, 0x0A, 0x00, 0x18, 0x04, 0x00, 0x00, 0x00};
		reply[4] = sn_code; 
		reply[7] = ch;
		reply[8] = type; 		
    reply[9] = generateCheckCode(reply, 9); //校验码

    for (i = 0; i < 10; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  功能：发送恢复均衡数据包  0x1005-0x1805
 *  参数：type 恢复类型
 *        sn_code 序号码
 *  返回：无
*/
void sendEqualizerRec(u8 type, u8 sn_code)
{
    u8 i, reply[9] = {0xee, 0xee, 0x00, 0x09, 0x00, 0x18, 0x05, 0x00, 0x00};
		reply[4] = sn_code;   
		reply[7] = type; 
		reply[8] = generateCheckCode(reply, 8); //校验码

    for (i = 0; i < 9; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  功能：发送静音控制数据包 0x1006 - 0x1806
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendMute(u8 sn_code)
{
    u8 i, reply[9] = {0xee, 0xee, 0x00, 0x09, 0x00, 0x18, 0x06, 0x00, 0x00};
		reply[4] = sn_code; 
    reply[7] = allConfig.Mute;
    reply[8] = generateCheckCode(reply, 8); //校验码

    for (i = 0; i < 9; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  功能：发送设置系统的预置音效数据包 0x1007 - 0x1807
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendPreSoundEffe(u8 sn_code)
{
}

/**
 *  功能：发送系统复位返回包 0x1008 - 0x1808
 *  参数：reset  系统复位 
 *       sn_code 序号码
 *  返回：无
*/
void sendSystemReset(u8 reset, u8 sn_code)
{
    u8 i, reply[9] = {0xee, 0xee, 0x00, 0x09, 0x00, 0x18, 0x08, 0x00, 0x00};
		reply[4] = sn_code; 
		reply[7] = reset;
    reply[8] = generateCheckCode(reply, 8); //校验码

    for (i = 0; i < 9; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  功能：发送参量均衡控制数据包 0x1009 - 0x1809
 *  参数：type 重置类型
 *        ch 通道 
 *        sn_code 序号码
 *  返回：无
*/
void sendEqualizerCtr(u8 type, u8 ch, u8 sn_code)
{
    u8 i, reply[10] = {0xee, 0xee, 0x00, 0x0A, 0x00, 0x18, 0x09, 0x00, 0x00, 0x00};
		reply[4] = sn_code; 
		reply[7] = ch; 
		reply[8] = type; 
    reply[9] = generateCheckCode(reply, 9); //校验码
    for (i = 0; i < 10; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  功能：发送图示均衡数据包 0x100A - 0x180A
 *  参数：type 重置类型
 *        ch 通道 
 *        sn_code 序号码
 *  返回：无
*/
void sendGraphicEqualizer(u8 type, u8 ch, u8 sn_code)
{
    u8 i, reply[10] = {0xee, 0xee, 0x00, 0x0A, 0x00, 0x18, 0x0A, 0x00, 0x00, 0x00};
		reply[4] = sn_code; 
		reply[7] = ch; 
		reply[8] = type; 
    reply[9] = generateCheckCode(reply, 9); //校验码
    for (i = 0; i < 10; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  功能：发送联调控制数据包 0x100B - 0x180B
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendJointCtr(u8 sn_code)
{
    u8 i, reply[9] = {0xee, 0xee, 0x00, 0x09, 0x00, 0x18, 0x0B, 0x00, 0x00};
		reply[4] = sn_code; 
    reply[7] = receiveDataArray[7];
    reply[8] = generateCheckCode(reply, 8); //校验码

    for (i = 0; i < 9; i++)
    {
         usart1_putchar(reply[i]);	
    }
}

/**
 *  功能：发送延时控制数据包 0x100C - 0x180C
 *  参数：ch 通道
 *        sn_code 序号码
 *        unit 延迟单位
 *        value 延时数据
 *  返回：无
*/
void sendDelayCtr(u8 unit,u16 value,u8 ch, u8 sn_code)
{
    u8 i, reply[12] = {0xee, 0xee, 0x00, 0x0C, 0x00, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00};
		reply[4] = sn_code;   
		reply[7] = ch; 
		reply[8] = value & 0xff;   //低位
    reply[9] = (value>>8) & 0xff; //高位
		reply[10] = unit; 
    reply[11] = generateCheckCode(reply, 11); //校验码

    for (i = 0; i < 12; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  功能：发送预留1 0x100D - 0x180D
 *  参数：sn_code 序号码
 *  返回：无
 *  备注：发送通道这里自定义一个函数，将通道的所有参数都发送过去，按照1002 的协议恢复包
*/
void sendReserve1(u8 sn_code)
{
#if 0 
		u8 ch;   //通道从1开始
    u8 para = 0; //参数
    u8 i, reply[11] = {0xEE, 0xEE, 0x00, 0x0B, 0x00, 0x18, 0x02, 0x00, 0x00, 0x01, 0x00};
		reply[4] = sn_code;   
    for (ch = 1; ch < 13; ch++)
    {
				
    }
		
#endif
}

/**
 *  功能：发送预留2 0x100E - 0x180E
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendReserve2(u8 sn_code)
{
}

/**
 *  功能：发送预留3 0x100F - 0x180F
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendReserve3(u8 sn_code)
{
}

/**
 *  功能：初始化上位机
 *  参数：无
 *  返回：无
 *  备注：在MCU上电和连接蓝牙APP时发送一遍数据包，以便初始化数据,SN 统统为0x00
*/
void initApp(void)
{

    u8 i;
    // sendStatusInfo(0);          //发送状态设备的查询数据包 -0x1801
    //发送通道这里自定义一个函数，将通道的所有参数都发送过去，按照1002 的协议恢复包

    sendTotalVolume(0); //发送DSP的总音量数据包 -0x1803
                        // sendResetBalance(0);      //发送设置重置均衡数据包 -0x1804
                        // sendEqualizerRec(0);      //发送恢复均衡数据包  -0x1805
    sendMute(0);        //发送静音控制数据包 -0x1806
    //sendPreSoundEffe(0);      //发送设置系统的预置音效数据包 -0x1807
    //sendEqualizerCtr(0);      //发送参量均衡控制数据包 -0x1809
    //sendGraphicEqualizer(0);  //发送图示均衡数据包 -0x180A
    //sendJointCtr(0);          //发送联调控制数据包 -0x180B

    //发送每一个通道的延时数据
    for (i = 1; i < 13; i++)
        sendDelayCtr(0,allConfig.CH[i].ms,i, 0); //发送延时控制数据包 -0x180C
}



 /**
  ******************************************************************************
  * @file    ./funcode.h 
  * @author  Aven
  * @version V1.0
  * @date    04-June-2020
  * @brief   功能码相关处理函数声明
  *   ******************************************************************************
  */
#ifndef _FUNCODE_H
#define _FUNCODE_H

#define CHANNEL_AGR_TYPE_MAX 40    //通道内参数类别的个数 指令码：0x1002

#include "global.h"



/*功能码结构体类型*/
typedef struct 
{
    u16 funcode;                           //功能码
    void (*fun_ptr)(void);          //功能码所对应的函数指针,形参是消息体长度
}Func_Code;


/**
 *  功能：状态设备的查询 0x1001 - 0x1801
 *  参数：无
 *  返回：无
*/
void getStatusInfo(void);


/**
 *  功能：设置通道参数，用于设定DSP功放板的通道参数 0x1002-0x1802
 *  参数：无
 *  返回：无
*/
void setChannel(void);

 

 
/**
 *  功能：设置DSP的总音量 0x1003-0x1803
 *  参数：无
 *  返回：无
*/
void setTotalVolume(void);

/**
 *  功能：设置重置均衡 0x1004
 *  参数：无
 *  返回：无
*/
void  setResetBalance(void);
 
/**
 *  功能：恢复均衡 0x1005-0x1805
 *  参数：无
 *  返回：无
*/
void setEqualizerRec(void);

/**
 *  功能：静音控制 0x1006 - 0x1806
 *  参数：无
 *  返回：无
*/
void setMute(void);



/**
 *  功能：设置系统的预置音效 0x1007 - 0x1807
 *  参数：无
 *  返回：无
*/
void setPreSoundEffe(void);


/**
 *  功能：系统复位 0x1008 - 0x1808
 *  参数：无
 *  返回：无
*/
void systemReset(void);


/**
 *  功能：参量均衡控制 0x1009 - 0x1809
 *  参数：无
 *  返回：无
*/
void setEqualizerCtr(void);


/**
 *  功能：图示均衡 0x100A - 0x180A
 *  参数：无
 *  返回：无
*/
void setGraphicEqualizer(void);

/**
 *  功能：联调控制 0x100B - 0x180B
 *  参数：无
 *  返回：无
*/
void setJointCtr(void);

/**
 *  功能：延时控制 0x100C - 0x180C
 *  参数：无
 *  返回：无
*/
void setDelayCtr(void);

/**
 *  功能：预留1 0x100D - 0x180D
 *  参数：无
 *  返回：无
*/
void  reserve1(void);


/**
 *  功能：预留2 0x100E - 0x180E
 *  参数：无
 *  返回：无
*/
void  reserve2(void);


/**
 *  功能：预留3 0x100F - 0x180F
 *  参数：无
 *  返回：无
*/
void  reserve3(void);


/**
 *  功能：上报设别异常状态信息（主动）0x2000
 *  参数：无
 *  返回：无
*/
void sendErrorStatusInfo(void);

/*
 *  功能：请求设备信息 0x5001-0x5801
 *  参数：无
 *  返回：无
*/
void sendEquipmentInfo (void);

/**
 *  功能：蓝牙配对 0x5007- 0x5807
 *  参数：无
 *  返回：无
*/
void BluetoothMatch(void);
 



/*相关返回包函数 *************************************************************************************/
//所有的返回包会在MCU上电和连接蓝牙APP时发送一遍(除了系统复位)
/*****************************************************************************************************/


/**
 *  功能：发送状态设备的查询数据包 0x1001 - 0x1801
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendStatusInfo(u8 sn_code);

/**
 *  功能：发送设置通道参数的查询数据包  0x1002-0x1802
 *  参数：flag 成功1 失败0  
 *        sn_code 序号码
 *  返回：无
*/
void sendChannel(u8 flag,u8 sn_code);
 
/**
 *  功能：发送DSP的总音量数据包 0x1003-0x1803
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendTotalVolume(u8 sn_code);


/**
 *  功能：发送设置重置均衡数据包 0x1004-0x1804
 *  参数：type 重置类型
 *        ch 通道 
 *        sn_code 序号码
 *  返回：无
*/
void sendResetBalance(u8 type,u8 ch,u8 sn_code);
 
/**
 *  功能：发送恢复均衡数据包  0x1005-0x1805
 *  参数：type 恢复类型
 *        sn_code 序号码
 *  返回：无
*/
void sendEqualizerRec(u8 type,u8 sn_code);


/**
 *  功能：发送静音控制数据包 0x1006 - 0x1806
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendMute(u8 sn_code);


/**
 *  功能：发送设置系统的预置音效数据包 0x1007 - 0x1807
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendPreSoundEffe(u8 sn_code);


/**
 *  功能：发送系统复位返回包 0x1008 - 0x1808
 *  参数：reset  系统复位 
 *       sn_code 序号码
 *  返回：无
*/
void sendSystemReset(u8 reset,u8 sn_code);



/**
 *  功能：发送参量均衡控制数据包 0x1009 - 0x1809
 *  参数：type 重置类型
 *        ch 通道 
 *        sn_code 序号码
 *  返回：无
*/
void sendEqualizerCtr(u8 type,u8 ch,u8 sn_code);



/**
 *  功能：发送图示均衡数据包 0x100A - 0x180A
 *  参数：type 重置类型
 *        ch 通道 
 *        sn_code 序号码
 *  返回：无
*/
void sendGraphicEqualizer(u8 type,u8 ch,u8 sn_code);


/**
 *  功能：发送联调控制数据包 0x100B - 0x180B
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendJointCtr(u8 sn_code);

/**
 *  功能：发送延时控制数据包 0x100C - 0x180C
 *  参数：ch 通道
 *        sn_code 序号码
 *        unit 延迟单位
 *        value 延时数据
 *  返回：无
*/
void sendDelayCtr(u8 unit,u16 value,u8 ch, u8 sn_code);

/**
 *  功能：发送预留1 0x100D - 0x180D
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendReserve1(u8 sn_code);


/**
 *  功能：发送预留2 0x100E - 0x180E
 *  参数：sn_code 序号码
 *  返回：无
*/
void sendReserve2(u8 sn_code);


/**
 *  功能：发送预留3 0x100F - 0x180F
 *  参数： sn_code 序号码
 *  返回：无
*/
void sendReserve3(u8 sn_code);



/*初始化上位机*/

/**
 *  功能：初始化上位机
 *  参数：无
 *  返回：无
*/
void initApp(void);

 

#endif


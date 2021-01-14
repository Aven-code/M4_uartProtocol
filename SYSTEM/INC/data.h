/**
 *   
 *   date： 2020/7/20
 *   another: aven
 *   time: 17:04
 *   备注： 声明全局结构体（音频属性）和相关函数
 * 
 **/
#ifndef _DATA_H
#define _DATA_H

#include "global.h"
/*
//混音数组组成
     h1~h6： 高电平1~高电平6
     l1~l6:  低电平1~低电平6
     digl/digr : 数字左/数字右
     bluel/bluer: 蓝牙左/蓝牙右
*/
enum mix_sequence {h1=0,h2,h3,h4,h5,h6,
l1,l2,l3,l4,l5,l6,
digl,digr,bluel,bluer};


//定义一个 默认的EQ 方便，重置当前的EQ值

/*
    EQ 类型
*/
typedef struct 
{
    u8 agin;      //增益，分贝DB
    u16 valueQ;     //Q值
    u16 Center_fre;//频率
 
}eq_stru;


/*
    高通/低通-结构体
*/
typedef struct 
{
   u8 type;//滤波器类型
   u16 freque;//转折频率
   u8 Order;//阶数 0-8阶
}Hlpass_Stru;


/*
    界面上的通道，这里要看一下 通道上高通低通是不是数据分离，参考传输数据
*/
typedef struct 
{
    Hlpass_Stru hign;       //高通
    Hlpass_Stru low;        //低通
    u8 passMute;//通道静音，01-静音 02-不静音
    /*通道相位*/
    u8 Phase;   //（+/-） 通道相位
    u8 value;   //数值
}Pass_Stru;



/*
    通道，每个通道有 对应的EQ、延时、高低通、音量和混音等
*/
typedef struct 
{
    u16 ms;   //毫秒
    u16 cm;     //厘米
    u16 inch; //英寸

    Pass_Stru passA;  //界面上的通道基本配置
    u8 mix_all[16];  //混音
    eq_stru eq[31]; // EQ 
}Ch_Stru;



/*
    当前DSP音控的数据结构体
*/
typedef struct 
{
    u8 volume;       //总音量
    u8 Mute;        //整机静音,01 关闭静音 02 静音
    Ch_Stru CH[12]; //共有十二个通道
}Stru_Data;

//预置音效:定义6个函数指针数组，方便0X1007协议包的序号8用下标访问


/**
 *  功能：初始化12个通道
 *  参数：Stru_Data 类型指针
 *  返回：无
*/
void StruInit(Stru_Data * all);


/**
 *  功能：打印Stru_Data数据信息
 *  参数：Stru_Data 类型指针
 *  返回：无
*/
void printData(const Stru_Data * all);

 
#endif


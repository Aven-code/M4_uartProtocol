/**
 *   
 *   date： 2020/7/20
 *   another: aven
 *   time: 17:02
 *   备注： 对全局结构体变量的相关函数操作
 * 
 **/
#include "data.h"
#include <stdio.h>


/**
 *  功能：初始化12个通道
 *  参数：Stru_Data 类型指针
 *  返回：无
 *  备注：目前初始化的 一致认为出厂时的设定值
*/
void StruInit(Stru_Data * all)
{
    u8 i,j;
    all->volume = 0x35;     //设置总音量50%
    all->Mute = 0x01;
    //初始化12个通道
    for(i=0;i<12;i++)
    {
        //延迟
        all->CH[i].ms = 0;
        all->CH[i].cm = 0;
        all->CH[i].inch = 0;
        
        //通道界面高通
        all->CH[i].passA.hign.type = 0x00;         //i通道界面高通过滤器：默认0 第一个
        all->CH[i].passA.hign.freque = 0x14;       //高通转折频率: 20Hz
        all->CH[i].passA.hign.Order = 0x04;         //高通阶数:24db/Oct
        
        //通道界面低通
        all->CH[i].passA.low.type = 0x00;           //i通道界面低通过滤器：默认0 第一个
        all->CH[i].passA.low.freque = (0x01<<8)|0xf4;//低通转折频率: 500Hz
        all->CH[i].passA.low.Order = 0x04;          //低通阶数:24db/Oct
       
        all->CH[i].passA.passMute = 0x02;           //通道是否静音: 不静音
        all->CH[i].passA.Phase = 0x01;             //通道相位 0x01 正相， 反相0x02
        all->CH[i].passA.value = 0x32;              //步长数值： 0-90 :默认50

        /*EQ:每个通道有31个EQ*/
        for(j=0;j<31;j++)
        {
            all->CH[i].eq[j].agin = 0x00;           //增益 默认0.0
            all->CH[i].eq[j].valueQ = 0x02;         //Q值：默认2.525 ，我向下取整，后期再进行小数化
            all->CH[i].eq[j].Center_fre = 0x32;    //频率是由机器随机生成，所以我这里字节默认50Khz 
        }

        /*混音：每个通道有16个混音*/
        for(j=0;j<16;j++)
        {
             all->CH[i].mix_all[j] = 0X7F;  // 0-255 的一半：127
        }
        
    
    }

}


/**
 *  功能：打印混音的值
 *  参数：mix u8类型数组
 *  返回：无
*/
static void printMix(const u8 * mix)
{
    printf("h1: %d\t",mix[h1]);
    printf("h2: %d\t",mix[h2]);
    printf("h3: %d\t",mix[h3]);
    printf("h4: %d\t",mix[h4]);
    printf("h5: %d\n",mix[h5]);
    printf("h6: %d\t",mix[h6]);
    printf("l1: %d\t",mix[l1]);
    printf("l2: %d\t",mix[l2]);
    printf("l3: %d\t",mix[l3]);
    printf("l4: %d\n",mix[l4]);
    printf("l5: %d\t",mix[l5]);
    printf("l6: %d\t",mix[l6]);

    printf("digl: %d\t",mix[digl]);
    printf("digr: %d\n",mix[digr]);
    printf("bluel: %d\t",mix[bluel]);
    printf("bluer: %d\t",mix[bluer]);
    
}
 
/**
 *  功能：打印Stru_Data数据信息
 *  参数：Stru_Data 类型指针
 *  返回：无
*/
void printData(const Stru_Data * all)
{ 
    
    u8 i,j; 
      
    printf("all->volume: %d\n",  all->volume);   
    printf("all->mute:%d\n",  all->Mute);
    for(i=0;i<12;i++)
    {
        printf("\nNo.%d--------------------------------Start----------------------------------------------\n",i+1);
        printf("ms: 0x%02X\t", all->CH[i].ms );
        printf("cm: 0x%02X\t", all->CH[i].cm );
        printf("inch: 0x%02X\t", all->CH[i].inch); 

				printf("\n__________hign________\n");
        printf("all->CH[i].passA.hign.type: 0x%02X\t",all->CH[i].passA.hign.type);
        printf("all->CH[i].passA.hign.freque: 0x%04X\t",all->CH[i].passA.hign.freque);
        printf("all->CH[i].passA.hign.Order: 0x%02X",all->CH[i].passA.hign.Order);

        printf("\n__________low________\n");
        printf("all->CH[i].passA.low.type: 0x%02X\t",all->CH[i].passA.low.type);
        printf("all->CH[i].passA.low.freque: 0x%04X\t",all->CH[i].passA.low.freque);
        printf("all->CH[i].passA.low.Order: 0x%02X",all->CH[i].passA.low.Order);

        printf("\n__________other________\n");
        printf("all->CH[i].passA.passMute: %c\t",(all->CH[i].passA.passMute == 0x01)?'Y':'N');
        printf("all->CH[i].passA.Phase: 0x%02X\t", all->CH[i].passA.Phase);
        printf("all->CH[i].passA.value: 0x%02X", all->CH[i].passA.value);
 
     
        printf("\n__________________________EQ____________________________\n");
        
        for(j=0;j<31;j++)
        {
            printf("index: %d all->CH[i].eq[%d].agin: %d\t",j,j,all->CH[i].eq[j].agin);
            printf("all->CH[i].eq[%d].valueQ: %d\t",j,all->CH[i].eq[j].valueQ);
            printf("all->CH[i].eq[%d].Center_fre: %d\t",j,all->CH[i].eq[j].Center_fre);
        }
 /*混音：每个通道有16个混音*/
        printf("\n --Mix---------------------\n");
        printMix(all->CH[i].mix_all);
        
        printf("\nNo.%d--------------------------------End----------------------------------------------\n",i+1);

    }
    
       
 
  
}




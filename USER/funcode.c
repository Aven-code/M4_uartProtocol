
/**
  ******************************************************************************
  * @file    ./funcode.c
  * @author  Aven
  * @version V2.0
  * @date    02-August-2020
  * @brief   ��������ش���������
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
 *  ���ܣ�״̬�豸�Ĳ�ѯ 0x1001 - 0x1801
 *  �������� 
 *  ���أ���
*/
void getStatusInfo(void)
{
    //�����������Ϣ�����ݼ���
}

/**
 *  ���ܣ�����ͨ�������������趨DSP���Ű��ͨ������ 0x1002-0x1802
 *  ��������
 *  ���أ���
*/
void setChannel(void)
{
    /*
        0 ��ʾ6������
        1 ��ʾEQ ���͵Ĳ���
        2 �����ͨ�˲�������
        3 �����ͨ�˲�������
        4 ��ʱ����
        5 ��λ����
        6 ����
        7 ȫ����������
        8 ��������
        9 ͨ����������
    */
    u8 argType = receiveDataArray[8]; //ͨ���������

    u8 ch = receiveDataArray[7]; //��ȡͨ��
    ch--;                        //���ʹ�0x01��ʼ

    /*�ж���ʲô���͵�*/
    if (argType == 0x00)
    {
        argType = 0;
    }
    else if (argType >= 1 && argType <= 31) //EQ����
    {
        argType = 1;

        allConfig.CH[ch].eq->agin = receiveDataArray[9];                                      //����,����һ��С��
        allConfig.CH[ch].eq->Center_fre = (receiveDataArray[11] << 8) | receiveDataArray[10]; //����Ƶ��
        allConfig.CH[ch].eq->valueQ = (receiveDataArray[13] << 8) | receiveDataArray[12];     //Qֵ
    }
    else if (argType == 32) //��ͨ�˲�������
    {
        argType = 2;
        allConfig.CH[ch].passA.low.type = receiveDataArray[9];                                  //�˲�������
        allConfig.CH[ch].passA.low.freque = (receiveDataArray[11] << 8) | receiveDataArray[10]; //ת��Ƶ��
        allConfig.CH[ch].passA.low.Order = receiveDataArray[12];                                //����
    }
    else if (argType == 33) //��ͨ�˲�������
    {
        argType = 3;
        allConfig.CH[ch].passA.hign.type = receiveDataArray[9];                                  //�˲�������
        allConfig.CH[ch].passA.hign.freque = (receiveDataArray[11] << 8) | receiveDataArray[10]; //ת��Ƶ��
        allConfig.CH[ch].passA.hign.Order = receiveDataArray[12];                                //����
    }
    else if (argType == 34) //��ʱ����
    {
        argType = 4;
        //û�ҵ���ص��ź�
    }
    else if (argType == 35) //ͨ����λ
    {
        argType = 5;
        allConfig.CH[ch].passA.Phase = receiveDataArray[9];
        allConfig.CH[ch].passA.value = receiveDataArray[10]; //��λ��ֵ������С��
    }
    else if (argType == 36) //����
    {
        argType = 6;
        allConfig.CH[ch].passA.passMute = receiveDataArray[9]; //�������
    }
    else if (argType == 37) //ȫ����������
    {
        argType = 6;
        //��������ģ����Ӧָ����
    }
    else if (argType == 38) //��������
    {
        u8 i,j = 9;
        argType = 6;
        for (i = 0; i < 16; i++, j++)
        {
            allConfig.CH[ch].mix_all[i] = receiveDataArray[j];
        }
        //���潲16�ֽڵĻ�������ȥ
    }
    else if (argType == 39) //ͨ����������
    {
        argType = 6;
        //ģ������
    }

    //�ظ�Э���,Ĭ�ϳɹ�
    sendChannel(1, receiveDataArray[4]);
}

/**
 *  ���ܣ�����DSP�������� 0x1003-0x1803
 *  ��������
 *  ���أ���
*/
void setTotalVolume(void)
{
    //��������
    allConfig.volume = receiveDataArray[7]; //����0-255
    if(allConfig.volume == 0x00)
        allConfig.Mute = 0x02;  //������Ϊ0ʱ������
    sendTotalVolume(receiveDataArray[4]);
}

/**
 *  ���ܣ��������þ��� 0x1004-0x1804
 *  ��������
 *  ���أ���
 *  ��ע�� ���þ��⹦��ֻ�Ե�ǰ��ͨ������ 
*/
void setResetBalance(void)
{
    //eq_stru default_eq = {0x00, 0x02, 0x32}; //Ĭ��ֵ

    //ֻ���õ�ǰ�ľ�����
    u8 j, ch = receiveDataArray[7]; //ͨ��
    ch--;
    if (receiveDataArray[8] == 0x01) //���þ��⵽�������趨ֵ
    {
        for (j = 0; j < 31; j++)
        {
            allConfig.CH[ch].eq[j].agin = 0x00;       //���� Ĭ��0.0
            allConfig.CH[ch].eq[j].valueQ = 0x02;     //Qֵ��Ĭ��2.525 ��������ȡ���������ٽ���С����
            allConfig.CH[ch].eq[j].Center_fre = 0x32; //Ƶ�����ɻ���������ɣ������������ֽ�Ĭ��50Khz
        }

        sendResetBalance(0x01, receiveDataArray[7], receiveDataArray[4]);
    }
    else if (receiveDataArray[8] == 0x02) //�ָ��������� Ԥ����Ч�Ķ�Ӧֵ
    {
        //����Ԥ����Ч����
        sendResetBalance(0x02,  receiveDataArray[7], receiveDataArray[4]);
    }
}

/**
 *  ���ܣ��ָ����� 0x1005-0x1805
 *  ��������
 *  ���أ���
*/
void setEqualizerRec(void)
{

    if (receiveDataArray[7] == 0x01) //���Ҫ�ָ���һ�Σ�ֱ�ӵ���allconfig
    {
        //����allconfig,��allconfig ��Ϊ���һ���޸ĵ�����
    }
    else if (receiveDataArray[7] == 0x02) //�ָ����о�������Ԥ�ò����Ͳ�Ҫ�޸�allconfig
    {
        //���ú���ʱ������Ĭ�Ϲ̶����ݼ��ɡ�
    }
    //�ظ���
    sendEqualizerRec(receiveDataArray[7], receiveDataArray[4]);
}

/**
 *  ���ܣ��������� 0x1006 - 0x1806
 *  ��������
 *  ���أ���
*/
void setMute(void)
{
    allConfig.Mute = receiveDataArray[7];
    sendMute(receiveDataArray[4]);
}

/**
 *  ���ܣ�����ϵͳ��Ԥ����Ч 0x1007 - 0x1807
 *  ��������
 *  ���أ���
*/
void setPreSoundEffe(void)
{
}

/**
 *  ���ܣ�ϵͳ��λ 0x1008 - 0x1808
 *  ��������
 *  ���أ���
*/
void systemReset(void)
{
    //�ж��Ƿ�λ
    if (receiveDataArray[7] == 0x01)
    {
        /*ϵͳ��λ*/
        //������Լ�һ���жϣ��жϸ�λ�Ƿ�ɹ�
        sendSystemReset(0x01, receiveDataArray[4]);
    }
    else if (receiveDataArray[7] == 0x02)
    {
        sendSystemReset(0x02, receiveDataArray[4]);
    }
}

/**
 *  ���ܣ������������ 0x1009 - 0x1809
 *  ��������
 *  ���أ���
*/
void setEqualizerCtr(void)
{

    sendEqualizerCtr(receiveDataArray[8], receiveDataArray[7], receiveDataArray[4]);
}

/**
 *  ���ܣ�ͼʾ���� 0x100A - 0x180A
 *  ��������
 *  ���أ���
*/
void setGraphicEqualizer(void)
{

    sendGraphicEqualizer(receiveDataArray[8], receiveDataArray[7], receiveDataArray[4]);
}

/**
 *  ���ܣ��������� 0x100B - 0x180B
 *  ��������
 *  ���أ���
*/
void setJointCtr(void)
{
    //�ж������Ƿ�ر�
    if (receiveDataArray[7])
    {
        //�ر�
    }

    sendJointCtr(receiveDataArray[4]);
}

/**
 *  ���ܣ���ʱ���� 0x100C - 0x180C
 *  ��������
 *  ���أ���
*/
void setDelayCtr(void)
{
    u8 ch = receiveDataArray[7]; //ͨ��
    ch--;
    /*ע�⣺������� ���� Ӣ�� �κ�һ����ֵ�����仯����������Ҫ�����仯*/
    if (receiveDataArray[10] == 0) //����
    {
        allConfig.CH[ch].ms = (receiveDataArray[9] << 8) | receiveDataArray[8];
        sendDelayCtr(0x00,allConfig.CH[ch].ms,receiveDataArray[7], receiveDataArray[4]);
    }
    else if (receiveDataArray[10] == 1) //����
    {
        allConfig.CH[ch].cm = (receiveDataArray[9] << 8) | receiveDataArray[8];
        sendDelayCtr(0x01,allConfig.CH[ch].cm,receiveDataArray[7], receiveDataArray[4]);
    }
    else if (receiveDataArray[10] == 2) //Ӣ��
    {
        allConfig.CH[ch].inch = (receiveDataArray[9] << 8) | receiveDataArray[8];
        sendDelayCtr(0x02,allConfig.CH[ch].inch,receiveDataArray[7], receiveDataArray[4]);
    }
  
}

/**
 *  ���ܣ�Ԥ��1 0x100D - 0x180D
 *  ��������
 *  ���أ���
*/
void reserve1(void)
{
}

/**
 *  ���ܣ�Ԥ��2 0x100E - 0x180E
 *  ��������
 *  ���أ���
*/
void reserve2(void)
{
}

/**
 *  ���ܣ�Ԥ��3 0x100F - 0x180F
 *  ��������
 *  ���أ���
*/
void reserve3(void)
{
}

/**
 *  ���ܣ��ϱ�����쳣״̬��Ϣ��������0x2000
 *  ��������
 *  ���أ���
*/
void sendErrorStatusInfo(void)
{
}

/*
 *  ���ܣ������豸��Ϣ 0x5001-0x5801
 *  ��������
 *  ���أ���
*/
void sendEquipmentInfo(void)
{
    u16 i;

    //��Ϣ����(ģ�⴮��)
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
 *  ���ܣ�������� 0x5007- 0x5807
 *  ��������
 *  ���أ���
*/
void BluetoothMatch(void)
{
    //5007 Ӧ����û����û����Ϣ��ģ����Բ���msg_len�ò���
    //�������ú������ڽ���ǰ�ײ��ֵ��������
    u8 i;
    //�����̶��������ֽ�
    u8 info[12] = {0xee, 0xee, 0x00, 0x0C, 0x00, 0x58, 0x07, 0x51, 0x51, 0x44, 0x4d, 0x00};
    info[4] = receiveDataArray[4];               //����������SN���к�
    info[11] = generateCheckCode(info, info[3]); //У����
    for (i = 0; i < 12; i++)
    {
			usart1_putchar(info[i]); 
    }

    //��Գɹ����Ͷ�ӦЭ���֮���ʼ����λ��
    initApp();
}

/*��ط��ذ����� ******************************************************************************************************************/
//���еķ��ذ�����MCU�ϵ����������APPʱ����һ��(����ϵͳ��λ)
/********************************************************************************************************************************/

/**
 *  ���ܣ�����״̬�豸�Ĳ�ѯ���ݰ� 0x1001 - 0x1801
 *  ������sn_code �����
 *  ���أ���
*/
void sendStatusInfo(u8 sn_code)
{
}

/**
 *  ���ܣ���������ͨ�������Ĳ�ѯ���ݰ�  0x1002-0x1802
 *  ������falg �ɹ�1 ʧ��0 
 *        sn_code �����
 *  ���أ���
*/
void sendChannel(u8 flag, u8 sn_code)
{
    //���� receiveDataArray  ���¹���һ������
    u8 i, reply[11] = {0xee, 0xee, 0x00, 0x0B, 0x00, 0x18, 0x02, 0x00, 0x00, 0x00, 0x00};
		reply[4] = sn_code;
    reply[7] = receiveDataArray[7];           //�ڼ�ͨ��
    reply[8] = receiveDataArray[8];           //�������
		reply[9] = flag;
    reply[10] = generateCheckCode(reply, 10); //У����

    //����
    for (i = 0; i < 11; i++)
    {
			usart1_putchar(reply[i]); 
    }
}

/**
 *  ���ܣ�����DSP�����������ݰ� 0x1003-0x1803
 *  ������sn_code �����
 *  ���أ���
*/
void sendTotalVolume(u8 sn_code)
{
    u8 i, reply[9] = {0xee, 0xee, 0x00, 0x09, 0x00, 0x18, 0x03, 0x00, 0x00};
		reply[4] = sn_code; 
    reply[7] = allConfig.volume;            //�趨�ɹ�֮�������
    reply[8] = generateCheckCode(reply, 8); //У����

    for (i = 0; i < 9; i++)
    {
			usart1_putchar(reply[i]);	
    }
}

/**
 *  ���ܣ������������þ������ݰ� 0x1004-0x1804
 *  ������type ��������
 *        ch ͨ�� 
 *        sn_code �����
 *  ���أ���
*/
void sendResetBalance(u8 type, u8 ch, u8 sn_code)
{
    u8 i, reply[10] = {0xee, 0xee, 0x00, 0x0A, 0x00, 0x18, 0x04, 0x00, 0x00, 0x00};
		reply[4] = sn_code; 
		reply[7] = ch;
		reply[8] = type; 		
    reply[9] = generateCheckCode(reply, 9); //У����

    for (i = 0; i < 10; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  ���ܣ����ͻָ��������ݰ�  0x1005-0x1805
 *  ������type �ָ�����
 *        sn_code �����
 *  ���أ���
*/
void sendEqualizerRec(u8 type, u8 sn_code)
{
    u8 i, reply[9] = {0xee, 0xee, 0x00, 0x09, 0x00, 0x18, 0x05, 0x00, 0x00};
		reply[4] = sn_code;   
		reply[7] = type; 
		reply[8] = generateCheckCode(reply, 8); //У����

    for (i = 0; i < 9; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  ���ܣ����;����������ݰ� 0x1006 - 0x1806
 *  ������sn_code �����
 *  ���أ���
*/
void sendMute(u8 sn_code)
{
    u8 i, reply[9] = {0xee, 0xee, 0x00, 0x09, 0x00, 0x18, 0x06, 0x00, 0x00};
		reply[4] = sn_code; 
    reply[7] = allConfig.Mute;
    reply[8] = generateCheckCode(reply, 8); //У����

    for (i = 0; i < 9; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  ���ܣ���������ϵͳ��Ԥ����Ч���ݰ� 0x1007 - 0x1807
 *  ������sn_code �����
 *  ���أ���
*/
void sendPreSoundEffe(u8 sn_code)
{
}

/**
 *  ���ܣ�����ϵͳ��λ���ذ� 0x1008 - 0x1808
 *  ������reset  ϵͳ��λ 
 *       sn_code �����
 *  ���أ���
*/
void sendSystemReset(u8 reset, u8 sn_code)
{
    u8 i, reply[9] = {0xee, 0xee, 0x00, 0x09, 0x00, 0x18, 0x08, 0x00, 0x00};
		reply[4] = sn_code; 
		reply[7] = reset;
    reply[8] = generateCheckCode(reply, 8); //У����

    for (i = 0; i < 9; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  ���ܣ����Ͳ�������������ݰ� 0x1009 - 0x1809
 *  ������type ��������
 *        ch ͨ�� 
 *        sn_code �����
 *  ���أ���
*/
void sendEqualizerCtr(u8 type, u8 ch, u8 sn_code)
{
    u8 i, reply[10] = {0xee, 0xee, 0x00, 0x0A, 0x00, 0x18, 0x09, 0x00, 0x00, 0x00};
		reply[4] = sn_code; 
		reply[7] = ch; 
		reply[8] = type; 
    reply[9] = generateCheckCode(reply, 9); //У����
    for (i = 0; i < 10; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  ���ܣ�����ͼʾ�������ݰ� 0x100A - 0x180A
 *  ������type ��������
 *        ch ͨ�� 
 *        sn_code �����
 *  ���أ���
*/
void sendGraphicEqualizer(u8 type, u8 ch, u8 sn_code)
{
    u8 i, reply[10] = {0xee, 0xee, 0x00, 0x0A, 0x00, 0x18, 0x0A, 0x00, 0x00, 0x00};
		reply[4] = sn_code; 
		reply[7] = ch; 
		reply[8] = type; 
    reply[9] = generateCheckCode(reply, 9); //У����
    for (i = 0; i < 10; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  ���ܣ����������������ݰ� 0x100B - 0x180B
 *  ������sn_code �����
 *  ���أ���
*/
void sendJointCtr(u8 sn_code)
{
    u8 i, reply[9] = {0xee, 0xee, 0x00, 0x09, 0x00, 0x18, 0x0B, 0x00, 0x00};
		reply[4] = sn_code; 
    reply[7] = receiveDataArray[7];
    reply[8] = generateCheckCode(reply, 8); //У����

    for (i = 0; i < 9; i++)
    {
         usart1_putchar(reply[i]);	
    }
}

/**
 *  ���ܣ�������ʱ�������ݰ� 0x100C - 0x180C
 *  ������ch ͨ��
 *        sn_code �����
 *        unit �ӳٵ�λ
 *        value ��ʱ����
 *  ���أ���
*/
void sendDelayCtr(u8 unit,u16 value,u8 ch, u8 sn_code)
{
    u8 i, reply[12] = {0xee, 0xee, 0x00, 0x0C, 0x00, 0x18, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00};
		reply[4] = sn_code;   
		reply[7] = ch; 
		reply[8] = value & 0xff;   //��λ
    reply[9] = (value>>8) & 0xff; //��λ
		reply[10] = unit; 
    reply[11] = generateCheckCode(reply, 11); //У����

    for (i = 0; i < 12; i++)
    {
        usart1_putchar(reply[i]);	
    }
}

/**
 *  ���ܣ�����Ԥ��1 0x100D - 0x180D
 *  ������sn_code �����
 *  ���أ���
 *  ��ע������ͨ�������Զ���һ����������ͨ�������в��������͹�ȥ������1002 ��Э��ָ���
*/
void sendReserve1(u8 sn_code)
{
#if 0 
		u8 ch;   //ͨ����1��ʼ
    u8 para = 0; //����
    u8 i, reply[11] = {0xEE, 0xEE, 0x00, 0x0B, 0x00, 0x18, 0x02, 0x00, 0x00, 0x01, 0x00};
		reply[4] = sn_code;   
    for (ch = 1; ch < 13; ch++)
    {
				
    }
		
#endif
}

/**
 *  ���ܣ�����Ԥ��2 0x100E - 0x180E
 *  ������sn_code �����
 *  ���أ���
*/
void sendReserve2(u8 sn_code)
{
}

/**
 *  ���ܣ�����Ԥ��3 0x100F - 0x180F
 *  ������sn_code �����
 *  ���أ���
*/
void sendReserve3(u8 sn_code)
{
}

/**
 *  ���ܣ���ʼ����λ��
 *  ��������
 *  ���أ���
 *  ��ע����MCU�ϵ����������APPʱ����һ�����ݰ����Ա��ʼ������,SN ͳͳΪ0x00
*/
void initApp(void)
{

    u8 i;
    // sendStatusInfo(0);          //����״̬�豸�Ĳ�ѯ���ݰ� -0x1801
    //����ͨ�������Զ���һ����������ͨ�������в��������͹�ȥ������1002 ��Э��ָ���

    sendTotalVolume(0); //����DSP�����������ݰ� -0x1803
                        // sendResetBalance(0);      //�����������þ������ݰ� -0x1804
                        // sendEqualizerRec(0);      //���ͻָ��������ݰ�  -0x1805
    sendMute(0);        //���;����������ݰ� -0x1806
    //sendPreSoundEffe(0);      //��������ϵͳ��Ԥ����Ч���ݰ� -0x1807
    //sendEqualizerCtr(0);      //���Ͳ�������������ݰ� -0x1809
    //sendGraphicEqualizer(0);  //����ͼʾ�������ݰ� -0x180A
    //sendJointCtr(0);          //���������������ݰ� -0x180B

    //����ÿһ��ͨ������ʱ����
    for (i = 1; i < 13; i++)
        sendDelayCtr(0,allConfig.CH[i].ms,i, 0); //������ʱ�������ݰ� -0x180C
}



 /**
  ******************************************************************************
  * @file    ./funcode.h 
  * @author  Aven
  * @version V1.0
  * @date    04-June-2020
  * @brief   ��������ش���������
  *   ******************************************************************************
  */
#ifndef _FUNCODE_H
#define _FUNCODE_H

#define CHANNEL_AGR_TYPE_MAX 40    //ͨ���ڲ������ĸ��� ָ���룺0x1002

#include "global.h"



/*������ṹ������*/
typedef struct 
{
    u16 funcode;                           //������
    void (*fun_ptr)(void);          //����������Ӧ�ĺ���ָ��,�β�����Ϣ�峤��
}Func_Code;


/**
 *  ���ܣ�״̬�豸�Ĳ�ѯ 0x1001 - 0x1801
 *  ��������
 *  ���أ���
*/
void getStatusInfo(void);


/**
 *  ���ܣ�����ͨ�������������趨DSP���Ű��ͨ������ 0x1002-0x1802
 *  ��������
 *  ���أ���
*/
void setChannel(void);

 

 
/**
 *  ���ܣ�����DSP�������� 0x1003-0x1803
 *  ��������
 *  ���أ���
*/
void setTotalVolume(void);

/**
 *  ���ܣ��������þ��� 0x1004
 *  ��������
 *  ���أ���
*/
void  setResetBalance(void);
 
/**
 *  ���ܣ��ָ����� 0x1005-0x1805
 *  ��������
 *  ���أ���
*/
void setEqualizerRec(void);

/**
 *  ���ܣ��������� 0x1006 - 0x1806
 *  ��������
 *  ���أ���
*/
void setMute(void);



/**
 *  ���ܣ�����ϵͳ��Ԥ����Ч 0x1007 - 0x1807
 *  ��������
 *  ���أ���
*/
void setPreSoundEffe(void);


/**
 *  ���ܣ�ϵͳ��λ 0x1008 - 0x1808
 *  ��������
 *  ���أ���
*/
void systemReset(void);


/**
 *  ���ܣ������������ 0x1009 - 0x1809
 *  ��������
 *  ���أ���
*/
void setEqualizerCtr(void);


/**
 *  ���ܣ�ͼʾ���� 0x100A - 0x180A
 *  ��������
 *  ���أ���
*/
void setGraphicEqualizer(void);

/**
 *  ���ܣ��������� 0x100B - 0x180B
 *  ��������
 *  ���أ���
*/
void setJointCtr(void);

/**
 *  ���ܣ���ʱ���� 0x100C - 0x180C
 *  ��������
 *  ���أ���
*/
void setDelayCtr(void);

/**
 *  ���ܣ�Ԥ��1 0x100D - 0x180D
 *  ��������
 *  ���أ���
*/
void  reserve1(void);


/**
 *  ���ܣ�Ԥ��2 0x100E - 0x180E
 *  ��������
 *  ���أ���
*/
void  reserve2(void);


/**
 *  ���ܣ�Ԥ��3 0x100F - 0x180F
 *  ��������
 *  ���أ���
*/
void  reserve3(void);


/**
 *  ���ܣ��ϱ�����쳣״̬��Ϣ��������0x2000
 *  ��������
 *  ���أ���
*/
void sendErrorStatusInfo(void);

/*
 *  ���ܣ������豸��Ϣ 0x5001-0x5801
 *  ��������
 *  ���أ���
*/
void sendEquipmentInfo (void);

/**
 *  ���ܣ�������� 0x5007- 0x5807
 *  ��������
 *  ���أ���
*/
void BluetoothMatch(void);
 



/*��ط��ذ����� *************************************************************************************/
//���еķ��ذ�����MCU�ϵ����������APPʱ����һ��(����ϵͳ��λ)
/*****************************************************************************************************/


/**
 *  ���ܣ�����״̬�豸�Ĳ�ѯ���ݰ� 0x1001 - 0x1801
 *  ������sn_code �����
 *  ���أ���
*/
void sendStatusInfo(u8 sn_code);

/**
 *  ���ܣ���������ͨ�������Ĳ�ѯ���ݰ�  0x1002-0x1802
 *  ������flag �ɹ�1 ʧ��0  
 *        sn_code �����
 *  ���أ���
*/
void sendChannel(u8 flag,u8 sn_code);
 
/**
 *  ���ܣ�����DSP�����������ݰ� 0x1003-0x1803
 *  ������sn_code �����
 *  ���أ���
*/
void sendTotalVolume(u8 sn_code);


/**
 *  ���ܣ������������þ������ݰ� 0x1004-0x1804
 *  ������type ��������
 *        ch ͨ�� 
 *        sn_code �����
 *  ���أ���
*/
void sendResetBalance(u8 type,u8 ch,u8 sn_code);
 
/**
 *  ���ܣ����ͻָ��������ݰ�  0x1005-0x1805
 *  ������type �ָ�����
 *        sn_code �����
 *  ���أ���
*/
void sendEqualizerRec(u8 type,u8 sn_code);


/**
 *  ���ܣ����;����������ݰ� 0x1006 - 0x1806
 *  ������sn_code �����
 *  ���أ���
*/
void sendMute(u8 sn_code);


/**
 *  ���ܣ���������ϵͳ��Ԥ����Ч���ݰ� 0x1007 - 0x1807
 *  ������sn_code �����
 *  ���أ���
*/
void sendPreSoundEffe(u8 sn_code);


/**
 *  ���ܣ�����ϵͳ��λ���ذ� 0x1008 - 0x1808
 *  ������reset  ϵͳ��λ 
 *       sn_code �����
 *  ���أ���
*/
void sendSystemReset(u8 reset,u8 sn_code);



/**
 *  ���ܣ����Ͳ�������������ݰ� 0x1009 - 0x1809
 *  ������type ��������
 *        ch ͨ�� 
 *        sn_code �����
 *  ���أ���
*/
void sendEqualizerCtr(u8 type,u8 ch,u8 sn_code);



/**
 *  ���ܣ�����ͼʾ�������ݰ� 0x100A - 0x180A
 *  ������type ��������
 *        ch ͨ�� 
 *        sn_code �����
 *  ���أ���
*/
void sendGraphicEqualizer(u8 type,u8 ch,u8 sn_code);


/**
 *  ���ܣ����������������ݰ� 0x100B - 0x180B
 *  ������sn_code �����
 *  ���أ���
*/
void sendJointCtr(u8 sn_code);

/**
 *  ���ܣ�������ʱ�������ݰ� 0x100C - 0x180C
 *  ������ch ͨ��
 *        sn_code �����
 *        unit �ӳٵ�λ
 *        value ��ʱ����
 *  ���أ���
*/
void sendDelayCtr(u8 unit,u16 value,u8 ch, u8 sn_code);

/**
 *  ���ܣ�����Ԥ��1 0x100D - 0x180D
 *  ������sn_code �����
 *  ���أ���
*/
void sendReserve1(u8 sn_code);


/**
 *  ���ܣ�����Ԥ��2 0x100E - 0x180E
 *  ������sn_code �����
 *  ���أ���
*/
void sendReserve2(u8 sn_code);


/**
 *  ���ܣ�����Ԥ��3 0x100F - 0x180F
 *  ������ sn_code �����
 *  ���أ���
*/
void sendReserve3(u8 sn_code);



/*��ʼ����λ��*/

/**
 *  ���ܣ���ʼ����λ��
 *  ��������
 *  ���أ���
*/
void initApp(void);

 

#endif


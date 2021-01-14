
#include "stm32f4xx.h"
#include "iwdg.h"

// ע�⣬��ͨ���Թؼ��ּĴ��� (IWDG_KR) д��ֵ 0xCCCC �����������Ź�ʱ����������ʼ�Ӹ�λ
// ֵ 0xFFF �ݼ�������
// �κ�ʱ�򽫹ؼ��� 0xAAAA д�� IWWDG_KR �Ĵ����У� IWDG_RLR ��ֵ�ͻᱻ���ص����������Ӷ�
// ����������Ź���λ��
// ��ˣ���ʼ�����Ź��������ι�������Ǵ� 0x0FFF ��ʼ�ݼ�������ֻ��ι���˲�ʹ�����õ�
// ���ؼ���ֵ�ݼ�����

/**
 * ���ܣ�  ��ʼ���������Ź�
 * ������  IWDG_Prescaler Ԥ��Ƶֵ���� LSI �Ļ����Ͻ��з�Ƶ����ȡֵ��
 *             IWDG_Prescaler_4
 *             IWDG_Prescaler_8
 *             IWDG_Prescaler_16
 *             IWDG_Prescaler_32
 *             IWDG_Prescaler_64
 *             IWDG_Prescaler_128
 *             IWDG_Prescaler_256
 *         Reload         ��װ��ֵ��ȡֵ��ΧΪ 0x0000 - 0x0FFF
 * ����ֵ����
 **/
void iwdg_init(uint8_t IWDG_Prescaler, uint16_t Reload)
{
	// ȡ���� IWDG_PR �� IWDG_RLR �Ĵ�����д����
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	// ����Ԥ��Ƶֵ
	IWDG_SetPrescaler(IWDG_Prescaler);
	
	// ������װ��ֵ
	IWDG_SetReload(Reload);
	
	// ʹ�ܿ��Ź�
	IWDG_Enable();
	
	// ʹ�ܶ� IWDG_PR �� IWDG_RLR �Ĵ�����д����
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
	
	// ι��һ�Σ�֮��Ż�ʹ�����ؼ���ֵ��ʼ�ݼ�����
	IWDG_FEED();
}



/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
��ƽ    ̨�������������ܿƼ�TC264DA���İ�
����    д��ZYF/chiusir
��E-mail  ��chiusir@163.com
�������汾��V1.1 ��Ȩ���У���λʹ��������ϵ��Ȩ
�������¡�2020��10��28��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://longqiu.taobao.com
------------------------------------------------
��dev.env.��AURIX Development Studio1.2.2�����ϰ汾
��Target �� TC264DA/TC264D
��Crystal�� 20.000Mhz
��SYS PLL�� 200MHz
________________________________________________________________
����iLLD_1_0_1_11_0�ײ����,

ʹ�����̵�ʱ�򣬽������û�пո��Ӣ��·����
����CIFΪTC264DA�����⣬�����Ĵ������TC264D
����Ĭ�ϳ�ʼ����EMEM��512K������û�ʹ��TC264D��ע�͵�EMEM_InitConfig()��ʼ��������
������\Libraries\iLLD\TC26B\Tricore\Cpu\CStart\IfxCpu_CStart0.c��164�����ҡ�
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ

������8��ת��������ͬ��ת����ÿ��ת��8��ͨ��������Ϊ8/10/12λ��Ĭ��Ϊ��߾���12λ��
ADC�˿�AN������Ϊ����ڣ�������Ϊ����ڣ���ARM��Ƭ����ͬ��
QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/
#ifndef _LQ_ADC_H_
#define _LQ_ADC_H_

#include "stdint.h"
#include "IfxVadc_reg.h"
#include "IfxVadc_regdef.h"
#include "Ifx_TypesReg.h"
#include "Vadc/Std/IfxVadc.h"
#include "Vadc/Adc/IfxVadc_Adc.h"
#include "Cpu\Std\Platform_Types.h"
extern short left_X,left_V,right_X, right_V ;
extern float ALL_AD;
extern uint8_t time,K;

/*
 * adc ͨ�� ö��   ADC0��Ӧ�ܽ� AN0
 */
typedef enum
{
	ADC0 = 0x0000, ADC1 = 0x0101, ADC2 = 0x0202, ADC3 = 0x0303, ADC4 = 0x0404, ADC5 = 0x0505, ADC6 = 0x0606, ADC7 = 0x0707,
	ADC8 = 0x0808, ADC10= 0x0A09, ADC11= 0x0B0A, ADC12= 0x0C0B, ADC13= 0x0D0C,

	ADC16= 0x100D, ADC17= 0x110E, ADC20= 0x140F, ADC25= 0x1910,

	ADC35= 0x2311, ADC36= 0x2412, ADC37= 0x2513, ADC38= 0x2614, ADC39= 0x2715, ADC44= 0x2A16, ADC45= 0x2B17, ADC46= 0x2C18,
	ADC47= 0x2D19, ADC48= 0x2E1A, ADC49= 0x2F1B
}ADC_Channel_t;

/* ��ȡADC��Ӧ�� */
#define ADC_GetGroup(ADC_Channel)   ((ADC_Channel & 0xF000)>>12)
/* ��ȡADC��Ӧͨ�� */
#define ADC_GetChannel(ADC_Channel) ((ADC_Channel & 0x0F00)>>8)
/* ��ȡADC��Ӧ�ܽ����к� �궨�� */
#define ADC_GetIndex(ADC_Channel)   (unsigned char)(ADC_Channel & 0x00FF)
/*��ȡADC��ֵ���ҹ�һ��*/
void GetlastAdcValue(void);


void ADC_InitConfig(ADC_Channel_t channel, unsigned long Freq_Hz);
unsigned short ADC_Read(ADC_Channel_t chn);
unsigned short ADC_ReadAverage(ADC_Channel_t  chn, unsigned char count);
void ADC_Work(uint8_t *L_Flag,uint8_t *R_Flag);


#endif /* 0_APPSW_TRICORE_APP_LQ_ADC_H_ */
/*LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL
 ��ƽ    ̨�������������ܿƼ�TC264DA���İ�
 ����    д��ZYF/chiusir
 ��E-mail  ��chiusir@163.com
 �������汾��V1.1 ��Ȩ���У���λʹ��������ϵ��Ȩ
 �������¡�2020��10��28��
 �������Ϣ�ο����е�ַ��
 ����    վ��http:// www.lqist.cn
 ���Ա����̡�http:// longqiu.taobao.com
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
 ________________________________________________________________

 QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ
 *  ��    ע��TC264 ������CCU6ģ��  ÿ��ģ��������������ʱ��  ������ʱ���ж�
 QQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQQ*/

#include "LQ_CCU6.h"

#include <CompilerTasking.h>
#include <LQ_STEER_PID.h>
#include <math.h>
#include <stdlib.h>

#include "LQ_PID.h"
#include "../APP/LQ_GPIO_LED.h"
#include "LQ_GPT12_ENC.h"
#include "LQ_MotorServo.h"
#include "LQ_ImageProcess.h"
#include "LQ_ADC.h"
#include "LQ_UART.h"
#include "LQ_GPIO_BUZZ.h"
#include "LQ_MPU6050_DMP.h"
#include "LQ_IIC_Gyro.h"
#include "LQ_Atom_Motor.h"
#include "LQ_GTM.h"
#include "LQ_ADC.h"
#include "TFT_Key_control.h"
#include "LQ_GPIO.h"
#include "LQ_TFT2.h"
#include "LQ_TFT18.h"
int ROAD_MAIN_ROW=50;

volatile sint16 ECPULSEL[10];             // �ٶ�ȫ�ֱ���
volatile sint16 ECPULSER[10];
int ECPULSE_L=0;
int ECPULSE_R=0;
volatile sint16 ECPULSE1 = 0;
volatile sint16 ECPULSE2 = 0;             // �ٶ�ȫ�ֱ���
volatile int ECPULSE_open = 0;
volatile int ECPULSE_Obstacle_L = 0;
volatile int ECPULSE_Obstacle_R = 0;

volatile sint32 RAllPulse = 0;            // �ٶ�ȫ�ֱ���

volatile float Target_Speed1 ;       // Ŀ���ٶ�ȫ�ֱ���
volatile float Target_Speed2 ;       // Ŀ���ٶ�ȫ�ֱ���

volatile float Real_Speed1=0;// ʵ���ٶ�ȫ�ֱ���
volatile float Real_Speed2=0;

volatile double start_V_L=500;
volatile double start_V_R=500;

volatile sint16 Target_Duty1=0;// Ŀ��ռ�ձȱ���
volatile sint16 Target_Duty2=0;

volatile sint16 Real_Duty1=0;// ʵ��ռ�ձȱ���
volatile sint16 Real_Duty2=0;

uint8_t motor_flag = 0;                   //�����ͣ��־λ  0������   1����
float K_Pid_L=0.5;
float K_Pid_R=1;
float K_error=0.15;
float K_adjust=2000;
float V_error;

extern sint16 OUT_PWM;                    //ת��PWM��ֵ
extern uint8_t L_CircleFlag;
extern uint8_t R_CircleFlag;
extern uint8_t Barrier_Flag;
extern float g_sSteeringError;
extern float last_g_sSteeringError;
int filter[20]={0};

volatile sint16 Error_flag;
/** @brief  ʮ��·�ڱ�־λ  */
extern uint8_t Crossroad_Flag;         //ʮ��·�ڱ�־λ 0:����ʮ��·�� 2-3:��ʮ��·��

extern uint8_t leftup[2] ;
extern uint8_t rightup[2] ;
extern uint8_t leftup_1[2];
extern uint8_t rightup_1[2] ;

extern uint8_t left_point[4];
extern uint8_t right_point[4];

extern uint8_t ImageSide[LCDH][2];         //���ұ�������

extern uint8_t point_num;

extern int min_tag;

int out_PWML=0,out_PWMR=0;
float KZ_L=1.1;
extern float Anglex,Angley,Anglez;//�����ǽǶ�
extern int Feedback_SpeedL,Feedback_Speed_tmpL,Feedback_SpeedR,Feedback_Speed_tmpR;

extern short left_X,left_V,right_X, right_V;
extern float ALL_AD;

int Fan_PWML=1000;
int Fan_PWMR=1000;

extern int X_minL,X_minR,X_min;
extern int ADC_Value[4];       //��ŵĲɼ�ֵ
extern uint16 MagneticField;
extern int B_lk;

extern char txt[16];
uint8_t rectangle_flag=0;
uint8_t open_flag=0;  //��·��־λ  ��ű�־λ
uint8_t Obstacle_flag=0;//·�ϱ�־λ
uint8_t slope_flag=0;  //�µ���־λ
uint8_t Park_flag=0;  //�����־λ
uint8_t Circle_flag_L=0;//��Բ����־λ
uint8_t Circle_flag_R=0;//��Բ����־λ
uint8_t cross_flag=0; //ʮ�ֱ�־λ
int Obstacle_turn_flag=0;//·�����ҹձ�־λ
int motor_turn_flag=0;//�������ҹձ�־λ
int Speed_turn_flag=0;//�ٶ�ģʽ��־λ

int Obstacle_num=0,Circle_num=0;
IFX_INTERRUPT(CCU60_CH0_IRQHandler, CCU60_VECTABNUM, CCU60_CH0_PRIORITY);
IFX_INTERRUPT(CCU60_CH1_IRQHandler, CCU60_VECTABNUM, CCU60_CH1_PRIORITY);
IFX_INTERRUPT(CCU61_CH0_IRQHandler, CCU61_VECTABNUM, CCU61_CH0_PRIORITY);
IFX_INTERRUPT(CCU61_CH1_IRQHandler, CCU61_VECTABNUM, CCU61_CH1_PRIORITY);

/** CCU6�ж�CPU��� */
const uint8 Ccu6IrqVectabNum[2] = {CCU60_VECTABNUM, CCU61_VECTABNUM};

/** CCU6�ж����ȼ� */
const uint8 Ccu6IrqPriority[4] = {CCU60_CH0_PRIORITY, CCU60_CH1_PRIORITY, CCU61_CH0_PRIORITY, CCU61_CH1_PRIORITY};

/** CCU6�жϷ�������ַ */
const void *Ccu6IrqFuncPointer[4] = {&CCU60_CH0_IRQHandler, &CCU60_CH1_IRQHandler, &CCU61_CH0_IRQHandler,
        &CCU61_CH1_IRQHandler};

/***********************************************************************************************/
/********************************CCU6�ⲿ�ж�  ������******************************************/
/***********************************************************************************************/

/*************************************************************************
 *  �������ƣ�void CCU60_CH0_IRQHandler(void)
 *  ����˵����
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2022��11��28��
 *  ��   ע��CCU60_CH0ʹ�õ��жϷ�����
 *************************************************************************/
float Cirle_PWM=0;
float curvature=0;
int L_min_range=2750,L_max_range=5000,R_min_range=2500,R_max_range=5700;
void CCU60_CH0_IRQHandler (void)
{
    /* ����CPU�ж�  �����жϲ���Ƕ�� */
//   IfxCpu_enableInterrupts();
// ����жϱ�־
    IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_t12PeriodMatch);

    /* �û����� */
    //û���ϰ���ŷ���
   if(motor_flag==1&&abs(Anglez)<90)
   {
       if(Obstacle_num>=0&&Obstacle_num<100)
       {
           out_PWML=0;
           out_PWMR=0;
       }
       else if(Obstacle_num>=100&&Obstacle_num<150)
       {
           out_PWML=3500;
           out_PWMR=3500;
       }
       else if(motor_turn_flag==0)
       {
           out_PWML=0;
           out_PWMR=7000;
       }
       else if(motor_turn_flag==1)
       {
           out_PWML=7000;
           out_PWMR=0;
       }
   }
   else if(motor_flag==1&&abs(Anglez)>90)
   {
       Obstacle_num=0;
       Anglez=0;   //�����ǻ�����0
       motor_flag=2;
       start_V_L=450;
   }
    //����ͷԪ����Ϣ����
    //CameraCar();
    //�ж��Ƿ�������ƾ��ζ�·
    rectangle_flag=0;
    if(X_minR==X_minL&&X_min>30
    //&&ImageSide[X_min+5][0]>25&&ImageSide[X_min+5][1]<163
    &&ImageSide[X_min+1][0]>ImageSide[X_min+3][0]
    &&ImageSide[X_min+3][0]>ImageSide[X_min+5][0]
    &&ImageSide[X_min+5][0]>ImageSide[X_min+7][0]

    &&ImageSide[X_min+1][1]<ImageSide[X_min+3][1]
    &&ImageSide[X_min+3][1]<ImageSide[X_min+5][1]
    &&ImageSide[X_min+5][1]<ImageSide[X_min+7][1])//X_min>25������������߶�·
    {
        rectangle_flag=1;
    }
    if(motor_flag==2)
    {
        //���ʼ���
        lkcongzhongjiansaomiao();
        //·���ж�
        lukuangudge();
    }

    //����ж�
    judge_Park();
    if(Park_flag==0)
    {
        //Բ���ж�
        judge_Circle();
        if(Circle_flag_R!=2&&Circle_flag_R!=3&&Circle_flag_R!=4
         &&Circle_flag_L!=2&&Circle_flag_L!=3&&Circle_flag_L!=4)
        {
            //�µ��ж�
            judge_slope();
            if(slope_flag==0||slope_flag==2)
            {
                //·���ж�
                judge_Obstacle();
                if(Obstacle_flag==0||Obstacle_flag==3)
                {
                    //��·�ж�
                    judge_open();
                }
            }
        }
    }

    //����ͷ���򻷿���
   g_sSteeringError = RoadGetSteeringError(ImageSide, (unsigned char)ROAD_MAIN_ROW);//����ͷ��ȡƫ��
   if(slope_flag==1)
   {
       g_sSteeringError=0;
   }
//   if(Circle_flag_L==1)
//   {
//       g_sSteeringError=-2;
//   }
//   if(Circle_flag_R==1)
//   {
//       g_sSteeringError=2;
//   }
   if(open_flag==0&&motor_flag==2)//û���ϰ���Ž���
   {
       if(g_sSteeringError>120)g_sSteeringError=120;
       if(g_sSteeringError<-120)g_sSteeringError=-120;
//       if(abs(g_sSteeringError)>60)
//       {
//           start_V_L=380;
//       }
//       else
//       {
//           start_V_L=450;
//       }
//      if(abs(last_g_sSteeringError)>80&&abs(g_sSteeringError)<2)//��ֹ���ͻ��
//      {
//        g_sSteeringError=last_g_sSteeringError;
//      }
//      last_g_sSteeringError=g_sSteeringError;
//
//       g_sSteeringError=(float)updateDeviation(filter,20);  //�˲�

//       if(Circle_flag==2)//�ж����һ���һ��ʱ����Ǹ�ֵ
//       {
//           g_sSteeringError=Cirle_PWM;//���ƫ��
//       }
//       if(abs(g_sSteeringError)>10)
//       {
           MotorDuty2 = (int)PidLocCtrl(&RSpeed_PID, g_sSteeringError);//����ͷ
//       }
//       if(MotorDuty2>5000)MotorDuty2=5000;
//       if(MotorDuty2<-5000)MotorDuty2=-5000;
   }
   //��ŷ��򻷿���
   if(open_flag!=0&&motor_flag==2)//û���ϰ���Ž���
   {
       //ALL_AD=(float)updateDeviation(filter,20);  //�˲�
       MotorDuty2 = (int)PidLocCtrl(&ESpeed_PID, ALL_AD);//���
   }
    //�ٶȻ�����
   if(motor_flag==0||motor_flag==2)
   {
       Target_Speed1=start_V_L;
       Target_Speed2=start_V_L;
       V_error=(float)((start_V_L-((Real_Speed1+Real_Speed2)/2))/20);
       MotorDuty1 = (int)PidIncCtrl(&LSpeed_PID, V_error);

       out_PWML=MotorDuty1+MotorDuty2;
       out_PWMR=MotorDuty1-MotorDuty2;
   }



  //����ת�޷�
   if(open_flag==0)
   {
       if(out_PWML>7300)  //7300
       {
           out_PWML=7300;
       }
       if(out_PWMR>7000) //7300
       {
           out_PWMR=7000;
       }
//       if(ECPULSE1>0&&out_PWML<900)  //7300
//       {
//           out_PWML=900;
//       }
//       if(out_PWMR>0&&out_PWMR<900) //7300
//       {
//           out_PWMR=900;
//       }


       if(out_PWML<(-1*6000))  //5500
       {
           out_PWML=(-1*6000);
       }
       if(out_PWMR<(-1*6500))  //6500
       {
           out_PWMR=(-1*6500);
       }
       if(motor_flag==2)
       {
           if(abs(ECPULSE1)==0)  //7300
           {
               out_PWML=-2500;
           }
           if(abs(ECPULSE2)==0)  //7300
           {
               out_PWMR=-2500;
           }
          if(out_PWML<0&&out_PWML>-2000)  //7300
          {
              out_PWML=-2000;
          }
          if(out_PWMR<0&&out_PWMR>-2000) //7300
          {
              out_PWMR=-2000;
          }
       }

   }
   if(open_flag!=0)
   {
       if(out_PWML>5300)  //7300
       {
           out_PWML=5300;
       }
       if(out_PWMR>5000) //7300
       {
           out_PWMR=5000;
       }

       if(out_PWML<(-1*5000))  //5500
       {
           out_PWML=(-1*5000);
       }
       if(out_PWMR<(-1*5500))  //6500
       {
           out_PWMR=(-1*5500);
       }
       if(motor_flag==2)
       {
           if(abs(ECPULSE1)==0)  //7300
           {
               out_PWML=-2500;
           }
           if(abs(ECPULSE2)==0)  //7300
           {
               out_PWMR=-2500;
           }
          if(out_PWML<0&&out_PWML>-2000)  //7300
          {
              out_PWML=-2000;
          }
          if(out_PWMR<0&&out_PWMR>-2000) //7300
          {
              out_PWMR=-2000;
          }
       }
   }
   //�ջ�����������
   if(Obstacle_flag==0
    &&Circle_flag_L!=2&&Circle_flag_L!=4
    &&Circle_flag_R!=2&&Circle_flag_R!=4
    &&Park_flag!=3&&Park_flag!=4
    )
   {
       if(out_PWML>=0)
       {
           ATOM_PWM_SetDuty(ATOMPWM5, 1*out_PWML, 17000);
           ATOM_PWM_SetDuty(ATOMPWM4, 0, 17000);
       }
       else if(out_PWML<0)
       {
           ATOM_PWM_SetDuty(ATOMPWM5, 0, 17000);
           ATOM_PWM_SetDuty(ATOMPWM4, -1*out_PWML, 17000);
       }

       if(out_PWMR>=0)
       {
           ATOM_PWM_SetDuty(ATOMPWM7, out_PWMR, 17000);
           ATOM_PWM_SetDuty(ATOMPWM6, 0, 17000);
       }
       else if(out_PWMR<0)
       {
           ATOM_PWM_SetDuty(ATOMPWM7, 0, 17000);
           ATOM_PWM_SetDuty(ATOMPWM6, (-1)*out_PWMR, 17000);
       }
   }
   //ͣ��
   if(motor_flag == 0)
   {
       start_V_L=0;
       Target_Speed1=0;
       Target_Speed2=0;
       MotorDuty1=0;
       MotorDuty2=0;
       out_PWML=0;
       out_PWMR=0;
//      ATOM_PWM_SetDuty(ATOMPWM4, 0, 17000);
//      ATOM_PWM_SetDuty(ATOMPWM5, 0, 17000);
//      ATOM_PWM_SetDuty(ATOMPWM6, 0, 17000);
//      ATOM_PWM_SetDuty(ATOMPWM7, 0, 17000);
   }
    //PIN_Reverse(P15_2);//�����ж�ʱ��
}

/*************************************************************************
 *  �������ƣ�void CCU60_CH1_IRQHandler(void)
 *  ����˵����
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2022��11��28��
 *  ��    ע��CCU60_CH1ʹ�õ��жϷ�����
 *************************************************************************/
int meun_flag;
void CCU60_CH1_IRQHandler (void)
{
    /* ����CPU�ж�  �����жϲ���Ƕ�� */
    IfxCpu_enableInterrupts();
    // ����жϱ�־
    IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU60, IfxCcu6_InterruptSource_t13PeriodMatch);

    /* �û����� */
    meun_flag=1;
    /* ��ȡ������ֵ */
    ECPULSE1= (-1)*ENC_GetCounter(ENC6_InPut_P20_3);  // ��� ĸ���ϱ�����2��С������ С��ǰ��Ϊ��ֵ
    ECPULSE2= ENC_GetCounter(ENC4_InPut_P02_8); // ��� ĸ���ϱ�����1��С������ С��ǰ��Ϊ��ֵ
    //�����ǻ�ȡ���ٶ�
    Get_aac();
    //��ȡ���ֵ
    GetlastAdcValue();
    //��ʱ����ʱ
    if(motor_flag==1
     //������״
     ||rectangle_flag==1
     //�ϰ���
     ||Obstacle_flag==1||Obstacle_flag==2||Obstacle_flag==3
     //��·
     ||open_flag==1||open_flag==2
     //��Բ��
     ||Circle_flag_L==1||Circle_flag_L==2||Circle_flag_L==3||Circle_flag_L==4||Circle_flag_L==5
     //��Բ��
     ||Circle_flag_R==1||Circle_flag_R==2||Circle_flag_R==3||Circle_flag_R==4||Circle_flag_R==5
     //�µ�
     ||slope_flag==1||slope_flag==2
     //����
     ||Park_flag==1||Park_flag==2 ||Park_flag==3||Park_flag==4)
    {
        Obstacle_num++;
    }
    //������������ʱ
    if(Obstacle_flag==1)
    {
        ECPULSE_Obstacle_L+=ECPULSE1;  //���ּ���
    }
    if(Obstacle_flag==2||open_flag==1||open_flag==2||slope_flag==1)
    {
        ECPULSE_Obstacle_R+=ECPULSE2;  //���ּ���
    }
   //�ٶ��˲�
//    Real_Speed1=(float)Left_Low_Pass_Filter(ECPULSE1,Feedback_SpeedL,Feedback_Speed_tmpL);
//    Real_Speed2=(float)Right_Low_Pass_Filter(ECPULSE2,Feedback_SpeedR,Feedback_Speed_tmpR);
    Real_Speed1=(float)(5.2051*ECPULSE1);  //ʵ��ת��   תÿ����
    Real_Speed2=(float)(5.2051*ECPULSE2);  //ʵ��ת��
//    x++;
    //��ѹ��ʾ
 //   Voltage_Get();

    /* �ٶȿ��� */
//    if (motor_flag==1)
//    {
        //�ٶ�ת����
        //AngleControl();  //ȷ��OUT_PWM��ֵ
        //�ٶȱջ�
//        if(x==20)
//        {
//            PIN_Reverse(P15_2);//�����ж�ʱ��
//            x=0;


//           Real_Speed1=(float)(5.2051*ECPULSE_L - 1.3837);  //ʵ��ת��   תÿ����
//           Real_Speed2=(float)(5.5562*ECPULSE_R - 11.058);  //ʵ��ת��   תÿ����

//           Target_Speed1=start_V_L;
//           Target_Speed2=start_V_L;
//
//           V_error=(float)((Target_Speed1-((Real_Speed1+Real_Speed2)/2))/5);
//
//           if(Obstacle_flag==0)
//           {
//               MotorDuty1 = (int)PidIncCtrl(&LSpeed_PID, V_error);
//               if(MotorDuty1>5500)
//               {
//                   MotorDuty1=5500;
//               }
//               if(MotorDuty1<=-5000)
//               {
//                   MotorDuty1=-5000;
//               }
//           }
//       }
//     }
}

/*************************************************************************
 *  �������ƣ�void CCU61_CH0_IRQHandler(void)
 *  ����˵����
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2022��11��28��
 *  ��    ע��CCU61_CH0ʹ�õ��жϷ�����
 *************************************************************************/
void CCU61_CH0_IRQHandler (void)
{
    /* ����CPU�ж�  �����жϲ���Ƕ�� */
    IfxCpu_enableInterrupts();

    // ����жϱ�־
    IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU61, IfxCcu6_InterruptSource_t12PeriodMatch);

    /* �û����� */
}

/*************************************************************************
 *  �������ƣ�void CCU61_CH1_IRQHandler(void)
 *  ����˵����
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2022��11��28��
 *  ��   ע��CCU61_CH1ʹ�õ��жϷ�����
 *************************************************************************/
extern uint16 MagneticField;
int i_delay=0;
void CCU61_CH1_IRQHandler (void)
{
    //static uint8_t i = 0;
    /* ����CPU�ж�  �����жϲ���Ƕ�� */
    IfxCpu_enableInterrupts();

    // ����жϱ�־
    IfxCcu6_clearInterruptStatusFlag(&MODULE_CCU61, IfxCcu6_InterruptSource_t13PeriodMatch);

    /* �û����� */


}

/*************************************************************************
 *  �������ƣ�CCU6_InitConfig CCU6
 *  ����˵������ʱ�������жϳ�ʼ��
 *  ����˵����ccu6    �� ccu6ģ��            CCU60 �� CCU61
 *  ����˵����channel �� ccu6ģ��ͨ��  CCU6_Channel0 �� CCU6_Channel1
 *  ����˵����us      �� ccu6ģ��  �ж�����ʱ��  ��λus
 *  �������أ���
 *  �޸�ʱ�䣺2020��3��30��
 *  ��    ע��    CCU6_InitConfig(CCU60, CCU6_Channel0, 100);  // 100us����һ���ж�
 *************************************************************************/
void CCU6_InitConfig (CCU6_t ccu6, CCU6_Channel_t channel, uint32 us)
{
    IfxCcu6_Timer_Config timerConfig;

    Ifx_CCU6 * module = IfxCcu6_getAddress((IfxCcu6_Index) ccu6);

    uint8 Index = ccu6 * 2 + channel;

    uint32 period = 0;

    uint64 clk = 0;

    /* �ر��ж� */
    boolean interrupt_state = disableInterrupts();

    IfxCcu6_Timer_initModuleConfig(&timerConfig, module);

    clk = IfxScuCcu_getSpbFrequency();

    /* ����ʱ��Ƶ��  */
    uint8 i = 0;
    while (i++ < 16)
    {
        period = (uint32) (clk * us / 1000000);
        if (period < 0xffff)
        {
            break;
        }
        else
        {
            clk = clk / 2;
        }
    }
    switch (channel)
    {
        case CCU6_Channel0 :
            timerConfig.timer = IfxCcu6_TimerId_t12;
            timerConfig.interrupt1.source = IfxCcu6_InterruptSource_t12PeriodMatch;
            timerConfig.interrupt1.serviceRequest = IfxCcu6_ServiceRequest_1;
            timerConfig.base.t12Frequency = (float) clk;
            timerConfig.base.t12Period = period;                                  // ���ö�ʱ�ж�
            timerConfig.clock.t12countingInputMode = IfxCcu6_CountingInputMode_internal;
            timerConfig.timer12.counterValue = 0;
            timerConfig.interrupt1.typeOfService = Ccu6IrqVectabNum[ccu6];
            timerConfig.interrupt1.priority = Ccu6IrqPriority[Index];
            break;

        case CCU6_Channel1 :
            timerConfig.timer = IfxCcu6_TimerId_t13;
            timerConfig.interrupt2.source = IfxCcu6_InterruptSource_t13PeriodMatch;
            timerConfig.interrupt2.serviceRequest = IfxCcu6_ServiceRequest_2;
            timerConfig.base.t13Frequency = (float) clk;
            timerConfig.base.t13Period = period;
            timerConfig.clock.t13countingInputMode = IfxCcu6_CountingInputMode_internal;
            timerConfig.timer13.counterValue = 0;
            timerConfig.interrupt2.typeOfService = Ccu6IrqVectabNum[ccu6];
            timerConfig.interrupt2.priority = Ccu6IrqPriority[Index];
            break;
    }

    timerConfig.trigger.t13InSyncWithT12 = FALSE;

    IfxCcu6_Timer Ccu6Timer;

    IfxCcu6_Timer_initModule(&Ccu6Timer, &timerConfig);

    IfxCpu_Irq_installInterruptHandler((void*) Ccu6IrqFuncPointer[Index], Ccu6IrqPriority[Index]);          // �����жϺ������жϺ�

    restoreInterrupts(interrupt_state);

    IfxCcu6_Timer_start(&Ccu6Timer);
}

/*************************************************************************
 *  �������ƣ�CCU6_DisableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel)
 *  ����˵����ֹͣCCU6ͨ���ж�
 *  ����˵����ccu6    �� ccu6ģ��            CCU60 �� CCU61
 *  ����˵����channel �� ccu6ģ��ͨ��  CCU6_Channel0 �� CCU6_Channel1
 *  �������أ���
 *  �޸�ʱ�䣺2020��5��6��
 *  ��    ע��
 *************************************************************************/
void CCU6_DisableInterrupt (CCU6_t ccu6, CCU6_Channel_t channel)
{
    Ifx_CCU6 * module = IfxCcu6_getAddress((IfxCcu6_Index) ccu6);
    IfxCcu6_clearInterruptStatusFlag(module, (IfxCcu6_InterruptSource) (7 + channel * 2));
    IfxCcu6_disableInterrupt(module, (IfxCcu6_InterruptSource) (7 + channel * 2));

}

/*************************************************************************
 *  �������ƣ�CCU6_DisableInterrupt(CCU6_t ccu6, CCU6_Channel_t channel)
 *  ����˵����ʹ��CCU6ͨ���ж�
 *  ����˵����ccu6    �� ccu6ģ��            CCU60 �� CCU61
 *  ����˵����channel �� ccu6ģ��ͨ��  CCU6_Channel0 �� CCU6_Channel1
 *  �������أ���
 *  �޸�ʱ�䣺2020��5��6��
 *  ��    ע��
 *************************************************************************/
void CCU6_EnableInterrupt (CCU6_t ccu6, CCU6_Channel_t channel)
{
    Ifx_CCU6 * module = IfxCcu6_getAddress((IfxCcu6_Index) ccu6);
    IfxCcu6_clearInterruptStatusFlag(module, (IfxCcu6_InterruptSource) (7 + channel * 2));
    IfxCcu6_enableInterrupt(module, (IfxCcu6_InterruptSource) (7 + channel * 2));

}

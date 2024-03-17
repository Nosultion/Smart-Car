/*
 * TFT_Key_control.c
 *
 *  Created on: 2023��4��19��
 *      Author: HSC
 *
 */

#include "TFT_Key_control.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Platform_Types.h>
#include "../APP/LQ_TFT18.h"
#include "LQ_PID.h"
#include "LQ_UART.h"
#include "stdlib.h"
#include "LQ_ImageProcess.h"
#include "LQ_CCU6.h"
#include "LQ_MotorServo.h"
#include "LQ_GPIO_KEY.h"
#include "Image_Process.h"
#include "LQ_ADC.h"
#include "LQ_I2C_9AX.h"
#include "LQ_AnoScope.h"
#include "LQ_Atom_Motor.h"
#include "LQ_GTM.h"
#include "LQ_GPIO_BUZZ.h"
#include "LQ_GPIO_Infrared.h"
#include "LQ_ADC.h"
#include "LQ_EEPROM.h"

extern float g_sSteeringError;
extern int ROAD_MAIN_ROW;
extern float K_error;
extern float K_adjust;
extern unsigned char Show_TFT_Flag;
extern volatile sint16 high;
extern uint8_t leftup[2] ;
extern uint8_t rightup[2] ;
extern uint8_t leftup_1[2];
extern uint8_t rightup_1[2] ;

uint8_t point_num=0;

extern unsigned short THRESHOLD;
extern float V_error;
int min_tag=0;//������Ӧ����

extern int yuzhi;

int Voltage,vol_last;

uint8_t left_point[4];
uint8_t right_point[4];

extern int out_PWML,out_PWMR;

float Anglex=0,Angley=0,Anglez=0;//�����ǽǶ�
float Angularz_speed = 0;//ˮƽ������ٶ�
extern float KZ_L;

int Feedback_SpeedL,Feedback_Speed_tmpL,Feedback_SpeedR,Feedback_Speed_tmpR;

extern short left_X,left_V,right_X, right_V;
extern float ALL_AD;
extern uint8_t open_flag;
extern uint8_t Obstacle_flag;
extern volatile int ECPULSE_open;
extern volatile int ECPULSE_Obstacle_L;
extern volatile int ECPULSE_Obstacle_R;

int ipts0[160][2],ipts1[160][2];
int begin_x=20,begin_y=80;
int ipts0_num, ipts1_num;
int thres = 120;
float block_size = 7;
float clip_value = 0;
int stepL = 0,stepR=0;

int select_flag=1,param_num=13;

extern unsigned char Image_Use_BLACK[IMAGEH][IMAGEW];
/** ͼ��ԭʼ���ݴ�� */
extern unsigned char Image_Data[IMAGEH][IMAGEW];
extern uint8_t ImageSide[LCDH][2];         //���ұ�������
extern int L_pointX[2],L_pointY[2],R_pointX[2],R_pointY[2],L_point_num,R_point_num;

extern uint8_t rectangle_flag;
extern uint8_t open_flag;  //��·��־λ
extern uint8_t Obstacle_flag;//·�ϱ�־λ
extern uint8_t slope_flag;  //�µ���־λ
extern uint8_t Park_flag;   //�����־λ
extern uint8_t Circle_flag_L;//��Բ����־λ
extern uint8_t Circle_flag_R;//��Բ����־λ
extern uint8_t cross_flag;//ʮ�ֱ�־λ
extern int Obstacle_turn_flag;//·�����ҹձ�־λ 0����� 1���ҹ�
extern int motor_turn_flag;//�������ҹձ�־λ    0����� 1���ҹ�
extern int Speed_turn_flag;//�ٶ�ģʽ��־λ   0�Ǹ���ȫԪ�� 1�ǵ���ȫԪ�� 2�ǵ�����Ԫ��
int Circle_YuZhi=5600;  //Բ����ֵ
int Circle_Yanshi=25;  //Բ��ʶ����ʱʱ��

extern int Obstacle_num,Circle_num;
extern int ADC_Value[4];       //��ŵĲɼ�ֵ
extern float A,B,C,P;
extern float curvature;
extern uint16 MagneticField;
//��Ļ��ʾ
char txt[16];
//�ڱ�������
int currentzhongjian[120],right_heixian[60],left_heixian[60],zhidao_flag,right_baixian[60],left_baixian[60];
int qulv_jinduan_right=0,qulv_jinduan_left=0,qulv_yuandaun_right=0,qulv_yuandaun_left=0;
int qvlv_quanju_right=0,qvlv_quanju_left=0;
int qvlv_quanju,qulv_jinduan,qulv_yuandaun;
int s_point=0,S_COUNT=0,fuduandian=0;
int qulv_s_y=0,qulv_s_l=0;
int duandian,duandianshu,duandianshu1,currentzhongjian_lk;//�ϵ㣬�ϵ���
int B_lk=0;

int turn_flag=0;
int turn_num=0;

//EEPROM��д������
unsigned long u32wBuff[4];
unsigned long u32rBuff[4];

float f32wBuff[20];
float f32rBuff[20];

//�����˵�
int menu_flag=0;
int num1=2;
int num2=0;
void menu(void)
{
    menu_flag_function();  //���ƽ�����˳��˵�
    if(menu_flag==0)
    {
        num2=0;
        sprintf(txt, " T_L%.0f V_L%3.0f  ", Target_Speed1,Real_Speed1);             //����Ŀ���ٶ�
        TFTSPI_P8X16Str(1, 0, txt, u16RED, u16WHITE);
        sprintf(txt, " T_R%.0f V_R%3.0f  ", Target_Speed2,Real_Speed2);             //����Ŀ���ٶ�
        TFTSPI_P8X16Str(1, 1, txt, u16RED, u16WHITE);
        sprintf(txt, " Image_data        ");
        TFTSPI_P8X16Str(1, 2, txt,u16ORANGE,u16BLACK);
        sprintf(txt, " Image_show         ");
        TFTSPI_P8X16Str(1, 3, txt,u16ORANGE,u16BLACK);
        sprintf(txt, " Inductance        ");
        TFTSPI_P8X16Str(1, 4, txt,u16ORANGE,u16BLACK);
        sprintf(txt, " PID               ");
        TFTSPI_P8X16Str(1, 5, txt,u16ORANGE,u16BLACK);
        sprintf(txt, " Camera             ");
        TFTSPI_P8X16Str(1, 6, txt,u16ORANGE,u16BLACK);
        sprintf(txt, " Road_Sign        ");
        TFTSPI_P8X16Str(1, 7, txt,u16ORANGE,u16BLACK);
        sprintf(txt, " EEPROM_Write           ");
        TFTSPI_P8X16Str(1, 8, txt,u16ORANGE,u16BLACK);
        sprintf(txt, " EEPROM_Read           ");
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
        arrow(&num1,2,9);
    }

    if(menu_flag!=0)
    {
        switch(num1){
            case 2: show_Dynamic();
                    arrow(&num2,0,6);
                    Dynamic_Value_Change();
                    break;

            case 3: show_Image();
                    break;

            case 4: show_Inductance();
                    break;

            case 5: show_PID();
                    arrow(&num2,0,8);
                    PID_Value_Change();
                    break;

            case 6: show_Error();
                    break;

            case 7: show_Sign();
                    break;

            case 8: EEPROM_input();
                    break;

            case 9: EEPROM_output();
                    break;
        }
    }

}

void show_PID(void)//��ʾPIDҳ��
{
    sprintf(txt, " Rp: %.1f         ", RSpeed_PID.kp);
    TFTSPI_P8X16Str(1, 0, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " Ri: %.1f         ", RSpeed_PID.ki);
    TFTSPI_P8X16Str(1, 1, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " Rd: %.1f         ", RSpeed_PID.kd);
    TFTSPI_P8X16Str(1, 2, txt,u16ORANGE,u16BLACK);

    sprintf(txt, " Ep: %.1f         ", ESpeed_PID.kp);
    TFTSPI_P8X16Str(1, 3, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " Ei: %.1f         ", ESpeed_PID.ki);
    TFTSPI_P8X16Str(1, 4, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " Ed: %.1f         ", ESpeed_PID.kd);
    TFTSPI_P8X16Str(1, 5, txt,u16ORANGE,u16BLACK);

    sprintf(txt, " Lp: %.1f         ", LSpeed_PID.kp);
    TFTSPI_P8X16Str(1, 6, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " Li: %.1f         ", LSpeed_PID.ki);
    TFTSPI_P8X16Str(1, 7, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " Ld: %.1f         ", LSpeed_PID.kd);
    TFTSPI_P8X16Str(1, 8, txt,u16ORANGE,u16BLACK);
}

void show_Error(void)//��ʾ���ҳ��
{
    sprintf(txt, " G-error: %d         ", (int)g_sSteeringError);
    TFTSPI_P8X16Str(1, 0, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " E_L:%d E_R:%d        ", ECPULSE1,ECPULSE2);             //�����ֱ�����
    TFTSPI_P8X16Str(1, 1, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " D_1:%-5d             ", MotorDuty1);           //����PWMֵ
    TFTSPI_P8X16Str(1, 2, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " D_2:%-5d             ", MotorDuty2);           //����PWMֵ
    TFTSPI_P8X16Str(1, 3, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " D_L:%-5d             ", out_PWML);           //����PWMֵ
    TFTSPI_P8X16Str(1, 4, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " D_R:%-5d             ", out_PWMR);           //����PWMֵ
    TFTSPI_P8X16Str(1, 5, txt,u16ORANGE,u16BLACK);

    sprintf(txt, " X:%.0f        ",Anglex);//�����ǽǶ�
    TFTSPI_P8X16Str(1, 6, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " Z:%.0f        ",Anglez);//�����ǽǶ�
    TFTSPI_P8X16Str(1, 7, txt,u16ORANGE,u16BLACK);

    sprintf(txt, " qvlv:%d lk:%d        ",(int)qvlv_quanju,(int)B_lk); //����
    TFTSPI_P8X16Str(1, 8, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " dianshu:%d           ",(int)duandianshu); //�ϵ�
    TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
}

void show_Inductance(void)//��ʾ���ҳ��
{
    sprintf(txt, " V0: %d %d          ", ADC_Value[0],left_X);
    TFTSPI_P8X16Str(1, 0, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " V1: %d %d          ", ADC_Value[1],left_V);
    TFTSPI_P8X16Str(1, 1, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " V2: %d %d          ", ADC_Value[2],right_V);
    TFTSPI_P8X16Str(1, 2, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " V3: %d %d          ", ADC_Value[3],right_X);
    TFTSPI_P8X16Str(1, 3, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " ALL: %d            ", (int)MagneticField);
    TFTSPI_P8X16Str(1, 4, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " E-error: %d        ", (int)ALL_AD);
    TFTSPI_P8X16Str(1, 5, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " D_L:%-5d           ", MotorDuty1);           //����PWMֵ
    TFTSPI_P8X16Str(1, 6, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " D_R:%-5d           ", MotorDuty2);           //����PWMֵ
    TFTSPI_P8X16Str(1, 7, txt,u16ORANGE,u16BLACK);
}

void show_Sign(void) //��ʾ��־λ
{
   sprintf(txt, " rectangle: %d         ", (int)rectangle_flag);
   TFTSPI_P8X16Str(1, 0, txt,u16ORANGE,u16BLACK);
   sprintf(txt, " open:      %d         ", (int)open_flag);
   TFTSPI_P8X16Str(1, 1, txt,u16ORANGE,u16BLACK);
   sprintf(txt, " Obstacle:  %d         ", (int) Obstacle_flag);
   TFTSPI_P8X16Str(1, 2, txt,u16ORANGE,u16BLACK);
   sprintf(txt, " slope:     %d         ", (int) slope_flag);
   TFTSPI_P8X16Str(1, 3, txt,u16ORANGE,u16BLACK);
   sprintf(txt, " Park:      %d         ", (int) Park_flag);
   TFTSPI_P8X16Str(1, 4, txt,u16ORANGE,u16BLACK);
   sprintf(txt, " CircleL:   %d         ", (int)Circle_flag_L);
   TFTSPI_P8X16Str(1, 5, txt,u16ORANGE,u16BLACK);
   sprintf(txt, " CircleR:   %d         ", (int)Circle_flag_R);
   TFTSPI_P8X16Str(1, 6, txt,u16ORANGE,u16BLACK);
   sprintf(txt, " cross:     %d         ", (int) cross_flag);
   TFTSPI_P8X16Str(1, 7, txt,u16ORANGE,u16BLACK);
   sprintf(txt, " turn_num:  %d         ", (int) turn_num);
   TFTSPI_P8X16Str(1, 8, txt,u16ORANGE,u16BLACK);
}

void show_Dynamic(void)//��ʾ��̬��ֵ
{
    sprintf(txt, " M_turn:   %d     ",(int)motor_turn_flag);//��������  0��  1
    TFTSPI_P8X16Str(1, 0, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " O_turn:   %d     ",(int)Obstacle_turn_flag);//���Ϸ���  0��  1
    TFTSPI_P8X16Str(1, 1, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " S_turn:   %d      ",(int)Speed_turn_flag);//�ٶ�ģʽ  0����Ԫ�� 1����Ԫ�� 2������Ԫ��
    TFTSPI_P8X16Str(1, 2, txt,u16ORANGE,u16BLACK);

    sprintf(txt, " C_YuZhi: %d     ",(int)Circle_YuZhi);//Բ�������ֵ
    TFTSPI_P8X16Str(1, 3, txt,u16ORANGE,u16BLACK);

    sprintf(txt, " C_Yanshi: %d    ",(int)Circle_Yanshi);//Բ����ʱʱ��
    TFTSPI_P8X16Str(1, 4, txt,u16ORANGE,u16BLACK);

    sprintf(txt, " ROW:%d               ",(int)ROAD_MAIN_ROW);//�ۿ���
    TFTSPI_P8X16Str(1, 5, txt,u16ORANGE,u16BLACK);
    sprintf(txt, " th:%d                ",(int)thres);//��ֵ
    TFTSPI_P8X16Str(1, 6, txt,u16ORANGE,u16BLACK);


}

int a=1;
void show_Image(void)//��ʾͼ��
{
    if(num1==3&&a==1)
    {
        TFTSPI_Init(0);
        a=2;
    }
    if(num2==0)
    {
        TFTSPI_Road(0, 0, LCDH, LCDW,Image_Use_BLACK);
        TFTSPI_Draw_Line(0, (unsigned char)ROAD_MAIN_ROW, 159, (unsigned char)ROAD_MAIN_ROW, u16RED);//�����
        sprintf(txt, " G-error: %d         ", (int)g_sSteeringError);
        if(KEY_Read(KEY1)==0)
        {
            num2=1;
        }
    }
    if(num2==1)
    {
        TFTSPI_Road(0, 0, LCDH, LCDW,Image_Data);
        TFTSPI_Draw_Line(0, (unsigned char)ROAD_MAIN_ROW, 159, (unsigned char)ROAD_MAIN_ROW, u16RED);//�����
        if(KEY_Read(KEY1)==0)
        {
            num2=0;
        }
    }

}

//EEPROM����д��
void EEPROM_input(void)
{
    u32wBuff[0] = ROAD_MAIN_ROW;
    u32wBuff[1] = thres;
    u32wBuff[2] = Circle_YuZhi;
    u32wBuff[3] = Circle_Yanshi;
//    u32wBuff[9]=1;

    f32wBuff[0] = LSpeed_PID.kp;
    f32wBuff[1] = LSpeed_PID.ki;
    f32wBuff[2] = LSpeed_PID.kd;
    f32wBuff[3] = RSpeed_PID.kp;
    f32wBuff[4] = RSpeed_PID.ki;
    f32wBuff[5] = RSpeed_PID.kd;
    f32wBuff[6] = ESpeed_PID.kp;
    f32wBuff[7] = ESpeed_PID.ki;
    f32wBuff[8] = ESpeed_PID.kd;

    f32wBuff[19]=1;
    EEPROM_EraseSector(0);
    EEPROM_EraseSector(1);

    EEPROM_Write(0, 0, u32wBuff, 5);
    EEPROM_Write(1, 0, (unsigned long*)f32wBuff, 21);
}

//EEPROM���ݶ���
void EEPROM_output(void)
{

    EEPROM_Read(0, 0, u32rBuff, 5);
    EEPROM_Read(1, 0, (unsigned long*)f32rBuff, 21);

    ROAD_MAIN_ROW = u32rBuff[0];
    thres = u32rBuff[1];

    Circle_YuZhi = u32rBuff[2];
    Circle_Yanshi = u32rBuff[3];

    LSpeed_PID.kp = f32rBuff[0];
    LSpeed_PID.ki = f32rBuff[1];
    LSpeed_PID.kd = f32rBuff[2];
    RSpeed_PID.kp = f32rBuff[3];
    RSpeed_PID.ki = f32rBuff[4];
    RSpeed_PID.kd = f32rBuff[5];
    ESpeed_PID.kp = f32rBuff[6];
    ESpeed_PID.ki = f32rBuff[7];
    ESpeed_PID.kd = f32rBuff[8];
}

void menu_flag_function(void)//�˵���־����������һ�������ת
{
    if(KEY_Read(KEY2)==0)
    {
        menu_flag=!menu_flag;
        if(num1==3&&menu_flag==0)
        {
            TFTSPI_Init(1);
            a=1;
        }
        TFTSPI_CLS(u16YELLOW);
    }

}

void arrow(int *num,int line_begin,int line_end)//��ͷ��ʾ����
{
    if(KEY_Read(KEY0)==0)
    {
        (*num)--;
        if((*num)<line_begin)
        {
            (*num)=line_end;
        }
    }
    if(KEY_Read(KEY1)==0)
    {
        (*num)++;
        if((*num)>line_end)
        {
            (*num)=line_begin;
        }
    }
    TFTSPI_P8X16Str(1,(unsigned char)(*num), ">",u16ORANGE,u16BLACK);
}

void PID_Value_Change(void)//PID��������
{
    switch(num2){
        case 0: ChangeValue_float(&RSpeed_PID.kp);
                break;
        case 1: ChangeValue_float(&RSpeed_PID.ki);
                break;
        case 2: ChangeValue_float(&RSpeed_PID.kd);
                break;
        case 3: ChangeValue_float(&ESpeed_PID.kp);
                break;
        case 4: ChangeValue_float(&ESpeed_PID.ki);
                break;
        case 5: ChangeValue_float(&ESpeed_PID.kd);
                break;
        case 6: ChangeValue_float(&LSpeed_PID.kp);
                break;
        case 7: ChangeValue_float(&LSpeed_PID.ki);
                break;
        case 8: ChangeValue_float(&LSpeed_PID.kd);
                break;
    }
}

void Dynamic_Value_Change(void)//��̬��ֵ��������
{
    switch(num2){

        case 1: Change_Obstacle_turn_flag();
                break;
        case 2: Change_Speed_turn_flag();
                break;
        case 3: ChangeValue_int_fifty(&Circle_YuZhi);
                break;
        case 4: ChangeValue_int(&Circle_Yanshi);
                break;
        case 5: ChangeValue_int(&ROAD_MAIN_ROW);
                break;
        case 6: ChangeValue_int(&thres);
                break;

    }
}

void Change_Obstacle_turn_flag(void)
{
    if(KEY_Read(DSW1)==1)
    {
        Obstacle_turn_flag-=1;
    }
    if(KEY_Read(DSW0)==1)
    {
        Obstacle_turn_flag+=1;
    }
    if(Obstacle_turn_flag>1)
    {
        Obstacle_turn_flag=0;
    }
    if(Obstacle_turn_flag<0)
    {
        Obstacle_turn_flag=1;
    }
}

void Change_Speed_turn_flag(void)
{
    if(KEY_Read(DSW1)==1)
    {
        Speed_turn_flag-=1;
    }
    if(KEY_Read(DSW0)==1)
    {
        Speed_turn_flag+=1;
    }
    if(Speed_turn_flag>2)
    {
        Speed_turn_flag=0;
    }
    if(Speed_turn_flag<0)
    {
        Speed_turn_flag=2;
    }
}

void ChangeValue_float(float *object)//float���͵Ĳ�������
{
    if(KEY_Read(DSW1)==1)
    {
        (*object)-=0.5;
    }
    if(KEY_Read(DSW0)==1)
    {
        (*object)+=0.5;
    }
}

void ChangeValue_int(int *object)//int���͵Ĳ�������
{
    if(KEY_Read(DSW1)==1)
    {
        (*object)-=1;
    }
    if(KEY_Read(DSW0)==1)
    {
        (*object)+=1;
    }
}

void ChangeValue_int_fifty(int *object)//int���͵Ĳ�������
{
    if(KEY_Read(DSW1)==1)
    {
        (*object)-=50;
    }
    if(KEY_Read(DSW0)==1)
    {
        (*object)+=50;
    }
}



void TFT_show(void)
{
    //�� 1��
    sprintf(txt, "ROW:%d th:%d  ",(int)ROAD_MAIN_ROW,(int)thres);           //����PWMֵ
    TFTSPI_P8X16Str(1, 0, txt, u16BLUE, u16WHITE);
    // ��2 3��
    sprintf(txt, "T_L%.0f V_L%3.0f  ", Target_Speed1,Real_Speed1);             //����Ŀ���ٶ�
    TFTSPI_P8X16Str(1, 1, txt, u16RED, u16BLUE);
    sprintf(txt, "T_R%.0f V_R%3.0f  ", Target_Speed2,Real_Speed2);             //����Ŀ���ٶ�
    TFTSPI_P8X16Str(1, 2, txt, u16RED, u16BLUE);

    // �� 4 5 6 7��
    sprintf(txt, "E_L:%d E_R:%d   ", ECPULSE1,ECPULSE2);             //�����ֱ�����
    TFTSPI_P8X16Str(1, 3, txt, u16RED, u16WHITE);
    sprintf(txt, "D_L:%-5d", MotorDuty1);           //����PWMֵ
    TFTSPI_P8X16Str(1, 4, txt, u16BLUE, u16RED);
    sprintf(txt, "D_R:%-5d", MotorDuty2);           //����PWMֵ
    TFTSPI_P8X16Str(1, 5, txt, u16BLUE, u16RED);
    sprintf(txt, "error: %-.0f-%.0f   ", g_sSteeringError,ALL_AD);
    TFTSPI_P8X16Str(1, 6, txt,u16RED,u16WHITE);

    // �� 8 9 ��
    sprintf(txt, "qvlv:%d lk:%d ",(int) qvlv_quanju,B_lk); //����
    TFTSPI_P8X16Str(1, 7, txt, u16BLACK, u16ORANGE);
    sprintf(txt, "Field:%d     ",(int) MagneticField); //�ϵ���
    TFTSPI_P8X16Str(1, 8, txt, u16RED, u16BLUE);
//    sprintf(txt, "dian:%d     ",(int) duandian); //�ϵ���
//    TFTSPI_P8X16Str(1, 8, txt, u16RED, u16BLUE);
    //��10��
    if(select_flag==1)
    {
        sprintf(txt, "S: Li: %.1f       ", LSpeed_PID.ki);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==2)
    {
        sprintf(txt, "S: Lp: %.1f       ", LSpeed_PID.kp);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==3)
    {
        sprintf(txt, "S: Rp: %.1f       ", RSpeed_PID.kp);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==4)
    {
        sprintf(txt, "S: Rd: %.1f       ", RSpeed_PID.kd);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==5)
    {
        sprintf(txt, "S: Ep: %.1f       ", ESpeed_PID.kp);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==6)
    {
        sprintf(txt, "S: Ed: %.1f       ", ESpeed_PID.kd);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==7)
    {
        sprintf(txt, "S: thres: %d       ", thres);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==8)
    {
        sprintf(txt, "S: high: %d       ", (int)ROAD_MAIN_ROW);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==9)
    {
        sprintf(txt, "K_adjust%.0f       ", K_adjust);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==10)
    {
        sprintf(txt, "ELE_A:%.1f       ", A);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==11)
    {
        sprintf(txt, "ELE_B:%.1f       ", B);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==12)
    {
        sprintf(txt, "ELE_C:%.1f       ", C);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
    else if(select_flag==13)
    {
        sprintf(txt, "ELE_P:%.1f       ", P);
        TFTSPI_P8X16Str(1, 9, txt,u16ORANGE,u16BLACK);
    }
}
extern unsigned char motor_flag;    //�����ͣ��־λ

void key_control(void)
{
    if(KEY_Read(KEY0)==0)
    {
        motor_flag=1;
        TFTSPI_Init(1);
        TFTSPI_CLS(u16YELLOW);   //��ɫ��Ļ
        Show_TFT_Flag=1;
    }
    else if(KEY_Read(KEY1)==0)
    {
        motor_flag=0;
        TFTSPI_Init(0);
        TFTSPI_CLS(u16YELLOW);   //��ɫ��Ļ
        //Show_TFT_Flag=4;
        if(Show_TFT_Flag==4)Show_TFT_Flag=3;
        else Show_TFT_Flag=4;
    }
    else if(KEY_Read(KEY2)==0)//����ѡ��
    {
        if(Show_TFT_Flag==1)
        {
            select_flag++;
        }
//        if(Show_TFT_Flag==2)
//        {
//            select_flag--;
//        }
        select_flag%=(param_num+1);
    }

    else if(KEY_Read(DSW0)==1)//����+
    {
        if(select_flag==1)
        {
            LSpeed_PID.ki+=0.5;
        }
        else if(select_flag==2)
        {
            LSpeed_PID.kp+=0.5;
        }
        else if(select_flag==3)
        {
            RSpeed_PID.kp+=0.5;
        }
        else if(select_flag==4)
        {
            RSpeed_PID.kd+=0.5;
        }
        else if(select_flag==5)
        {
            ESpeed_PID.kp+=0.5;
        }
        else if(select_flag==6)
        {
            ESpeed_PID.kd+=0.5;
        }
        else if(select_flag==7)
        {
            thres+=1;
        }
        else if(select_flag==8)
        {
            ROAD_MAIN_ROW+=1;
        }
        else if(select_flag==9)
        {
            K_adjust+=100;
        }
        else if(select_flag==10)
        {
            A+=0.1;
        }
        else if(select_flag==11)
        {
            B+=0.1;
        }
        else if(select_flag==12)
        {
            C+=0.1;
        }
        else if(select_flag==13)
        {
            P+=1;
        }
    }

    else if(KEY_Read(DSW1)==1)//����-
    {
        if(select_flag==1)
        {
            LSpeed_PID.ki-=0.5;
        }
        else if(select_flag==2)
        {
            LSpeed_PID.kp-=0.5;
        }
        else if(select_flag==3)
        {
            RSpeed_PID.kp-=0.5;
        }
        else if(select_flag==4)
        {
            RSpeed_PID.kd-=0.5;
        }
        else if(select_flag==5)
        {
            ESpeed_PID.kp-=0.5;
        }
        else if(select_flag==6)
        {
            ESpeed_PID.kd-=0.5;
        }
        else if(select_flag==7)
        {
            thres-=1;
        }
        else if(select_flag==8)
        {
            ROAD_MAIN_ROW-=1;
        }
        else if(select_flag==9)
        {
            K_adjust-=100;
        }
        else if(select_flag==10)
        {
            A-=0.1;
        }
        else if(select_flag==11)
        {
            B-=0.1;
        }
        else if(select_flag==12)
        {
            C-=0.1;
        }
        else if(select_flag==13)
        {
            P-=1;
        }
    }

}

int i_test=0;
int edge_strength=0;
int threshold = 90;
void UART_Test(void)
{
    //ͼ������
//    for (int i = 0; i < IMAGEH; i ++)          //���۸� 120
//    {
//        for (int j = 0; j < IMAGEW; j ++)     //���ۿ�188
//        {
//            //printf("Image_Use_BLACK[%d][%d]=%d",i,j,(int)Image_Use_BLACK[i][j]);
//            printf("Image_Data[%d][%d]=%d",i,j,(int)Image_Data[i][j]);
//        }
//    }
    //��ӡһ��ͼ������
//    for (int j = 0; j < IMAGEW; j ++)     //���ۿ�188
//    {
//        printf("Image_Data[55][%d]=%d\n",j,(int)Image_Data[55][j]);
//    }
    //��ӡ���ұ�������
//    for (int j = 0; j < 120; j ++)     //���ۿ�188
//    {
//        //printf("ImageSide[%d][0]=%d \n",j,(int)ImageSide[j][0]);
//        printf("ImageSide[%d][1]=%d \n",j,(int)ImageSide[j][1]);
//    }
    //��ӡ�ϵ�
    //printf("L_pointX=%d L_pointY=%d R_pointX=%d R_pointY=%d open_flag=%d \n",L_pointX[0],L_pointY[0],R_pointX[0],R_pointY[0],open_flag);

    //���ڵ���
    //printf("Show_TFT_Flag=%d\n",(int)Show_TFT_Flag);
    //printf("%.2f,%.2f\n",Real_Speed1,Real_Speed2);
    //printf("%d,%d,%d\n",(int)ECPULSE1,(int)ECPULSE2,(int)Target_Speed1*1024/100/60);
    //printf("%d,%d,%d,%.2f\n",(int)Real_Speed1,(int)Real_Speed2,(int)Target_Speed1,V_error);//�ٶȻ�����
    //printf("%d,%.2f\n",(int)MotorDuty2,g_sSteeringError);

    //vofa+ ����
    //printf("%d,%d\n",(int)out_PWML,(int)out_PWMR);
    //printf("%d,%d,%d,%d\n",(int)MotorDuty2,(int)MotorDuty_R,(int)(MotorDuty_L+MotorDuty_R),(int)(MotorDuty_L-MotorDuty_R));
    //printf("%d,%d��%d,%d\n",(int)left_X,(int)left_V,(int)right_V,(int)right_X);

    //������λ��

    ANO_DT_send_int16byte16((short)Real_Speed1,(short)Real_Speed2,(short)start_V_L,(short)MotorDuty2,(short)MotorDuty1+MotorDuty2,(short)MotorDuty1-MotorDuty2,(short)out_PWML,(short)out_PWMR);
    //ANO_DT_send_int16byte16((short)Real_Speed1,(short)Real_Speed2,(short)Target_Speed1,(short)V_error,(short)g_sSteeringError,(short)MotorDuty1,(short)MotorDuty2,(short)Angularz_speed);
    //ANO_DT_send_int16byte16((short)Real_Speed1,(short)Real_Speed2,(short)Target_Speed1,(short)left_X,(short)left_V,(short)right_V,(short)right_X,(short)ALL_AD);
    //ANO_DT_send_int16byte16((short)Real_Speed1,(short)Real_Speed2,(short)Target_Speed1,(short)ADC_Value[0],(short)ADC_Value[1],(short)ADC_Value[2],(short)ADC_Value[3],(short)ALL_AD);
    //�����ǵ���
    //printf("Anglex=%.2f Angley=%.2f Anglez=%.2f Anglex=%.2f\n",Anglex,Angley,Anglez,Angularz_speed);
}


//�Ϻ����� ����������/��ǽ�� ������Ѱ
int X_minL,X_minR,X_min;
void process_image(unsigned char Image_Use[IMAGEH][IMAGEW])
{
    // ԭͼ�����ұ���
    //���������
    int x1 = IMAGEW / 2 - begin_x, y1 = begin_y;//Ѳ����ʼ������
    int x2 = IMAGEW / 2 + begin_x, y2 = begin_y;
    X_minL=begin_y,X_minR=begin_y;
    for (int i=30; i<IMAGEW-30; i++)
    {
        if(Image_Use[begin_y][i]>thres&&Image_Use[begin_y][i+1]>thres&&Image_Use[begin_y][i+2]>thres&&Image_Use[begin_y][i+3]>thres&&Image_Use[begin_y][i+4]>thres
           &&Image_Use[begin_y][i+5]>thres&&Image_Use[begin_y][i+6]>thres&&Image_Use[begin_y][i+7]>thres&&Image_Use[begin_y][i+8]>thres&&Image_Use[begin_y][i+9]>thres
           &&Image_Use[begin_y][i+10]>thres&&Image_Use[begin_y][i+11]>thres&&Image_Use[begin_y][i+12]>thres&&Image_Use[begin_y][i+13]>thres&&Image_Use[begin_y][i+14]>thres
        )
        {
            x1=i;
            x2=i+15;
            //printf("i=%d",i);
            break;
        }
    }
    //�������
    //ipts0_num = sizeof(ipts0) / sizeof(ipts0[0]);    //������������
    ipts0_num = 160;    //������������
    for (; x1 > 0; x1--)
    {
        //printf("Image_Use[%d][%d]=%d \n",x1,y1,(int)Image_Use[x1][y1]);
        //printf("11111111111");
        if (Image_Use[y1][x1-1]< thres&&Image_Use[y1][x1]>= thres)//���������
        {
            //printf("Image_Use[%d][%d]=%d Image_Use[%d][%d]=%d\n",y1,x1-1,(int)Image_Use[y1][x1-1],y1,x1,(int)Image_Use[y1][x1]);
            findline_lefthand_adaptive(Image_Use, block_size, clip_value, x1, y1, Image_Use_BLACK, &ipts0_num);
            break;//�ҵ�����ߵ�һ���߽��
        }
    }
//    if (Image_Use[y1][x1]>= thres)
//    {
//        //printf("22222222222");
//        //printf("Image_Use[%d][%d]=%d Image_Use[%d][%d]=%d\n",y1,x1,(int)Image_Use[y1][x1],y1,x1-1,(int)Image_Use[y1][x1-1]);
//        findline_lefthand_adaptive(Image_Use, block_size, clip_value, x1, y1, Image_Use_BLACK, &ipts0_num);
//        //printf("333333333");
//        //printf("local_thres=%f \n",local_thres);
//    }
    //else ipts0_num = 0;

    //���ұ���
    ipts1_num = 160;    //�ұ����������
    for (; x2 < 188 - 1; x2++)
    {
        if (Image_Use[y2][x2+1]< thres&&Image_Use[y2][x2]>= thres)
        {
            //printf("Image_Use[%d][%d]=%d Image_Use[%d][%d]=%d\n",y2,x2-1,(int)Image_Use[y2][x2-1],y2,x2,(int)Image_Use[y2][x2]);
            findline_righthand_adaptive(Image_Use, block_size, clip_value, x2, y2, Image_Use_BLACK, &ipts1_num);
            break;//�ҵ��ұ��ߵ�һ���߽��
        }
    }
    //�ҳ�Ѱ�ұ��ߵ���Զ������
    X_min=120;
    if(X_minR<X_minL)
    {
        X_min=X_minR;
        for(int i=X_minL;i>X_minR-1;i--)
        {
            ImageSide[i][0]=14;
        }
    }
    else
    {
        X_min=X_minL;
        for(int i=X_minR;i>X_minL-1;i--)
        {
            ImageSide[i][1]=174;
        }
    }
//    if (Image_Use[y2][x2]>= thres)
//    {
//        findline_righthand_adaptive(Image_Use, block_size, clip_value, x2, y2, Image_Use_BLACK, &ipts1_num);
//    }
    //else ipts1_num = 0;

    //Ѱ������


}


const int dir_front[4][2]= {{0,  -1},{1,  0}, {0,  1},{-1, 0}};
const int dir_frontleft[4][2]= {{-1, -1},{1,  -1},{1,  1},{-1, 1}};
const int dir_frontright[4][2] = {{1,  -1},{1,  1},{-1, 1},{-1, -1}};
int L_pointX[2],L_pointY[2],R_pointX[2],R_pointY[2],L_point_num=0,R_point_num=0;
// �����Թ�Ѳ��
void findline_lefthand_adaptive(unsigned char Image_Use[IMAGEH][IMAGEW], int block_size , int clip_value, int x, int y, unsigned char Image_Use_BLACK[IMAGEH][IMAGEW], int *num)
{
    int half =block_size / 2;
    int  dir = 0, turn = 0;
    int point_num=0;
    //int X_temp,Y_temp;
    stepL = 0;
    point_num=0;
    while (stepL < *num && half < x && x < IMAGEW - half - 1 && half < y && y < IMAGEH - half - 1 && turn < 4)
    {
        //printf("x=%d y=%d\n",x,y);
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++)
        {
            for (int dx = -half; dx <= half; dx++)
            {
                local_thres += Image_Use[y+dy][x+dx];
            }
        }

        local_thres=local_thres/ (block_size * block_size);
        local_thres -= clip_value;
        //local_thres=135;
        //printf("local_thres=%d \n",local_thres);
        //int current_value = Image_Use[y][x];
        int front_value = Image_Use[ y + dir_front[dir][1]][ x + dir_front[dir][0]];
        int frontleft_value = Image_Use[y + dir_frontleft[dir][1]][x + dir_frontleft[dir][0]];
        //printf("\n dir=%d front_value=%d frontleft_value=%d local_thres=%d\n",dir,front_value,frontleft_value,local_thres);
        if (front_value < local_thres)
        {
            dir = (dir + 1) % 4;
            turn++;
        }
        else if (frontleft_value < local_thres)
        {
            x += dir_front[dir][0];
            y += dir_front[dir][1];
            //��¼���������
            if(dir_front[dir][1]==-1)
            {
                ImageSide[y][0]=(unsigned char)x;
                X_minL--;
            }
//            if(dir_front[dir][1]==1&&y<80&&y>20)
//            {
//                point_num++;
//            }
//            if(point_num>5)
//            {
//                Circle_flag_L=1;
//            }
            Image_Use_BLACK[y][x]=255;

            stepL++;
            turn = 0;
            //dir=0;
            //printf("ipts0[%d][0]=%d ipts0[%d][1]=%d \r\n ",step,pts[step][0],step,pts[step][1]);
        }
        else
        {
            x += dir_frontleft[dir][0];
            y += dir_frontleft[dir][1];
            //��¼���������
            if(dir_frontleft[dir][1]==-1)
            {
                ImageSide[y][0]=(unsigned char)x;
                X_minL--;
            }
            dir = (dir + 3) % 4;
//            pts[stepL][0] = x;
//            pts[stepL][1] = y;
            Image_Use_BLACK[y][x]=255;
            stepL++;
            turn = 0;
        }
    }
}

// �����Թ�Ѳ��
void findline_righthand_adaptive(unsigned char Image_Use[IMAGEH][IMAGEW], int block_size , int clip_value, int x, int y, unsigned char Image_Use_BLACK[IMAGEH][IMAGEW], int *num)
{
    int half = block_size / 2;
    int dir = 0, turn = 0;
    //int X_temp,Y_temp;
    stepR = 0;
    while (stepR < *num && half < x && x < IMAGEW - half-1 && half < y && y <IMAGEH -half- 1 && turn < 4)
    {
        int local_thres = 0;
        for (int dy = -half; dy <= half; dy++)
        {
            for (int dx = -half; dx <= half; dx++)
            {
                local_thres += Image_Use[y+dy][x+dx];
            }
        }
        local_thres=local_thres/ (block_size * block_size);//ƽ������ֵ
        local_thres -= clip_value;
//        local_thres=135;

        //int current_value = AT(img, x, y);      //��ǰ����ֵ
        int front_value = Image_Use[ y + dir_front[dir][1]][ x + dir_front[dir][0]];
        int frontright_value = Image_Use[y + dir_frontright[dir][1]][x + dir_frontright[dir][0]];

        if (front_value < local_thres)
        {  //�ж�ǰ������ֵ�Ƿ�С��ƽ������ֵ
            dir = (dir + 3) % 4;
            turn++;
        }
        else if (frontright_value < local_thres)
        {//�ж��ҷ�����ֵ�Ƿ�С��ƽ������ֵ
            x += dir_front[dir][0];   //С������һ������Ϊ�ҷ�����
            y += dir_front[dir][1];
            //��¼�ұ�������
            if(dir_front[dir][1]==-1)
            {
                ImageSide[y][1]=(unsigned char)x;
                X_minR--;
            }
            //��¼��������� �жϹյ�
//            if(dir_front[dir][1]==0&&dir_front[dir][0]==-1&&R_point_num!=0)
//            {
//                R_point_num++;
//            }
//            if(dir_front[dir][1]==0&&dir_front[dir][0]==-1&&R_point_num==0)
//            {
//                R_point_num=1;
//                X_temp=x;
//                Y_temp=y;
//            }
//            if(dir_front[dir][1]==0&&dir_front[dir][0]==-1&&R_point_num==35)
//            {
//                if((X_temp-x)<40&&(Y_temp-y)<3)
//                {
//                    R_pointX[0]=X_temp;
//                    R_pointY[0]=Y_temp;
//                }
//                else
//                {
//                    X_temp=0;
//                    Y_temp=0;
//                    R_point_num=0;
//                }
//            }
            //dir=0;
//            pts[stepR][0] = x;        //�����һ��������Ѱ����
//            pts[stepR][1] = y;
            Image_Use_BLACK[y][x]=255;
            stepR++;
            turn = 0;

        }
        else
        {
            x += dir_frontright[dir][0];  //��һ������Ϊǰ������
            y += dir_frontright[dir][1];
            //��¼�ұ�������
            if(dir_frontright[dir][1]==-1)
            {
                ImageSide[y][1]=(unsigned char)x;
                X_minR--;
            }
            dir = (dir + 1) % 4;
//            pts[stepR][0] = x;                           //�����һ��������Ѱ����
//            pts[stepR][1] = y;
            Image_Use_BLACK[y][x]=255;
            stepR++;
            turn = 0;

        }
    }
    //*num = step;                         //��¼��ǰ����������
}

float computeCurvature_L(uint8_t p1, uint8_t  p2, uint8_t p3)
{
    // ��������������
    float curvature = 0.0;

    // �������� v1 �� v2
    float v1x = (float)(ImageSide[p1][0]-ImageSide[p2][0]);
    //printf("v1x=%.1f\n",v1x);
    float v1y = (float)(p1 - p2);
    //printf("v1y=%.1f\n",v1y);
    float v2x = (float)(ImageSide[p3][0]-ImageSide[p2][0]);
    float v2y = (float)(p3 - p2);

    // �������������ģ��
    float crossProduct = fabs(v1x * v2y - v2x * v1y);
    //printf("crossProduct=%.1f\n",crossProduct);
    // �������� v1 �� v2 ��ģ��
    float v1Magnitude = sqrt(v1x * v1x + v1y * v1y);
    //printf("v1Magnitude=%.1f\n",v1Magnitude);
    float v2Magnitude = sqrt(v2x * v2x + v2y * v2y);
    //printf("v2Magnitude=%.1f\n",v2Magnitude);
    // ��������
    curvature = crossProduct / (v1Magnitude * v2Magnitude * (v1Magnitude + v2Magnitude))*10000;
    //printf("curvature=%.1f\n",curvature);

    return curvature;
}


//��ѹ��ȡ
void Voltage_Get(void)
{
    int ADCR=ADC_Read(ADC7);
    Voltage=ADCR*0.4464;
    Voltage=(vol_last*31+Voltage*1)/32;
    vol_last=Voltage;
}

//�������������
long int aacx_last,aacy_last,aacz_last;
long int gyrox_last,gyroy_last,gyroz_last;
void Get_aac(void)
{
    short gyrox,gyroy,gyroz;        //������ԭʼ����

    FX_21002_GetRaw(&gyrox, &gyroy, &gyroz);
//    gyrox_last=(gyrox*5+gyrox_last*10)/16;
//    gyroy_last=(gyroy*5+gyroy_last*10)/16;
//    gyroz_last=(gyroz*5+gyroz_last*10)/16;
//    gyrox_last=gyrox;
//    gyrox_last=gyroy;
//    gyrox_last=gyroz;
    if(gyrox > 100||gyrox <-100)
    {
        Anglex+=gyrox*62.5/1000*5/1000;   //5ms��һ�Σ�
    }
    if(gyroy > 100||gyroy <-100)
    {
        Angley+=gyroy*62.5/1000*5/1000;//�ǶȻ���
    }
    if(gyroz > 100||gyroz <-100)
    {
        Angularz_speed = (int)(gyroz*62.5/1000);//���ٶ�
        Anglez+=gyroz*62.5/1000*5/1000;//�ǶȻ���
    }
}

/***************�����ٶȵ�ͨ�˲�******************/
int Left_Low_Pass_Filter(short int speedL,int Feedback_Speed,long int Feedback_Speed_tmp)
{
    int Feedback_Speed_Now;
    int tmp=20;
    int tmp2=tmp/2;
    int tmp7;
    Feedback_Speed_Now=5.2051*speedL;             //���ֵ����ǰת��
    Feedback_Speed_tmp+=Feedback_Speed_Now-Feedback_Speed; //�ٶȲ����
    Feedback_Speed=Feedback_Speed_tmp/tmp;                 //ȡ1/20ʹ�ٶ�ƽ��
    tmp7=Feedback_Speed_tmp%tmp;
    if(tmp7>=tmp2)  Feedback_Speed+=1;                         //�ٶ�ֵ��������
    if(tmp7<=-tmp2) Feedback_Speed-=1;

    Feedback_SpeedL=Feedback_Speed;
    Feedback_Speed_tmpL=Feedback_Speed_tmp;

    return Feedback_Speed;                                 //�����˲�����ٶ�
}

/***************�����ٶȵ�ͨ�˲�******************/
int Right_Low_Pass_Filter(short int speedR,int Feedback_Speed,long int Feedback_Speed_tmp)
{
    int Feedback_Speed_Now;
    int tmp=20;
    int tmp2=tmp/2;
    int tmp7;
    Feedback_Speed_Now=5.5562*speedR;             //���ֵ����ǰת��
    Feedback_Speed_tmp+=Feedback_Speed_Now-Feedback_Speed; //�ٶȲ����
    Feedback_Speed=Feedback_Speed_tmp/tmp;                 //ȡ1/20ʹ�ٶ�ƽ��
    tmp7=Feedback_Speed_tmp%tmp;
    if(tmp7>=tmp2)  Feedback_Speed+=1;                         //�ٶ�ֵ��������
    if(tmp7<=-tmp2) Feedback_Speed-=1;

    Feedback_SpeedR=Feedback_Speed;
    Feedback_Speed_tmpR=Feedback_Speed_tmp;

    return Feedback_Speed;                                 //�����˲�����ٶ�
}

//***************************************************ɨ�躯��***************************************************************//
int qianzhan= 30;                         //ǰհԽС���Խ��ǰ40
void lkcongzhongjiansaomiao(void)
{
    //******************************************************************���ʼ���**************************************************************/
    int qulv_s_y=0,qulv_s_l=0;

    for(int y=68;y>=0;y--)
    {
      if(y>42&&y<68)//���ʽ����ж�
      {
       if((((ImageSide[y][0] + ImageSide[y][1])/2)-((ImageSide[y+1][0] + ImageSide[y+1][1])/2))>0)
        {
          qulv_jinduan_right++;
        }
       else if((((ImageSide[y][0] + ImageSide[y][1])/2)-((ImageSide[y+1][0] + ImageSide[y+1][1])/2))<0)
        {
          qulv_jinduan_left++;
        }
      }
      if(y<42&&y>=10)//����Զ���жϣ�Զ����һ��Ͳ�Ҫ��
      {
       if((((ImageSide[y][0] + ImageSide[y][1])/2)-((ImageSide[y+1][0] + ImageSide[y+1][1])/2))>0)
        {
          qulv_yuandaun_right++;
        }
       else if((((ImageSide[y][0] + ImageSide[y][1])/2)-((ImageSide[y+1][0] + ImageSide[y+1][1])/2))<0)
        {
          qulv_yuandaun_left++;
        }
      }
      if(y<=65&&y>10)//����ȫ���ж�
      {
        if((((ImageSide[y][0] + ImageSide[y][1])/2)-((ImageSide[y+1][0] + ImageSide[y+1][1])/2))>0)
        {
          qvlv_quanju_right++;
        }
        else if((((ImageSide[y][0] + ImageSide[y][1])/2)-((ImageSide[y+1][0] + ImageSide[y+1][1])/2))<0)
        {
          qvlv_quanju_left++;
        }
      }
      if(y<53)//Сsλ���жϣ�Сs�����ֱ�߳��
      {
          if((((ImageSide[y][0] + ImageSide[y][1])/2)-((ImageSide[y+2][0] + ImageSide[y+2][1])/2))>0)
        {
           qulv_s_y++;
        }
        else if((currentzhongjian[y]-currentzhongjian[y+2])<0)
        {
           qulv_s_l++;
        }
      }
      // *****************************************�ϵ�����*********************************************//
      if(y<46&&((abs)(ImageSide[y][1]-((ImageSide[y][0] + ImageSide[y][1])/2))<1||(abs)(ImageSide[y][0]-((ImageSide[y][0] + ImageSide[y][1])/2))<1))
       {
         duandian=y-25;
         fuduandian=y;
         if(y>14)
          {
            duandianshu=y-25;
            if(duandianshu>35)
              duandianshu=35;
          }
         if(y>qianzhan)
         {
           duandianshu1=y-qianzhan;
           if(duandianshu1>25)
             duandianshu1=25;
         }
       }
      else
       {
         duandian=0;
         duandianshu=0;
         duandianshu1=0;
       }
    }
    //********************************************************************���ʼ���*******************************************************************
      qvlv_quanju=abs(qvlv_quanju_right-qvlv_quanju_left);//����ȫ��
      qulv_jinduan=abs(qulv_jinduan_right-qulv_jinduan_left);//���ʽ���
      qulv_yuandaun=abs(qulv_yuandaun_right-qulv_yuandaun_left);//����Զ��
      qvlv_quanju_right=qvlv_quanju_left=qulv_jinduan_right=qulv_jinduan_left=qulv_yuandaun_right=qulv_yuandaun_left=0;
}

/*!
 *  @brief     ,·���ж�
 */
char dawan_flag,zhidao_ruwan_flag;
int i_add=0;
void lukuangudge(void)
{

    if(abs(g_sSteeringError)<=20&&qvlv_quanju<=12&&duandian<=2)
    {
      zhidao_flag++;
    }
    else
    {
      zhidao_flag=0;
    }

    //����
    if(qulv_yuandaun>25||abs(g_sSteeringError)>65)
    {
      start_V_L=370;
      dawan_flag=1;
    }
    //СS��
    else if(s_point==0&&qulv_s_y>6&&qulv_s_l>6&&zhidao_flag<18)
    {
       if(dawan_flag==1&&i_add<25)
       {
           start_V_L+=1;
           i_add++;
       }
       else
       {
           i_add=0;
           dawan_flag=0;
           start_V_L=400;
       }
       dawan_flag=0;
    }
    //С��
    else if(qulv_yuandaun>10&&qulv_yuandaun<=25&&abs(g_sSteeringError)<=65&&abs(g_sSteeringError)>=30)
    {
      if(dawan_flag==1&&i_add<25)
      {
          start_V_L=400;
          i_add++;
      }
      else
      {
          i_add=0;
          dawan_flag=0;
          start_V_L=400;
      }
    }
    //��ֱ��
    else if(zhidao_flag>10)
    {
      if(dawan_flag==1&&i_add<35)
      {
          start_V_L+=3;
          i_add++;
      }
      else
      {
          i_add=0;
          dawan_flag=0;
          start_V_L=500;
      }
      //dawan_flag=0;
    }
    else
    {
      dawan_flag=0;
      zhidao_ruwan_flag=0;
      start_V_L=400;
    }
//    //����ֱ�������
//   if(((Real_Speed1+Real_Speed2)/2)>(350)&&qulv_yuandaun>5&&qulv_jinduan<5)
//   {
//     zhidao_flag=0;
//     zhidao_ruwan_flag=1;
//     if(dawan_flag==1&&i_add<25)
//     {
//         start_V_L=400;
//         i_add++;
//     }
//     else
//     {
//         i_add=0;
//         dawan_flag=0;
//         start_V_L=400;
//     }
//   }
   //��·
   if(open_flag!=0)
   {
       //����
       if(abs(ALL_AD)>55)
       {
         start_V_L=320;
         dawan_flag=1;
       }
       //��ֱ��
       else if(abs(ALL_AD)<15)
       {
         if(dawan_flag==1&&i_add<15)
         {
             start_V_L+=5;
             i_add++;
         }
         else
         {
             i_add=0;
             dawan_flag=0;
             start_V_L=450;
         }
         //dawan_flag=0;
       }
       else
       {
           start_V_L=300;
       }
   }
   //�µ�
   if(slope_flag==1)
   {
       start_V_L=750;
   }
   if(slope_flag==2)
   {
       start_V_L=280;
   }
   //Բ��
   if(Circle_flag_R!=0||Circle_flag_L!=0)
   {
       start_V_L=360;
   }
}

/*!
 *  @brief      ���Իع鷽�̼���б�ʺ���
 *  @note
 */
int regression(int startline,int endline)
{
  if(endline>100)
   {
     endline=100;
   }
  int i;
  int sumX=0,sumY=0,avrX=0,avrY=0 ;
  int num=0,B_up1=0,B_up2=0,B_up,B_down;
  for(i=startline;i<=endline;i++)
   {
     num++;
     sumX+=i;
     sumY+=(ImageSide[i][0] + ImageSide[i][1])/2;
   }
  avrX=sumX/num;
  avrY=sumY/num;
  B_up=0;
  B_down=0;
  for(i=startline;i<=endline;i++)
   {
     B_up1=(int)((ImageSide[i][0] + ImageSide[i][1])/2-avrY);
     B_up2=i-avrX;
     B_up+=(int)(10*(B_up1*B_up2));
     B_up=B_up/100*100;
     B_down+=(int)(10*((i-avrX)*(i-avrX)));
   }
  if(B_down==0)
   {
      B_lk=0;
   }
  else
   {
     B_lk=B_up*16/B_down;//16
   }
  return B_lk;
}

// ƫ��ֵ���º���
int updateDeviation(int* filter, int filterSize)
{
    // ��ǰ�ƶ��˲���
    for (int i = filterSize - 1; i > 0; i--) {
        filter[i] = filter[i - 1];
    }

    // ���µ�ƫ��ֵ�����˲���
    //filter[0] = RoadGetSteeringError(ImageSide, ROAD_MAIN_ROW);//����ͷ��ȡƫ��;
    filter[0] = ALL_AD;
    // ����ƽ��ֵ
    double average = 0.0;
    for (int i = 0; i < filterSize; i++) {
        average += filter[i];
    }
    average /= filterSize;

    return (int)average;
}

void judge_Obstacle(void)
{
    //ʶ��·��
    if(rectangle_flag==1&&Infrared_Read()==0&&Obstacle_flag==0&&Anglex>0)
    {
//        ECPULSE_Obstacle_L=0;
//        ECPULSE_Obstacle_R=0;
        Obstacle_num=0;
        Obstacle_flag=1;
    }
//    //ͣ��
//    if(Obstacle_flag==1&&Obstacle_num<50)
//    {
//        ATOM_PWM_SetDuty(ATOMPWM5, 0, 17000);
//        ATOM_PWM_SetDuty(ATOMPWM4, 900, 17000);
//        ATOM_PWM_SetDuty(ATOMPWM7, 0, 17000);
//        ATOM_PWM_SetDuty(ATOMPWM6, 900, 17000);
//    }
//    if(Obstacle_flag==1&&Obstacle_num>50)
//    {
//        Obstacle_num=0;
//        Obstacle_flag=2;
//    }
    if(Obstacle_turn_flag==1)  //�ҹ�
    {
        //ת����
        if(Obstacle_flag==1&&Obstacle_num<80)
        {
            ATOM_PWM_SetDuty(ATOMPWM5, 6000, 17000);
            ATOM_PWM_SetDuty(ATOMPWM4, 0, 17000);
            ATOM_PWM_SetDuty(ATOMPWM7, 0, 17000);
            ATOM_PWM_SetDuty(ATOMPWM6, 2000, 17000);
        }
        if(Obstacle_flag==1&&Obstacle_num>=80)
        {
    //        ECPULSE_Obstacle_L=0;
    //        ECPULSE_Obstacle_R=0;
            Obstacle_num=0;
            Obstacle_flag=2;
        }
        //ת����
        if(Obstacle_flag==2&&Obstacle_num<130)
        {
            ATOM_PWM_SetDuty(ATOMPWM5, 0, 17000);
            ATOM_PWM_SetDuty(ATOMPWM4, 2500, 17000);
            ATOM_PWM_SetDuty(ATOMPWM7, 6000, 17000);
            ATOM_PWM_SetDuty(ATOMPWM6, 0, 17000);
        }
        if(Obstacle_flag==2&&Obstacle_num>130)
        {
            //ECPULSE_Obstacle_R=0;
            Obstacle_num=0;
            Obstacle_flag=3;
        }
        //����
        if(Obstacle_flag==3&&Obstacle_num<40)
        {
            ATOM_PWM_SetDuty(ATOMPWM5, 5000, 17000);
            ATOM_PWM_SetDuty(ATOMPWM4, 0, 17000);
            ATOM_PWM_SetDuty(ATOMPWM7, 0, 17000);
            ATOM_PWM_SetDuty(ATOMPWM6, 2000, 17000);
        }
        if(Obstacle_flag==3&&Obstacle_num>100)
        {
            Obstacle_num=0;
            Obstacle_flag=0;
        }
    }
    //��ձ���
    else if(Obstacle_turn_flag==0)
    {
        //ת����
        if(Obstacle_flag==1&&Obstacle_num<80)
        {
          ATOM_PWM_SetDuty(ATOMPWM5, 0, 17000);
          ATOM_PWM_SetDuty(ATOMPWM4, 2500, 17000);
          ATOM_PWM_SetDuty(ATOMPWM7, 6000, 17000);
          ATOM_PWM_SetDuty(ATOMPWM6, 0, 17000);
        }
        if(Obstacle_flag==1&&Obstacle_num>=80)
        {
    //        ECPULSE_Obstacle_L=0;
    //        ECPULSE_Obstacle_R=0;
            Obstacle_num=0;
            Obstacle_flag=2;
        }
        //ת����
        if(Obstacle_flag==2&&Obstacle_num<160)
        {
            ATOM_PWM_SetDuty(ATOMPWM5, 6000, 17000);
            ATOM_PWM_SetDuty(ATOMPWM4, 0, 17000);
            ATOM_PWM_SetDuty(ATOMPWM7, 0, 17000);
            ATOM_PWM_SetDuty(ATOMPWM6, 3000, 17000);
        }
        if(Obstacle_flag==2&&Obstacle_num>160)
        {
            //ECPULSE_Obstacle_R=0;
            Obstacle_num=0;
            Obstacle_flag=3;
        }
        //����
        if(Obstacle_flag==3&&Obstacle_num<40)
        {
            ATOM_PWM_SetDuty(ATOMPWM5, 0, 17000);
            ATOM_PWM_SetDuty(ATOMPWM4, 2500, 17000);
            ATOM_PWM_SetDuty(ATOMPWM7, 5000, 17000);
            ATOM_PWM_SetDuty(ATOMPWM6, 0, 17000);
        }
        if(Obstacle_flag==3&&Obstacle_num>100)
        {
            Obstacle_num=0;
            Obstacle_flag=0;
            start_V_L=250;
        }
    }
}

void judge_open(void)
{
    //��·����
//
//        if(L_pointX[0]!=0&&L_pointY[0]!=0&&R_pointX[0]!=0&&R_pointY[0]!=0&&abs(L_pointY[0]-R_pointY[0])<10&&R_pointY[0]>30&&L_pointY[0]>30)
//        {
//            open_flag=1;
//        }
    //��ʱһ��ʱ�������·
    if(Obstacle_flag==0&&open_flag==1&&Obstacle_num>200)
    {
        Obstacle_num=0;
        open_flag=2;
    }
    //�ж϶�·
    else if(rectangle_flag==1)//X_min>25������������߶�·
    {
        if(Infrared_Read()==1)
        {
            open_flag=1;
            start_V_L=450;
            Obstacle_num=0;
        }
        else
        {
            open_flag=0;
            Obstacle_num=0;
        }
    }
    //�����·��ʼ�жϳ���·ʱ��
    else if(open_flag==2)
    {
        if(Obstacle_num>50)
        {
           Obstacle_num=0;
           if(X_min<50)    //����·�ж�
           {
               open_flag=0;
           }
        }
    }
}

void judge_slope(void)
{
    if(Infrared_Read()==0&&Anglex<0)
    {
        Obstacle_num=0;
        slope_flag=1;
    }
    if(slope_flag==1&&Anglex>15)
    {
        Obstacle_num=0;
         slope_flag=2;
    }
    if(slope_flag==1&&Obstacle_num>400)
    {
        Obstacle_num=0;
        slope_flag=0;
    }
    if(slope_flag==2&&Obstacle_num>100)
    {
        Obstacle_num=0;
        slope_flag=0;
    }
}

void judge_Park(void)
{
    int j=40; //ɨ�����������
    turn_num=0;
    if(X_min<15)
    {
        for(int i=ImageSide[j][0];i<=ImageSide[j][1];i++)
        {
            if(Image_Data[j][i]<thres&&turn_flag==1)
            {
                turn_flag=0;
                turn_num++;
            }
            if(Image_Data[j][i]>=thres&&turn_flag==0)
            {
                turn_flag=1;
                turn_num++;
            }
        }
        //�ж����ҳ���  һ�߶���һ��ֱ��
        if(motor_turn_flag==1)
        {
            if(turn_num>6
//         //����
//         &&ImageSide[j][1]==173&&ImageSide[j+1][1]==173&&ImageSide[j+2][1]==173
//         &&ImageSide[j+3][1]==173&&ImageSide[j+4][1]==173&&ImageSide[j+5][1]==173
//         &&ImageSide[j+6][1]==173&&ImageSide[j+7][1]==173&&ImageSide[j+8][1]==173
//         &&ImageSide[j+9][1]==173&&ImageSide[j+10][1]==173&&ImageSide[j+11][1]==173
             //ֱ��
             &&ImageSide[j][0]>ImageSide[j+1][0]&&ImageSide[j+1][0]>ImageSide[j+2][0]&&ImageSide[j+2][0]>ImageSide[j+3][0]
             //&&ImageSide[j+3][0]>ImageSide[j+4][0]&&ImageSide[j+4][0]>ImageSide[j+5][0]&&ImageSide[j+5][0]>ImageSide[j+6][0]
             )
            {
                Park_flag=1;  //�ҳ���
                Anglez=0;   //�����ǻ�����0
                turn_num=0;
                start_V_L=200;//����
            }
        }
        if(motor_turn_flag==0)
        {
            if(turn_num>6
      //        //����
      //        &&ImageSide[j][0]==15&&ImageSide[j+1][0]==15&&ImageSide[j+2][0]==15
      //        &&ImageSide[j+3][0]==15&&ImageSide[j+4][0]==15&&ImageSide[j+5][0]==15
      //        &&ImageSide[j+6][0]==15&&ImageSide[j+7][0]==15&&ImageSide[j+8][0]==15
      //        &&ImageSide[j+9][0]==15&&ImageSide[j+10][0]==15&&ImageSide[j+11][0]==15
              //ֱ��
              &&ImageSide[j][1]<ImageSide[j+1][1]&&ImageSide[j+1][1]<ImageSide[j+2][1]&&ImageSide[j+2][1]<ImageSide[j+3][1]
              //&&ImageSide[j+3][1]<ImageSide[j+4][1]&&ImageSide[j+4][1]<ImageSide[j+5][1]&&ImageSide[j+5][1]<ImageSide[j+6][1]
              )
              {
                  Park_flag=2;  //�󳵿�
                  Anglez=0;   //�����ǻ�����0
                  turn_num=0;
                  Obstacle_num=0;
                  start_V_L=200;//����
              }
          }
    }
    else
    {
        turn_num=0;
    }

    //��������
    if((Park_flag==1||Park_flag==2))
    {
        Park_flag=3;
        Obstacle_num=0;
    }
    if(motor_turn_flag==1)
    {
        if(Park_flag==3&&Obstacle_num>60&&abs(Anglez)<70)
        {
           ATOM_PWM_SetDuty(ATOMPWM5, 6000, 17000);
           ATOM_PWM_SetDuty(ATOMPWM4, 0, 17000);
           ATOM_PWM_SetDuty(ATOMPWM7, 0, 17000);
           ATOM_PWM_SetDuty(ATOMPWM6, 2300, 17000);
        }
    }
    if(motor_turn_flag==0)
    {
        if(Park_flag==3&&abs(Anglez)<70)
        {
           ATOM_PWM_SetDuty(ATOMPWM5, 0, 17000);
           ATOM_PWM_SetDuty(ATOMPWM4, 2500, 17000);
           ATOM_PWM_SetDuty(ATOMPWM7, 6000, 17000);
           ATOM_PWM_SetDuty(ATOMPWM6, 0, 17000);
        }
    }
    //��ǽ���  ͣ��
    if(Park_flag==3&&abs(Anglez)>=70)
    {
        Obstacle_num=0;
        Park_flag=4;  //��תͣ����־
        motor_flag=0;
        start_V_L=0;
    }
    //��ֹ����
    if(Park_flag==4&&Obstacle_num<5)
    {
        ATOM_PWM_SetDuty(ATOMPWM5, 0, 17000);
        ATOM_PWM_SetDuty(ATOMPWM4, 500, 17000);
        ATOM_PWM_SetDuty(ATOMPWM7, 0, 17000);
        ATOM_PWM_SetDuty(ATOMPWM6, 500, 17000);
    }
    //ͣ�����
    if(Park_flag==4&&Obstacle_num<5)
    {
        start_V_L=0;
        Obstacle_num=0;
        Park_flag=0;
        motor_flag=0;
    }
}

extern float Cirle_PWM;
void judge_Circle(void)
{
    //�ж�һ��Բ��һ��ֱ��
    if(Circle_flag_L==0&&Circle_flag_R==0)
    {
        for(int i=40;i<60;i++)
        {
            //��Բ���ж�
            if(
//             ImageSide[i][0]>ImageSide[i+1][0]&&ImageSide[i+1][0]>ImageSide[i+2][0]&&ImageSide[i+2][0]>ImageSide[i+3][0]
//             &&ImageSide[i+3][0]>ImageSide[i+4][0]&&ImageSide[i+4][0]>ImageSide[i+5][0]&&ImageSide[i+5][0]>ImageSide[i+6][0]
//             &&ImageSide[i+6][0]>=ImageSide[i+7][0]&&ImageSide[i+7][0]>=ImageSide[i+8][0]&&ImageSide[i+8][0]>=ImageSide[i+9][0]
//             //&&ImageSide[i][i+18]>ImageSide[i+19][0]&&ImageSide[i+19][0]>ImageSide[i+20][0]&&ImageSide[i+20][0]>ImageSide[i+21][0]
//             &&ImageSide[i][1]<ImageSide[i+2][1]&&ImageSide[i+2][1]<ImageSide[i+4][1]&&ImageSide[i+4][1]<ImageSide[i+6][1]
//             &&ImageSide[i+6][1]<ImageSide[i+8][1]&&ImageSide[i+8][1]<ImageSide[i+10][1]&&ImageSide[i+10][1]<ImageSide[i+12][1]
//             &&MagneticField>250
             MagneticField>Circle_YuZhi
            )
            {
                Circle_flag_L=1;//ȷ������Բ��
                Obstacle_num=0;
            }
            //��Բ���ж�
            if(
//             ImageSide[i][1]<ImageSide[i+1][1]&&ImageSide[i+1][1]<ImageSide[i+2][1]&&ImageSide[i+2][1]<ImageSide[i+3][1]
//             &&ImageSide[i+3][1]<ImageSide[i+4][1]&&ImageSide[i+4][1]<ImageSide[i+5][1]&&ImageSide[i+5][1]<ImageSide[i+6][1]
//             &&ImageSide[i][1]<=ImageSide[i+7][1]&&ImageSide[i+7][1]<=ImageSide[i+8][1]&&ImageSide[i+8][1]<=ImageSide[i+9][1]
//             //&&ImageSide[i][i+18]>ImageSide[i+19][0]&&ImageSide[i+19][0]>ImageSide[i+20][0]&&ImageSide[i+20][0]>ImageSide[i+21][0]
//             &&ImageSide[i][0]>ImageSide[i+2][0]&&ImageSide[i+2][0]>ImageSide[i+4][0]&&ImageSide[i+4][0]>ImageSide[i+6][0]
//             &&ImageSide[i+6][0]>ImageSide[i+8][0]&&ImageSide[i+8][0]>ImageSide[i+10][0]&&ImageSide[i+10][0]>ImageSide[i+12][0]
//             &&MagneticField>250
             MagneticField>Circle_YuZhi
            )
            {
                Circle_flag_R=1;//ȷ������Բ��
                Obstacle_num=0;
            }
        }
    }

    //�ж�һ��ֱ��һ�߶���
    //��Բ��
    if(Circle_flag_L==1&&Obstacle_num>Circle_Yanshi&&Obstacle_num<100)
    {
        for(int j=40;j<60;j++)
        {
            if(ImageSide[j][0]==15&&ImageSide[j+1][0]==15&&ImageSide[j+2][0]==15
             &&ImageSide[j+3][0]==15&&ImageSide[j+4][0]==15&&ImageSide[j+5][0]==15
             &&ImageSide[j+6][0]==15&&ImageSide[j+7][0]==15&&ImageSide[j+8][0]==15
             &&ImageSide[j+9][0]==15&&ImageSide[j+10][0]==15&&ImageSide[j+11][0]==15
             &&ImageSide[j+12][0]==15&&ImageSide[j+13][0]==15&&ImageSide[j+14][0]==15

             &&ImageSide[j][1]<ImageSide[j+1][1]&&ImageSide[j+1][1]<ImageSide[j+2][1]&&ImageSide[j+2][1]<ImageSide[j+3][1]
             &&ImageSide[j+3][1]<ImageSide[j+4][1]&&ImageSide[j+4][1]<ImageSide[j+5][1]&&ImageSide[j+5][1]<ImageSide[j+6][1]
             &&ImageSide[j+6][1]<ImageSide[j+7][1]&&ImageSide[j+7][1]<ImageSide[j+8][1]&&ImageSide[j+8][1]<ImageSide[j+9][1]
             &&MagneticField>5000
             //&&ADC_Value[0]>=ADC_Value[3]
               )
            {
                Circle_flag_L=2;//ȷ�����λ��
                Circle_flag_R=0;
                Obstacle_num=0;
                break;
            }
        }
    }
    if(Circle_flag_L==1&&Obstacle_num<100)
    {
        if(MagneticField<5000)
        {
            Obstacle_num=0;
            Circle_flag_L=0;
            Circle_flag_R=0;
        }
    }
    else if(Circle_flag_L==1&&Obstacle_num>100)
    {
        Obstacle_num=0;
        Circle_flag_L=0;
        Circle_flag_R=0;
    }

    //�󻷴��
    if(Circle_flag_L==2&&Obstacle_num<60&&motor_flag==2)
    {
        ATOM_PWM_SetDuty(ATOMPWM5, 0, 17000);
        ATOM_PWM_SetDuty(ATOMPWM4, 3000, 17000);
        ATOM_PWM_SetDuty(ATOMPWM7, 6000, 17000);
        ATOM_PWM_SetDuty(ATOMPWM6, 0, 17000);
    }
    else if(Circle_flag_L==2&&Obstacle_num>60) //������
    {
        Obstacle_num=0;
        Anglez=0;   //�����ǻ�����0
        Circle_flag_L=3;
        Circle_flag_R=0;
    }
    //����һ��ʱ���رտ��� ��ǳ���
    if(Circle_flag_L==3&&Obstacle_num>30&&abs(Anglez)>210)
    {
        Obstacle_num=0;
        Circle_flag_L=4;
        Circle_flag_R=0;
    }

    if(Circle_flag_L==4&&Obstacle_num<60&&motor_flag==2)
    {
        ATOM_PWM_SetDuty(ATOMPWM5, 0, 17000);
        ATOM_PWM_SetDuty(ATOMPWM4, 2000, 17000);
        ATOM_PWM_SetDuty(ATOMPWM7, 5500, 17000);
        ATOM_PWM_SetDuty(ATOMPWM6, 0, 17000);
    }
    else if(Circle_flag_L==4&&Obstacle_num>60)
    {
        Obstacle_num=0;
        Circle_flag_L=5;
        Circle_flag_R=0;
    }
    //һ��ʱ������¼��Բ��
    if(Circle_flag_L==5&&Obstacle_num>600)
    {
        Obstacle_num=0;
        Circle_flag_L=0;
        Circle_flag_R=0;
    }

    //��Բ��
    if(Circle_flag_R==1&&Obstacle_num>Circle_Yanshi&&Obstacle_num<100)
    {
        for(int j=40;j<60;j++)
        {
            if(ImageSide[j][1]==173&&ImageSide[j+1][1]==173&&ImageSide[j+2][1]==173
             &&ImageSide[j+3][1]==173&&ImageSide[j+4][1]==173&&ImageSide[j+5][1]==173
             &&ImageSide[j+6][1]==173&&ImageSide[j+7][1]==173&&ImageSide[j+8][1]==173
             &&ImageSide[j+9][1]==173&&ImageSide[j+10][1]==173&&ImageSide[j+11][1]==173
             &&ImageSide[j+12][1]==173&&ImageSide[j+13][1]==173&&ImageSide[j+14][1]==173

             &&ImageSide[j][0]>ImageSide[j+1][0]&&ImageSide[j+1][0]>ImageSide[j+2][0]&&ImageSide[j+2][0]>ImageSide[j+3][0]
             &&ImageSide[j+3][0]>ImageSide[j+4][0]&&ImageSide[j+4][0]>ImageSide[j+5][0]&&ImageSide[j+5][0]>ImageSide[j+6][0]
             &&ImageSide[j+6][0]>ImageSide[j+7][0]&&ImageSide[j+7][0]>ImageSide[j+8][0]&&ImageSide[j+8][0]>ImageSide[j+9][0]
             &&MagneticField>5000
             //&&ADC_Value[0]<=ADC_Value[3]
               )
            {
                Circle_flag_L=0;
                Circle_flag_R=2;//ȷ�����λ��
                Obstacle_num=0;
                break;
            }
        }
    }
    if(Circle_flag_R==1&&Obstacle_num<100)
    {
        if(MagneticField<5000)
        {
            Obstacle_num=0;
            Circle_flag_L=0;
            Circle_flag_R=0;
        }
    }
    else if(Circle_flag_R==1&&Obstacle_num>100)
    {
        Obstacle_num=0;
        Circle_flag_L=0;
        Circle_flag_R=0;
    }

    //�һ����
    if(Circle_flag_R==2&&Obstacle_num<60&&motor_flag==2)
    {
        ATOM_PWM_SetDuty(ATOMPWM5, 6000, 17000);
        ATOM_PWM_SetDuty(ATOMPWM4, 0, 17000);
        ATOM_PWM_SetDuty(ATOMPWM7, 0, 17000);
        ATOM_PWM_SetDuty(ATOMPWM6, 2500, 17000);
    }
    else if(Circle_flag_R==2&&Obstacle_num>60) //������
    {
        Obstacle_num=0;
        Anglez=0;   //�����ǻ�����0
        Circle_flag_L=0;
        Circle_flag_R=3;
    }
    //����һ��ʱ���رտ��� ��ǳ���
    if(Circle_flag_R==3&&Obstacle_num>30&&abs(Anglez)>220)
    {
        Obstacle_num=0;
        Circle_flag_L=0;
        Circle_flag_R=4;
    }

    if(Circle_flag_R==4&&Obstacle_num<70)
    {
        ATOM_PWM_SetDuty(ATOMPWM5, 5000, 17000);
        ATOM_PWM_SetDuty(ATOMPWM4, 0, 17000);
        ATOM_PWM_SetDuty(ATOMPWM7, 0, 17000);
        ATOM_PWM_SetDuty(ATOMPWM6, 2000, 17000);
    }
    else if(Circle_flag_R==4&&Obstacle_num>70)
    {
        Obstacle_num=0;
        Circle_flag_L=0;
        Circle_flag_R=5;
    }

    //һ��ʱ������¼��Բ��
    if(Circle_flag_R==5&&Obstacle_num>600)
    {
        Obstacle_num=0;
        Circle_flag_L=0;
        Circle_flag_R=0;
    }
}

//ͼ��ѹ�� 120x160  80x120
void compressImage(unsigned char image[120][160], unsigned char image_min[80][120])
{
    int i, j;
    float scale_x = 160.0 / 120.0;
    float scale_y = 120.0 / 80.0;

    for (i = 0; i < 80; i++) {
        for (j = 0; j < 120; j++) {
            int src_x = scale_x * j;
            int src_y = scale_y * i;
            image_min[i][j] = image[src_y][src_x];//image_min[i][j]Ϊѹ�����ͼ������
        }
    }
}

//ʹ����ɱ���ɨ�跨
void findMaxDifference(unsigned char image[ROWS][COLS], unsigned char max[COLS / STEP_COL]) {
    int i, j, maxDiff;

    for (i = 0; i <= COLS / STEP_COL; i++) {
        int startRow =80;
        int endRow = 0;
        int maxRow1 = 80;
        int maxRow2 = 80;
        //maxDiff = abs((int)((image[startRow][i * STEP_COL] - image[startRow-1][i * STEP_COL ])/(image[startRow][i * STEP_COL] + image[startRow-1][i * STEP_COL]))*1000);
        maxDiff = abs((float)(image[startRow][i * STEP_COL] - image[startRow-1][i * STEP_COL ])/(float)(image[startRow][i * STEP_COL] + image[startRow-1][i * STEP_COL ])*1000);

        for (j = startRow-1; j > endRow; j--) {
            int diff = abs((float)(image[j][i * STEP_COL] - image[j-1][i * STEP_COL ])/(float)(image[j][i * STEP_COL] + image[j-1][i * STEP_COL ])*1000);
            //int diff = abs(image[j][i * STEP_COL] - image[j-1][i * STEP_COL ]);
            //printf("image[%d][%d]=%d\n",(int)j,(int)i * STEP_COL,(int)image[j][i * STEP_COL]);
            //printf("Row=%d ,COL=%d ,diff=%d \n",j,i * STEP_COL,diff);
            if (diff>yuzhi&&image[j][i * STEP_COL]>125&&image[j+1][i * STEP_COL]>125&&image[75][i * STEP_COL]>125&&image[70][i * STEP_COL]>125)
            {
                //maxDiff = diff;
                maxRow1 = j;
                maxRow2 = j - 1;
                //printf("Row=%d ,COL=%d ,diff=%d \n",j,i * STEP_COL,diff);
                break;
            }
        }
        max[i] =(unsigned char) maxRow1;//���0��24������
        //printf(" max[%d]=%d\n",i ,(int)maxRow1);
    }
}

void find_sideimage(unsigned char image[80][120], unsigned char max[COLS / STEP_COL],uint8_t imageOut[LCDH][2])
{
    int min = max[0];  // �����һ��Ԫ��Ϊ��Сֵ

    int j=0;

    for (int x = 1; x < COLS / STEP_COL; x++) {
        if (max[x] < min) {
            min = max[x];  // ������С��ֵ
            min_tag=x;
        }
    }

    //printf("min=%d,min_tag=%d\n",(int)min,(int)min_tag);
    int maxDiff_L,maxDiff_R;

    for (int i = 80; i >=0 ; i--) {
        int startCOL = min_tag*STEP_COL;
        //int startCOL = 60;
        int endRow_L = 0;
        int endRow_R = 120;


        int maxCol1_L=startCOL;
        int maxCol2_L=startCOL;
        int maxCol1_R=startCOL;
        int maxCol2_R=startCOL;

        int last_maxCol1_L=0;

        maxDiff_L = abs((float)(image[i][startCOL] - image[i][startCOL-1])/(float)(image[i][startCOL] + image[i][startCOL-1])*1000);
        maxDiff_R = abs((float)(image[i][startCOL] - image[i][startCOL+1])/(float)(image[i][startCOL] + image[i][startCOL+1])*1000);
       //���������
        for (j = startCOL; j > endRow_L; j--) {
            int diff = abs((float)(image[i][j] - image[i][j-1])/(float)(image[i][j] + image[i][j-1])*1000);
            //int diff = abs(image[j][i * STEP_COL] - image[j-1][i * STEP_COL ]);
            //printf("image[%d][%d]=%d\n",(int)i,(int)j,(int)image[i][j]);
            //printf("Row=%d ,COL=%d ,diff=%d \n",i,j,diff);
            if (diff > maxDiff_L&& diff>yuzhi) {
                maxDiff_L = diff;
                maxCol1_L = j;
                maxCol2_L = j - 1;
                break;
            }
        }
        if(maxCol1_L==startCOL)
        {
            maxCol1_L=0;
        }


        imageOut[i][0]=(uint8_t)maxCol1_L;//iΪ��߽��������maxCol1_L����imageOut[i][0]Ϊ��߽��������

        last_maxCol1_L=maxCol1_L;

        //�����ұ���
        for (j = startCOL; j < endRow_R; j++) {
            int diff = abs((float)(image[i][j] - image[i][j+1])/(float)(image[i][j] + image[i][j+1])*1000);
            //int diff = abs(image[j][i * STEP_COL] - image[j-1][i * STEP_COL ]);
            //printf("image[%d][%d]=%d\n",(int)j,(int)i * STEP_COL,(int)image[j][i * STEP_COL]);
            //printf("Row=%d ,COL=%d ,diff=%d \n",j,i * STEP_COL,diff);
            if (diff > maxDiff_R&& diff>yuzhi) {
                maxDiff_R = diff;
                maxCol1_R = j;
                maxCol2_R = j + 1;
                break;
            }
        }
        if(maxCol1_R==startCOL)
        {
            maxCol1_R=120;
        }
        imageOut[i][1]=(uint8_t)maxCol1_R;//iΪ�ұ߽��������maxCol1_R����imageOut[i][0]Ϊ�ұ߽��������
    }
}
//ɨ���ϱ���
void find_upsideimage(unsigned char image[80][120],uint8_t imageOut[2][160])
{
    int maxDiff_up;

    for (int i = 0; i <120 ; i++)
    {
        int startCOL = 60;
        int EndCOL = 0;

        int minCol=startCOL;
        int last_minCol=0;

        maxDiff_up = abs((float)(image[startCOL][i] - image[startCOL-1][i])/(float)(image[startCOL][i] + image[startCOL-1][i])*1000);
       //�����ϱ���
        for (int j = startCOL; j > EndCOL; j--)
        {
            int diff = abs((float)(image[j][i] - image[j-1][i])/(float)(image[j][i] + image[j-1][i])*1000);
            if (diff > maxDiff_up&& diff>yuzhi)
            {
                maxDiff_up = diff;
                minCol = j;
                break;
            }
        }
        if(minCol==startCOL)
        {
            minCol=60;
        }
        imageOut[0][i]=(uint8_t)minCol;//iΪ��߽��������maxCol1_L����imageOut[i][0]Ϊ��߽��������
        //printf("UpdowmSide[0][%d]=%d\r\n ",i,imageOut[0][i]);
        last_minCol=minCol;
    }
}



//ʹ��sobel�㷨������������߽�
void find_track_boundaries(unsigned char Image_Use[IMG_HEIGHT][IMG_WIDTH], uint8_t ImageSide[IMG_HEIGHT][2])
{
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    int sumX = 0;
    int sumY = 0;


    for (int y = 0; y < IMG_HEIGHT; y++) {
        int left_boundary = 0;
        int right_boundary = 255;

        for (int x = 0; x < IMG_WIDTH; x++) {
            //printf("Image_Use[%d][%d]=%d",x,y,Image_Use[x][y]);
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (y + i >= 0 && y + i < IMG_HEIGHT && x + j >= 0 && x + j < IMG_WIDTH) {
                        sumX += Image_Use[y + i][x + j] * Gx[i + 1][j + 1];
                        sumY += Image_Use[y + i][x + j] * Gy[i + 1][j + 1];
                    }
                }
            }

            edge_strength = (int)sqrt(sumX * sumX + sumY * sumY);
            //printf("edge_strength=%d \n",edge_strength);
            if (edge_strength > threshold)
            {
                if (left_boundary == 0)
                {
                    left_boundary = x;
                    //printf("left_boundary_x=%d left_boundary_y=%d\n",x,y);
                }
                else if(right_boundary == 255)
                {
                    right_boundary = x;
                }
            }
        }

        ImageSide[y][0] = (uint8_t)left_boundary;
        ImageSide[y][1] = (uint8_t)right_boundary;
    }
}

//��˹�˲�
#define KERNEL_SIZE 5

void apply_gaussian_filter(unsigned char Image_Use[IMG_HEIGHT][IMG_WIDTH], unsigned char Filtered_Image[IMG_HEIGHT][IMG_WIDTH]) {
    double gaussian_kernel[KERNEL_SIZE][KERNEL_SIZE] = {
        {1.0 / 256, 4.0 / 256,  6.0 / 256,  4.0 / 256, 1.0 / 256},
        {4.0 / 256, 16.0 / 256, 24.0 / 256, 16.0 / 256, 4.0 / 256},
        {6.0 / 256, 24.0 / 256, 36.0 / 256, 24.0 / 256, 6.0 / 256},
        {4.0 / 256, 16.0 / 256, 24.0 / 256, 16.0 / 256, 4.0 / 256},
        {1.0 / 256, 4.0 / 256,  6.0 / 256,  4.0 / 256, 1.0 / 256}
    };

    int kernel_offset = KERNEL_SIZE / 2;

    for (int y = 0; y < IMG_HEIGHT; y++) {
        for (int x = 0; x < IMG_WIDTH; x++) {
            double filtered_pixel = 0.0;

            for (int ky = -kernel_offset; ky <= kernel_offset; ky++) {
                for (int kx = -kernel_offset; kx <= kernel_offset; kx++) {
                    int image_x = x + kx;
                    int image_y = y + ky;

                    if (image_x >= 0 && image_x < IMG_WIDTH && image_y >= 0 && image_y < IMG_HEIGHT) {
                        filtered_pixel += Image_Use[image_y][image_x] * gaussian_kernel[ky + kernel_offset][kx + kernel_offset];
                    }
                }
            }

            Filtered_Image[y][x] = (unsigned char)round(filtered_pixel);
        }
    }
}


/*
 * ������DataBuff�е�����
 * ���ؽ����õ�������
 */

float Get_Data(unsigned char *DataBuff)
{
    uint8_t data_Start_Num = 0; // ��¼����λ��ʼ�ĵط�
    uint8_t data_End_Num = 0; // ��¼����λ�����ĵط�
    uint8_t data_Num = 0; // ��¼����λ��
    uint8_t minus_Flag = 0; // �ж��ǲ��Ǹ���
    float data_return = 0; // �����õ�������


    for(uint8_t i=0;i<200;i++) // ���ҵȺź͸�̾�ŵ�λ��
    {
        if(DataBuff[i] == '=') data_Start_Num = i + 1; // +1��ֱ�Ӷ�λ��������ʼλ
        if(DataBuff[i] == '!')
        {
            data_End_Num = i - 1;
            break;
        }
    }
    if(DataBuff[data_Start_Num] == '-') // ����Ǹ���
    {
        data_Start_Num += 1; // ����һλ������λ
        minus_Flag = 1; // ����flag
    }
    data_Num = data_End_Num - data_Start_Num + 1;

    if(data_Num == 4) // ���ݹ�4λ
    {
        data_return = (DataBuff[data_Start_Num]-48)  + (DataBuff[data_Start_Num+2]-48)*0.1f +
                (DataBuff[data_Start_Num+3]-48)*0.01f;
    }
    else if(data_Num == 5) // ���ݹ�5λ
    {
        data_return = (DataBuff[data_Start_Num]-48)*10 + (DataBuff[data_Start_Num+1]-48) + (DataBuff[data_Start_Num+3]-48)*0.1f +
                (DataBuff[data_Start_Num+4]-48)*0.01f;
    }
    else if(data_Num == 6) // ���ݹ�6λ
    {
        data_return = (DataBuff[data_Start_Num]-48)*100 + (DataBuff[data_Start_Num+1]-48)*10 + (DataBuff[data_Start_Num+2]-48) +
                (DataBuff[data_Start_Num+4]-48)*0.1f + (DataBuff[data_Start_Num+5]-48)*0.01f;
    }
    if(minus_Flag == 1)  data_return = -data_return;
//    printf("data=%.2f\r\n",data_return);
    return data_return;
}

/*
 * ���ݴ�����Ϣ����PID����
 */
void USART_PID_Adjust(unsigned char *DataBuff)
{
    float data_Get = Get_Data(DataBuff); // ��Ž��յ�������

    //printf("data=%.2f\r\n",data_Get);

        if(DataBuff[0]=='L' && DataBuff[1]=='P') // ����KP
            LSpeed_PID.kp = data_Get;
        else if(DataBuff[0]=='L' && DataBuff[1]=='I') // ����KI
            LSpeed_PID.ki = data_Get;
        else if(DataBuff[0]=='L' && DataBuff[1]=='D') // ����KD
            LSpeed_PID.kd = data_Get;
        else if(DataBuff[0]=='R' && DataBuff[1]=='P') // ����KP
            RSpeed_PID.kp = data_Get;
        else if(DataBuff[0]=='R' && DataBuff[1]=='I') // ����KI
            RSpeed_PID.ki = data_Get;
        else if(DataBuff[0]=='R' && DataBuff[1]=='D') // ����KD
            RSpeed_PID.kd = data_Get;
        else if((DataBuff[0]=='L' && DataBuff[1]=='V') && DataBuff[2]=='e') //����Ŀ���ٶ�
            Target_Speed1 = data_Get;
        else if((DataBuff[0]=='R' && DataBuff[1]=='V') && DataBuff[2]=='s') //����Ŀ���ٶ�
            Target_Speed2 = data_Get;
//    }
//    else if(Motor_n == 0) // �ұߵ��
//    {
//        if(DataBuff[0]=='P' && DataBuff[1]=='1') // λ�û�P
//            pid_r_position.kp = data_Get;
//        else if(DataBuff[0]=='I' && DataBuff[1]=='1') // λ�û�I
//            pid_r_position.ki = data_Get;
//        else if(DataBuff[0]=='D' && DataBuff[1]=='1') // λ�û�D
//            pid_r_position.kd = data_Get;
//        else if(DataBuff[0]=='P' && DataBuff[1]=='2') // �ٶȻ�P
//            pid_r_speed.kp = data_Get;
//        else if(DataBuff[0]=='I' && DataBuff[1]=='2') // �ٶȻ�I
//            pid_r_speed.ki = data_Get;
//        else if(DataBuff[0]=='D' && DataBuff[1]=='2') // �ٶȻ�D
//            pid_r_speed.kd = data_Get;
//        else if((DataBuff[0]=='S' && DataBuff[1]=='p') && DataBuff[2]=='e') //Ŀ���ٶ�
//            R_Target_Speed = data_Get;
//        else if((DataBuff[0]=='P' && DataBuff[1]=='o') && DataBuff[2]=='s') //Ŀ��λ��
//            R_Target_Position = data_Get;
//    }
}
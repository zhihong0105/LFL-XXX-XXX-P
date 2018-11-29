/******************** (C) COPYRIGHT 2013 HID ******************************
* File Name 	: System.c
* Author 		  : ZhihongWang
* Version 		: V0.0.1
* Date 			  : 2013-12-09
* Description : 
*******************************************************************************/
#ifndef __SYSTEM_INITIALIZE_H__
#define __SYSTEM_INITIALIZE_H__
//=========================================
//
#define  USE_HSE
//#define  PA6_COMP1_OUTPUT   1
//#define   CF_CF1_OUTPUT        1
//#define  CHANGE_FRQ            1
#define  DIM_MODE             1     /* 1:PWM  2:digital potentiometer */
#define  FEEDBACK             1     /* 1����������������ڻ�׼��2����׼�̶� */

#define  REGULATE_FREQUENCY_MODER      2 /* 1:����Ƶ�ʵļ����PWMƵ�ʸı䣻2���̶�����Ƶ�ʵ�ʱ����������СƵ���㣩*/
#define  MIN_FREQUENCY                 2 /* 1:��СƵ��20kHz  2:��СƵ��55kHz  */
/* ========================================= */
//
#define  FCY                    48000000	   /* freq of system */
#define  DIV_N                  (PWM2_FREQ/3.333)     

/* TIM3����ʱ��1��Ƶ */
#define  TIM3_DIV               1
#define  PWM3_FREQ              4000//2000          /* freq of time3 */
#define  TIM3_PERIOD            ((unsigned int)(FCY/PWM3_FREQ))
/* TIM6����ʱ�Ӳ���Ƶ */
#define  PWM6_FREQ              1000         /* freq of time6 */
#define  TIM6_PERIOD            ((unsigned int)(FCY/PWM6_FREQ))
/* TIM1���� */ 
#define	 PWM1_FREQ_LOW			 24000   //
#define  PWM1_FREQ_SKIP_OFF		(PWM1_FREQ_HIGH - 5000)
#define  PWM1_FREQ_SKIP_ON 		(PWM1_FREQ_SKIP_OFF - 5000)
#define  PWM1_FREQ_HIGH			 180000  //
#define  TIM1_PERIOD_SHORT  	((unsigned int)(FCY/PWM1_FREQ_HIGH))
#define  TIM1_PERIOD_SKIP_OFF   ((unsigned int)(FCY/PWM1_FREQ_SKIP_OFF))
#define  TIM1_PERIOD_SKIP_ON    ((unsigned int)(FCY/PWM1_FREQ_SKIP_ON))
#define  TIM1_PERIOD_LONG  	    ((unsigned int)(FCY/PWM1_FREQ_LOW))
#define  FRQ_TO_PERIOD(x)       ((unsigned int)((FCY)/(x)))
 
//#define  PWM1_FREQ_CONST        60000
//#define  TIM1_PERIOD_CONST      ((unsigned int)(FCY/PWM1_FREQ_CONST/2))
//#define  VREF                   3.0//1.25
//#define  AD_VREF                (VREF/3.3 * 4096)

/* ���ֵ�λ�� */
#define  RATE_VREF              4.0          /* �����ʱ��Ӧ�Ĳο���ѹֵ */
#define  MCP_FULL_VALUE         10000        /* ���ֵ�λ������ֵ */
#define  RES_UP                 2000         /* �����ֵ�λ���������ϵ���ֵ */
#define  VOLTAGE_MCP            (5.0*MCP_FULL_VALUE/(MCP_FULL_VALUE + RES_UP)) /* ���ֵ�λ�����˵ĵ�ѹ */
#define  DIM_TO_DATA(x)         ((x)*(RATE_VREF/200.0*255.0/VOLTAGE_MCP))      /* ����ֵת��Ϊ�䵽��λ����ֵ */


/* TIM2����ʱ�Ӳ���Ƶ */
#if REGULATE_FREQUENCY_MODER == 1
  #define  PWM2FREQ             180000//(13653 * 4) //40000   /* freq of time15 */
#elif REGULATE_FREQUENCY_MODER == 2
  #define  PWM2_FREQ             55000//60000//(13653 * 4) //40000   /* freq of time15 */
#endif
#define  TIM2_PERIOD           ((unsigned int)(FCY/PWM2_FREQ))

/* ADC */
#define  ADC1_DR_ADR            0x40012440
#define  SUM_OF_CHANNEL         3    /* Num of ADC Channel */


void IO_Init(void);
void RCC_Configuration(void);
void NVIC_Configuration(void);
void TIM_Init(void);
void FunADC_Init(void);
void UART_Init(void);
void WatvhDog_inti(void);
void FunComp_Init(void);
void GPIO_Interrupt(void);

extern unsigned short int g_uiPWM_Pulse;
extern unsigned char      g_ucReciveDelay;
extern unsigned char      g_ucFlagReciveDealWith;
extern unsigned long      g_ulPowerTemp;
extern unsigned short int g_uiADC_ConvertedValue[];
#endif

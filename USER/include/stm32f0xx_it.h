/**
  ******************************************************************************
  * @file    RCC/stm32f0xx_it.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */ 

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F0XX_IT_H
#define __STM32F0XX_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "PidControl.h"
#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void NMI_Handler(void);
void HardFault_Handler(void);
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);
void FrqInit(void);
	 
extern char               g_cFlagMeasureComplete;
extern char               g_cFlagADComplete2;
extern unsigned short int g_uiFeedBack;
extern unsigned short int g_uiCurrentIn;
extern unsigned short int g_uiVoltageIn;
extern unsigned short int g_uiCurrentOutAdcValue;
extern unsigned short int g_uiVoltageOutAdcValue;
extern unsigned short int g_uiVinAC;
extern unsigned short int g_uiPowerLimit;
extern volatile unsigned char      g_ucReciveData[];
extern volatile unsigned char      g_ucReciveCnt;
extern volatile ScmSysParam        g_sSysParam; 
extern volatile char               g_cUART2ReciveComplete;
extern unsigned long      g_ulVoltageIn;
extern unsigned long      g_ulCurrentIn;
extern unsigned long      g_ulPowerIn;
extern unsigned short int g_uiFaultDelay;
extern unsigned char      g_ucTime1s;
extern unsigned short int g_uiTemperature;
extern unsigned char      g_ucFlagAdcEnd;
extern unsigned short int g_uiDelayCnt;
extern volatile unsigned char g_ucMCP41010SendEn;
extern unsigned char      g_ucMCP41010SendData;
extern volatile unsigned char g_ucCSE7759State;
extern volatile unsigned short int g_uiVoltageCnt;
extern volatile unsigned short int g_uiCurrentCnt;
extern volatile unsigned short int g_uiPowerCnt;
extern volatile unsigned short int g_uiVoltageTime;
extern volatile unsigned short int g_uiCurrentTime;
extern volatile unsigned short int g_uiPowerTime;
extern volatile unsigned short int g_uiSelDelay;
extern volatile unsigned char      g_ucTestState;
extern volatile unsigned short int g_uiDimInterval;
extern volatile unsigned char      g_ucBreak;
extern volatile unsigned char      g_ucPowerState;
extern volatile unsigned short int g_uiUnderVolRestartDelay;
extern volatile unsigned short int g_uiReduceCnt;
extern volatile unsigned char      g_ucAdcCnt;
extern volatile unsigned short int g_uiReduceDelayCnt;
extern volatile unsigned short int g_uiRunCnt;
#ifndef  PWM_STE_EN
    #error "Please define PWM_STE_EN!"
#elif    PWM_STE_EN == 1
extern volatile unsigned short int g_uiArrTemp;
extern volatile unsigned short int g_uiFqrTemp;
#endif

#if VOL_OUT_OVER_PROTECT_EN == 1
/* 开路保护延时 */
extern volatile unsigned short int g_uiVolOutOverProtectDelayCnt;
#endif

#define  AN0           g_uiFeedBack
#define  AN1           g_uiCurrentIn
#define  AN2           g_uiVoltageIn
#define  AN3           g_uiTemperature
#define  AN4           g_uiVbus
#define  AN5           g_uiPowerLimit

#define  CURRENT_IN_OFFSET    372

/* 数字电位器SCK的开始延时和关断延时 */
#define SCK_LOW_TIME   30
#define SCK_HIGE_TIME  SCK_LOW_TIME

/* CSE7759测量时间 */
//#define CSE7759_TIME   1000

#ifdef __cplusplus
}
#endif

#endif /* __STM32F0XX_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/******************** (C) COPYRIGHT 2013 HID ******************************
* File Name 	: main.c
* Author 		  : ZhihongWang
* Version 		: V0.0.1
* Date 			  : 2013-12-09
* Description : 
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "main.h"
#include "System_Initialize.h"
#include "delay.h"
#include "Temperature_Test.h"
#include "MyMath.h"
#include "FlashOperate.h"
#include "PidControl.h"
#include "stm32f0xx_it.h"
#include "Repair.h"
/* global variable definition ------------------------------------------------*/
unsigned short int          g_uiPWM_Pulse;
unsigned char               g_ucReciveDelay;
unsigned char               g_ucFlagReciveDealWith;
unsigned long               g_ulPowerTemp;
unsigned char               g_ucSendData[SEND_MAX];
unsigned char               g_ucSendSum;
volatile ScmSysParam        g_sSysParam; 
Flag			            g_sFlags;
char                        g_cFlagMeasureComplete = 0;
char                        g_cFlagADComplete2;
unsigned short int          g_uiFeedBack;
unsigned short int          g_uiCurrentIn;
unsigned short int          g_uiVoltageIn;
unsigned short int          g_uiCurrentOutAdcValue;
unsigned short int          g_uiVoltageOutAdcValue;
unsigned short int          g_uiCurrentOut_mA;
unsigned short int          g_uiVoltageOut_100mV;
unsigned short int          g_uiVinAC;
unsigned short int          g_uiPowerLimit;
unsigned short int          g_uiADC_ConvertedValue[8];
volatile unsigned char      g_ucReciveData[RECIVE_MAX];
volatile unsigned char      g_ucReciveCnt;
volatile char               g_cUART2ReciveComplete;
unsigned long               g_ulVoltageIn;
unsigned long               g_ulCurrentIn;
unsigned long               g_ulPowerIn;
unsigned short int          g_uiFaultDelay;
unsigned short int          g_uiTemperature = 0;
unsigned char               g_ucFlagAdcEnd = 0;
unsigned short int          g_uiDelayCnt = 0;
#if REF_ADJUST_MODE == 2
ScmPiReg                    g_sPiParmRefVol;
#endif
volatile unsigned char      g_ucMCP41010SendEn = 0;
unsigned char               g_ucMCP41010SendData = 0;
volatile unsigned char      g_ucCSE7759State = ALL_READY;
volatile unsigned short int g_uiVoltageCnt = 0;
volatile unsigned short int g_uiCurrentCnt = 0;
volatile unsigned short int g_uiPowerCnt = 0;
volatile unsigned short int g_uiVoltageTime;
volatile unsigned short int g_uiCurrentTime;
volatile unsigned short int g_uiPowerTime;

volatile unsigned short int g_uiVoltageCntCheck;
volatile unsigned short int g_uiCurrentCntCheck;
volatile unsigned short int g_uiPowerCntCheck;
volatile unsigned long      g_uiVoltageTimeCheck;
volatile unsigned long      g_uiCurrentTimeCheck;
volatile unsigned long      g_uiPowerTimeCheck;

volatile unsigned short int g_uiSelDelay = 0;
volatile unsigned char      g_ucTestState = DO_NOTHING;
volatile unsigned short int g_uiTrigWaitingTime;
volatile unsigned short int g_uiDimInterval = 0;
volatile unsigned char      g_ucBreak = 0;
volatile unsigned char      g_ucPowerState = 0;            /* �ñ������ڲ��Ե�ʱ��ʹ�ã����ڸ���״̬ */
volatile unsigned short int g_uiUnderVolRestartDelay = 0;  /* �ñ�������Ƿѹ�ָ���ʱ */
volatile unsigned short int g_uiReduceCnt;                 /* ����ı����� */
volatile unsigned short int g_uiReduceDelayCnt = 0;        /* �ϵ���ʱ����������� */
volatile unsigned char      g_ucAdcCnt = 0;                /* ADC�������� */
volatile unsigned short int g_uiRunCnt = 0;                /* ��������ָʾ�Ʒ�תʱ�� */
volatile unsigned char      g_ucMainCycle = 0;             /* ��ѭ������ */
volatile unsigned short int g_uiTemp = 0;
volatile unsigned short int g_uiTryToRestoreTime = RESTORT_TIME; //
volatile unsigned short int g_uiRepairingTimer = 0;   /* ����У������ʱ��ʱ */
volatile unsigned char      g_ucRepairing = 0; //У����־��0��û�н���У����1�����ڽ���У��
volatile unsigned char      g_ucCSE7759Flag = 0; 
#ifndef GRT_V15
    #error "Please define GRT_V15!"
#elif    PROTOCOL == GRT_V15
volatile unsigned long      g_ulRunTime = 0;               /* ���е�ʱ�䣺unit��hour */
volatile unsigned long      g_ulRunTimeTemp; 
volatile unsigned char      g_ucRunTimeUpdata = 0;         /* ����ʱ����±�־��0���޸��£�1���и��� */
volatile unsigned char      g_ucRepairEn;                  /* ��˥����ʹ��λ��0����ʹ�ܣ� 1��ʹ�� */
#endif
#ifndef LED_MODULE_OPEN_TEST_EN
  #error "Plese define LED_MODULE_OPEN_TEST_EN!"
#elif LED_MODULE_OPEN_TEST_EN == 1
  volatile unsigned short int  g_uiLedModuleOpenTime;
  static unsigned short int fg_uiVolOutLast_100mV;
  static unsigned short int fg_uiLedOnTimeDly = 0;    //����֮����ʱһ��ʱ�����ѹ�ȶ�֮���ټ�⡣
  static unsigned short int fg_uiLedOnTimeHis = 0;    //����֮�󣬴���ѹ�ȶ�֮�����ʷ��ѹ�Աȡ�
  static unsigned short int fg_uiLedVolHis;           //��������ʷ�����ѹ���ݡ�
  ScmLedData  sLedHis; 
  static unsigned char      fg_ucDimValueLast;  
  static unsigned char      fg_ucOpenLoadFlag = 0;    //������־
  static unsigned char      fg_ucDaGeFlag = 0;
  static unsigned char      fg_ucDaGeCnt = 0;
#endif
#if VOL_OUT_OVER_PROTECT_EN == 1
/* ��·������ʱ */
volatile unsigned short int g_uiVolOutOverProtectDelayCnt = 0;
#endif
#if VOL_IN_PROTECT_EN == 1  //
volatile unsigned short int g_uiVolInOverCnt;           //�����ѹ������ʱ
#endif
#ifndef  PWM_STE_EN
    #error "Please define PWM_STE_EN!"
#elif    PWM_STE_EN == 1
volatile unsigned short int g_uiArrTemp = TIM1_PERIOD_SHORT;
volatile unsigned short int g_uiFqrTemp = 4080;
#endif
#if  TRIG == 1
volatile unsigned char g_ucTrigCnt = 0;
#endif
volatile unsigned char g_ucVinLowFlag = 0;

/* ���ܼ��� */
#if ENERGY_TEST_EN == 1
volatile unsigned long g_ulEnergySaveCnt;
volatile unsigned int  g_uiEnergySaveEn = 0;     //
unsigned char          fg_ucCSE7759FlagBuffer = 0;
#endif
/*************************** ���ļ���ȫ�ֱ��� ***************************/
static unsigned short int   fg_uiTIM3_Pulse = 0;
static unsigned short int   fg_uiState = 0;
static unsigned char        fg_ucOrderOnOff;         /* ����ػ� */
static unsigned char        fg_ucGroupNumber;
static unsigned char        fg_ucFlagInputParamComplete = 0;
static unsigned char        fg_ucDimReal = DIM_TO_DATA(START_DIM);   /* ���͵����ֵ�λ����ֵ */
static char                 fg_cTemperature;
static unsigned char        fg_ucBrightnessTrue;
static unsigned char        fg_ucDimEn = 1;
static unsigned short int   fg_uiPowerTimeCntBuffer[POWER_BUFFER_LONG];
static unsigned short int   fg_uiPowerPluseCntBuffer[POWER_BUFFER_LONG];

void InitPiParm(void);
/* �����ƾ߱������� */
#if  PARM_REPAIR_EN == 1 
	ScmRepairParamAll   g_sRepairParmAll;
	ScmRepairParamAll   g_sRepairParmAllTemp;
	unsigned short int  g_uiVoltageInFrq, g_uiCurrentInFrq, g_uiPowerInFrq;
	UcmRepairParam      ePowerCheck, eVoltageCheck, eCurrentCheck;  //
	static short int    fg_uiPowerTimeTemp, fg_uiPowerCntTemp;
	static short int    fg_uiVoltageTimeTemp, fg_uiVoltageCntTemp;
	static short int    fg_uiCurrentTimeTemp, fg_uiCurrentCntTemp;
#ifndef SEECRET_EN
  #error "Please define SEECRET_EN"
#elif SEECRET_EN == 1
  unsigned char g_ucUniqueDeviceId[12] = {0};
  unsigned char g_ucUniqueDeviceIdTemp[12] = {0};
  volatile unsigned short int g_uiDelayTime = 0;
  unsigned char g_ucLisenceEn;
  void ReadDeviceID(unsigned char *ucUniqueDeviceId);
#endif
unsigned short int          g_uiCurrentOutTemp_mA;
#endif
/*************************** ���ļ��ĺ��� ***************************/
static void VariableInit(void);
static void SendDataToPLC(void);
static void CommunicateWithPLC(void);
static unsigned short int DimCurrentOut(unsigned int uiCurrentSet, unsigned int uiCurrentOut, unsigned short int uiTIM3_Pulse);
static void DimCurrentOutMCP41010(unsigned char ucDim);
static void DimLighting(unsigned char DimValue);
static unsigned short int ProtectChek(unsigned short int uiState);
static void TurnOffLamp(void);
static unsigned char InputParmTest(void);
static void VrefInit(void);
static unsigned char ReducePower(unsigned int uiVoltageIn, char cTemperature, unsigned char ucBrightness);
  
void SetFre(void);
/*******************************************************************************
* Function Name : variable_init
* Description   : initialize variable
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
static void VariableInit(void)
{   
    g_ucReciveDelay = RECIVE_DELAY;
    fg_ucDimReal = DIM_TO_DATA(START_DIM);
   // g_sSysParam.uiState = 0;
	g_sSysParam.ucLampEn = 1;
    FrqInit();
}
/*******************************************************************************
* Function Name : TurnOnLamp
* Description   : TurnOnLamp
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
static void TurnOnLamp(void)
{
    VariableInit();
#if REF_ADJUST_MODE == 2  /* ��ʼ��PID */
    InitPiParm();
#endif 
    fg_ucDimReal = DIM_TO_DATA(START_DIM);
    PWM_ON; 
}
/*******************************************************************************
* Function Name : TurnOffLamp
* Description   : TurnOffLamp
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
static void TurnOffLamp(void)
{		  
    PWM_OFF; 
    fg_ucDimReal = DIM_TO_DATA(START_DIM);
}
/*******************************************************************************
* Function Name : SendDataToPLC
* Description   : SendDataToPLC
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
static void SendDataToPLC(void)
{
	static unsigned char  s_ucSendCnt = 0;
	if(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == SET){  
		if(g_sSysParam.ucUART_SendEn == 1){	 
			if(s_ucSendCnt < g_ucSendSum){
				USART_SendData(USART1, g_ucSendData[s_ucSendCnt]);
			  s_ucSendCnt++;
			}else{
				g_sSysParam.ucUART_SendEn = 0;
				s_ucSendCnt = 0;
				g_ucSendSum = 0;
				g_ucFlagReciveDealWith = 0;
			}
	  }
	}
}

/*******************************************************************************
* Function Name : variable_init
* Description   : initialize variable
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
static void Dim(unsigned char ucDimValue)
{
    if (ucDimValue == 0x00){
        fg_ucOrderOnOff = 0; //Turn off the lamp
	    g_sSysParam.iBrightness = 0;
    }else if(ucDimValue <= 0xc8){  
	    g_sSysParam.iBrightness = ucDimValue;
		if (g_sSysParam.iBrightness < DIM_MIN){
			g_sSysParam.iBrightness = DIM_MIN;
		}
		if (g_sSysParam.iBrightness > DIM_MAX){
			g_sSysParam.iBrightness = DIM_MAX;
		}
	#if REF_ADJUST_MODE == 2
		if (fg_ucOrderOnOff == 0){
		    InitPiParm();
		}
	#endif
		fg_ucOrderOnOff = 1;
    }
}

/*******************************************************************************
* Function Name : Char2Float
* Description   : Change char to float.
* Input         : pucData--Head adress of char�� 
* Output        : rezult of changing
* Return        : None.
*******************************************************************************/
float Char2Float(unsigned char *pucData)
{
	float fReturnValue;
	fReturnValue = ((long)pucData[0] << 24) | ((long)pucData[1] << 16)| ((long)pucData[2] << 8) | pucData[3];
	return(fReturnValue);
}
/*******************************************************************************
* Function Name : CommunicateWithPLC
* Description   : initialize variable
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
static void CommunicateWithPLC(void)
{
#if   PROTOCOL == XJP_V01
  static unsigned char ucFlagDimEn = 1;          //����ʹ��λ
	unsigned char ucXorCheck, ucSumCheck;
#endif 
#if  PARM_REPAIR_EN == 1
	unsigned char ucAck = 0x04;  //???,????????????
	UcmRepairParam uTemp;        //????
	UcmRepairParam uRatio;       //??
	UcmRepairParam uIntercept;   //??
	ScmRepairParamAll  sRepairParmAllRead;
	float fTemp;
	float fOutputVoltageRatioTemp, fOutputCurrentRatioTemp;
	float fOutputCurrentInterceptTemp;
#endif
#if ENERGY_TEST_EN == 1
	unsigned long ulEnergyCurrent_100mWh;
	unsigned short int  uiEnergyCntTemp;
  double   dTemp;
	unsigned long ulTemp;
#endif
	unsigned char ucCnt;
  if(g_ucFlagReciveDealWith == 1 && g_sSysParam.ucUART_SendEn == 0)
	{   /* ��ȡ�������� */
		g_sSysParam.uiVoltageOut = g_uiVoltageOut_100mV;
		g_sSysParam.uiCurrentOut = g_uiCurrentOut_mA;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
		if(g_sSysParam.uiVoltageIn != 0 && g_sSysParam.uiCurrentIn != 0){		    
			g_sSysParam.ucFactor = g_sSysParam.uiPowerIn * 10 / ((g_sSysParam.uiVoltageIn/10.0) * (g_sSysParam.uiCurrentIn/1000.0));
		}
		if(g_sSysParam.ucFactor > 99){
			g_sSysParam.ucFactor = 99; 
		}
   #ifndef  PROTOCOL
       #error "Please define PROTOCOL!"
   #elif    PROTOCOL == MJX_V14
		if(g_ucReciveData[0] == 0x01 && g_ucReciveData[1] == 0 && g_ucReciveData[2] == 0x01){   /* PLC Read parameter from Power */									         
			g_ucSendData[0] = 0x81;
		  g_ucSendData[1] = DEVICE_NUMBER;
			g_ucSendData[2] = fg_cTemperature;
			g_ucSendData[3] = g_sSysParam.uiVoltageIn>>8;
			g_ucSendData[4] = (unsigned char)g_sSysParam.uiVoltageIn; 
			g_ucSendData[5] = g_sSysParam.uiCurrentIn>>8;
			g_ucSendData[6] = (unsigned char)g_sSysParam.uiCurrentIn;
			g_ucSendData[7] = g_sSysParam.uiVoltageOut>>8;
			g_ucSendData[8] = (unsigned char)g_sSysParam.uiVoltageOut; 
			g_ucSendData[9] = g_sSysParam.uiCurrentOut>>8;
			g_ucSendData[10] = (unsigned char)g_sSysParam.uiCurrentOut;
			g_ucSendData[11] = g_sSysParam.uiPowerIn>>8; 
			g_ucSendData[12] = (unsigned char)g_sSysParam.uiPowerIn;
			g_ucSendData[13] = g_sSysParam.ucFactor; 
			g_ucSendData[14] = fg_uiTIM3_Pulse /48;//0;
			g_ucSendData[15] = fg_ucBrightnessTrue; 
			if(g_sSysParam.ucLampEn == 0){
				g_ucSendData[15] = 0;
			}
			g_ucSendData[16] = g_ucPowerState;//0;
			g_ucSendData[17] = g_sSysParam.uiState>>8; 
			g_ucSendData[18] = (unsigned char)g_sSysParam.uiState;
					  
			/* ��У�� */
			g_ucSendData[19] = 0;
			for(ucCnt = 0; ucCnt < 19; ucCnt++){
				g_ucSendData[19] += g_ucSendData[ucCnt];
			}
			g_ucSendSum = 20;   //The sum of SendData;
			g_sSysParam.ucUART_SendEn = 1;
		}else if(g_ucReciveData[0] == 0x11 && (unsigned char)(g_ucReciveData[0]+ g_ucReciveData[1]) == g_ucReciveData[2]){
            /* ����������� */ 					
            Dim(g_ucReciveData[1]);
			g_ucSendData[0] = 0x80;
			g_ucSendData[1] = g_ucReciveData[1];
			g_ucSendData[2] = g_ucSendData[0] + g_ucSendData[1]; 
			g_ucSendSum = 3;            //The sum of SendData;
			g_sSysParam.ucUART_SendEn = 1;
		}else if(g_ucReciveData[0] == 0x12 && (unsigned char)(g_ucReciveData[0] + g_ucReciveData[1] + g_ucReciveData[2] + g_ucReciveData[3]) == g_ucReciveData[4]){
			/* �㲥���⣬��������Ӧ */
			if((fg_ucGroupNumber == g_ucReciveData[1]) || g_ucReciveData[1] == 0){
        Dim(g_ucReciveData[3]);
			}
		}else if(g_ucReciveData[0] == 0x15 && (unsigned char)(g_ucReciveData[0]+ g_ucReciveData[1]) == g_ucReciveData[2]){
			if(g_ucReciveData[1] != 0){ /* �����������Ϊ0 */
				if(1 == SetGroupNumber(g_ucReciveData[1])){
					fg_ucGroupNumber = g_ucReciveData[1];
					g_ucSendData[0] = 0x85;
					g_ucSendData[1] = g_ucReciveData[1];
					g_ucSendData[2] = g_ucSendData[0] + g_ucSendData[1]; 
					g_ucSendSum = 3;            //The sum of SendData;
					g_sSysParam.ucUART_SendEn = 1;
				}
			}
		}else if(g_ucReciveData[0] == 0x01 && g_ucReciveData[1] == 0x02 && g_ucReciveData[2] == 0x01){
			if(fg_ucDimEn == 1){
					//fg_ucDimEn = 0;
			}else{
					//fg_ucDimEn = 1;
			}
			g_ucSendData[0] = 0x55;
			g_ucSendData[1] = fg_ucDimEn;
			g_ucSendData[2] = g_ucSendData[0] + g_ucSendData[1]; 
			g_ucSendSum = 3;            //The sum of SendData;
			g_sSysParam.ucUART_SendEn = 1;
    }
   #elif    PROTOCOL == GRT_V15
		if (g_ucReciveData[0] == 0x01 && g_ucReciveData[2] == 0 && (unsigned char)(g_ucReciveData[0] + g_ucReciveData[1] + g_ucReciveData[2]) == g_ucReciveData[3]){   /* PLC Read parameter from Power */	
            //��Ž���
            if (g_ucReciveData[1] != 0 && g_ucReciveData[1] != fg_ucGroupNumber){ /* ��������ڵ���Ų�һ�¾;�����ţ������������Ϊ0 */
				if (1 == SetGroupNumber(g_ucReciveData[1])){
					/* ��ȡ��� */
                    fg_ucGroupNumber = ReadGroupNumber();
				}
			}
			//��ȡ����ʱ�䣬�͹�˥����ʹ��λ
			ReadRunTime(&g_ulRunTime, &g_ucRepairEn);
            //			
			g_ucSendData[0] = 0x81;
		  g_ucSendData[1] = DEVICE_NUMBER;
			g_ucSendData[2] = fg_cTemperature;
			g_ucSendData[3] = g_sSysParam.uiVoltageIn>>8;
			g_ucSendData[4] = (unsigned char)g_sSysParam.uiVoltageIn; 
			g_ucSendData[5] = g_sSysParam.uiCurrentIn>>8;
			g_ucSendData[6] = (unsigned char)g_sSysParam.uiCurrentIn;
			g_ucSendData[7] = g_sSysParam.uiVoltageOut>>8;
			g_ucSendData[8] = (unsigned char)g_sSysParam.uiVoltageOut; 
			g_ucSendData[9] = g_sSysParam.uiCurrentOut>>8;
			g_ucSendData[10] = (unsigned char)g_sSysParam.uiCurrentOut;
			g_ucSendData[11] = g_sSysParam.uiPowerIn>>8; 
			g_ucSendData[12] = (unsigned char)g_sSysParam.uiPowerIn;
			g_ucSendData[13] = g_sSysParam.ucFactor; 
			g_ucSendData[14] = fg_uiTIM3_Pulse /48;//0;
			g_ucSendData[15] = fg_ucBrightnessTrue; 
            if (g_sSysParam.ucLampEn == 0){
                g_ucSendData[15] = 0;
            }
			g_ucSendData[16] = g_ucPowerState;//0;
			if (g_ucRepairEn != 0){
			    g_sSysParam.uiState |= COMPENSATE;
			}else{
				g_sSysParam.uiState &= ~COMPENSATE;
			}
			g_ucSendData[17] = g_sSysParam.uiState>>8; 
			g_ucSendData[18] = (unsigned char)g_sSysParam.uiState;
					  
			g_ucSendData[19] = g_ulRunTime >> 16;
			g_ucSendData[20] = g_ulRunTime >> 8;
			g_ucSendData[21] = g_ulRunTime;
			g_ucSendData[22] = 0;
			/* ��У�� */
			g_ucSendData[23] = 0;
			for(ucCnt = 0; ucCnt < 23; ucCnt++){
				g_ucSendData[23] += g_ucSendData[ucCnt];
			}
			g_ucSendSum = 24;   //The sum of SendData;
			g_sSysParam.ucUART_SendEn = 1;
		}else if(g_ucReciveData[0] == 0x11 && (unsigned char)(g_ucReciveData[0]+ g_ucReciveData[1]) == g_ucReciveData[2]){
            /* ����������� */ 					
            Dim(g_ucReciveData[1]);
			g_ucSendData[0] = 0x80;
			g_ucSendData[1] = g_ucReciveData[1];
			g_ucSendData[2] = g_ucSendData[0] + g_ucSendData[1]; 
			g_ucSendSum = 3;            //The sum of SendData;
			g_sSysParam.ucUART_SendEn = 1;
		}else if(g_ucReciveData[0] == 0x12 && (unsigned char)(g_ucReciveData[0] + g_ucReciveData[1] + g_ucReciveData[2] + g_ucReciveData[3]) == g_ucReciveData[4]){
			/* �㲥���⣬��������Ӧ */
			if((fg_ucGroupNumber == g_ucReciveData[1]) || g_ucReciveData[1] == 0){
                Dim(g_ucReciveData[3]);
			}
		}else if(g_ucReciveData[0] == 0x15 && (unsigned char)(g_ucReciveData[0]+ g_ucReciveData[1]+ g_ucReciveData[2]+ g_ucReciveData[3]+ g_ucReciveData[4]+ g_ucReciveData[5]) == g_ucReciveData[6]){
		    if (g_ucReciveData[5] & 0x01){ //������
				if (g_ucReciveData[1] != 0 && g_ucReciveData[1] != fg_ucGroupNumber){ /* �����������Ϊ0 */
					SetGroupNumber(g_ucReciveData[1]);
					/* ��ȡ��� */
                    fg_ucGroupNumber = ReadGroupNumber();	
				}
			}
			if (g_ucReciveData[5] & 0x02){//��������ʱ��
				g_ulRunTimeTemp = g_ucReciveData[2];
				g_ulRunTimeTemp <<= 8;
				g_ulRunTimeTemp |= g_ucReciveData[3];
				g_ulRunTimeTemp <<= 8;
				g_ulRunTimeTemp |= g_ucReciveData[4];
				ReadRunTime(&g_ulRunTime, &g_ucRepairEn);
				if (g_ulRunTimeTemp != g_ulRunTime){ //�Ƚ�
					SaveRunTime(g_ulRunTimeTemp, g_ucRepairEn);
					ReadRunTime(&g_ulRunTime, &g_ucRepairEn);
				}
		    }
			if (g_ucReciveData[5] & 0x04){//���ò���ʹ��λ
				if (g_ucReciveData[5] & 0x08){//ʹ��
					if (g_ucRepairEn == 0){ //�ж��Ƿ������ڵ�ʹ��λһ�£����һ�����ñ���
					    ReadRunTime(&g_ulRunTime, &g_ucRepairEn);
					    SaveRunTime(g_ulRunTime, 1);
						ReadRunTime(&g_ulRunTime, &g_ucRepairEn);
					}
				}else{//��ʹ��
					if (g_ucRepairEn != 0){ //�ж��Ƿ������ڵ�ʹ��λһ�£����һ�����ñ���
					    ReadRunTime(&g_ulRunTime, &g_ucRepairEn);
					    SaveRunTime(g_ulRunTime, 0);
						ReadRunTime(&g_ulRunTime, &g_ucRepairEn);
					}
				}
		    }
			//��������
			g_ucSendData[0] = 0x85;
			g_ucSendData[1] = 0x01;
			g_ucSendData[2] = g_ucSendData[0] + g_ucSendData[1]; 
			g_ucSendSum = 3;            //The sum of SendData;
			g_sSysParam.ucUART_SendEn = 1;
		}else if(g_ucReciveData[0] == 0x01 && g_ucReciveData[1] == 0x02 && g_ucReciveData[2] == 0x01){
            if (fg_ucDimEn == 1){
                //fg_ucDimEn = 0;
            }else{
                //fg_ucDimEn = 1;
            }
            g_ucSendData[0] = 0x55;
			g_ucSendData[1] = fg_ucDimEn;
			g_ucSendData[2] = g_ucSendData[0] + g_ucSendData[1]; 
			g_ucSendSum = 3;            //The sum of SendData;
			g_sSysParam.ucUART_SendEn = 1;
        }
   #elif   PROTOCOL == XJP_V01
		switch(g_ucReciveData[0]){
			case 0x12:
				if (g_ucReciveCnt == 2){ //�ж����ݳ��ȣ���0��ʼ
			        if ((unsigned char)(g_ucReciveData[0]+ g_ucReciveData[1]) == g_ucReciveData[2]){//�жϺ�У��
						//Dimming Broadcast Control		
						if (ucFlagDimEn == 1){
							Dim(g_ucReciveData[1]);
						}
						g_ucFlagReciveDealWith = 0;
					}else{
						g_ucFlagReciveDealWith = 0;
					}
			    }else{
					g_ucFlagReciveDealWith = 0;
				}
				break;
			case 0x13:
				if (g_ucReciveCnt == 2){ //�ж����ݳ��ȣ���0��ʼ
			    if ((unsigned char)(g_ucReciveData[0]+ g_ucReciveData[1]) == g_ucReciveData[2]){//�жϺ�У��
						//Dim and return Power/Status
						if (ucFlagDimEn == 1){
							if (g_ucReciveData[1] <= 0XC8){ 
							    Dim(g_ucReciveData[1]);
							}
						}
						 //return status
						g_ucSendData[0] = 0x93;
						g_ucSendData[1] = fg_cTemperature;
						g_ucSendData[2] = (unsigned char)g_sSysParam.uiVoltageOut; 
						g_ucSendData[3] = g_sSysParam.uiVoltageOut>>8;
						g_ucSendData[4] = (unsigned char)g_sSysParam.uiCurrentOut;
						g_ucSendData[5] = g_sSysParam.uiCurrentOut>>8;
						g_ucSendData[6] = (unsigned char)g_sSysParam.uiVoltageIn; 
						g_ucSendData[7] = g_sSysParam.uiVoltageIn>>8;
						g_ucSendData[8] = (unsigned char)g_sSysParam.uiCurrentIn; 
						g_ucSendData[9] = g_sSysParam.uiCurrentIn>>8;
						g_ucSendData[10] = (unsigned char)g_sSysParam.uiPowerIn;
						g_ucSendData[11] = g_sSysParam.uiPowerIn>>8; 
						g_ucSendData[12] = g_sSysParam.ucFactor; 
						g_ucSendData[13] = (unsigned char)g_sSysParam.uiState;
						g_ucSendData[14] = g_sSysParam.uiState>>8; 
						g_ucSendData[15] = g_sSysParam.iBrightness; 
						if (g_ucReciveData[1] <= 0XC8){
							g_ucSendData[16] = 0;
							for(ucCnt = 1; ucCnt < 16; ucCnt++){
								g_ucSendData[16] += g_ucSendData[ucCnt];
							}
							g_ucSendSum = 17;            //The sum of SendData;
							g_sSysParam.ucUART_SendEn = 1;
						}else if(g_ucReciveData[1] == 0xff){
							g_ucSendData[16] = fg_ucGroupNumber;
							g_ucSendData[17] = 0;
							for(ucCnt = 1; ucCnt < 17; ucCnt++){
								g_ucSendData[17] += g_ucSendData[ucCnt];
							}
							g_ucSendSum = 18;            //The sum of SendData;
							g_sSysParam.ucUART_SendEn = 1;
						}else{
							g_ucFlagReciveDealWith = 0;
						}
					}else{
						g_ucFlagReciveDealWith = 0;
					}
			    }else{
					g_ucFlagReciveDealWith = 0;
				}
				break;
			case 0x14:
				if (g_ucReciveCnt == 2){ //�ж����ݳ��ȣ���0��ʼ
			        if ((unsigned char)(g_ucReciveData[0]+ g_ucReciveData[1]) == g_ucReciveData[2]){//�жϺ�У��
						ucFlagDimEn = 0;    //�ر�ǰ����ָ����⹦��
						Dim(g_ucReciveData[1]);
						//Return acknowledge
						g_ucSendData[0] = 0x94;
						g_ucSendData[1] = g_sSysParam.iBrightness;//g_ucReciveData[1];
						g_ucSendData[2] = g_ucSendData[0] + g_ucSendData[1]; 
						g_ucSendSum = 3;            //The sum of SendData;
						g_sSysParam.ucUART_SendEn = 1;
					}else{
						g_ucFlagReciveDealWith = 0;
					}
			    }else{
					g_ucFlagReciveDealWith = 0;
				}
				break;
			case 0x15:
				if (g_ucReciveCnt == 2){ //�ж����ݳ��ȣ���0��ʼ
			        if ((unsigned char)(g_ucReciveData[0]+ g_ucReciveData[1]) == g_ucReciveData[2]){//�жϺ�У��
						ucFlagDimEn = 1;    //ʹ��ǰ����ָ����⹦��
						Dim(g_ucReciveData[1]);
						//Return acknowledge
						g_ucSendData[0] = 0x95;
						g_ucSendData[1] = g_sSysParam.iBrightness;//g_ucReciveData[1];
						g_ucSendData[2] = g_ucSendData[0] + g_ucSendData[1]; 
						g_ucSendSum = 3;            //The sum of SendData;
						g_sSysParam.ucUART_SendEn = 1;
					}else{
						g_ucFlagReciveDealWith = 0;
					}
			    }else{
					g_ucFlagReciveDealWith = 0;
				}
				break;
			case 0x16:
				if (g_ucReciveCnt == 2 && g_ucReciveData[1] == 0X01){ //�ж����ݳ��ȣ���0��ʼ
			        if ((unsigned char)(g_ucReciveData[0]+ g_ucReciveData[1]) == g_ucReciveData[2]){//�жϺ�У��
						g_ucSendData[0] = 0x96;
						g_ucSendData[1] = (unsigned char)(SOFT_VERSION >> 8);
					    g_ucSendData[2] = (unsigned char)SOFT_VERSION;
						g_ucSendData[3] = HARDWARE_VERSION;
						g_ucSendData[4] = DEVICE_NUMBER;
						ReadRepairParmAll(&g_sRepairParmAllTemp, PARAM_REPAIR_ADR1);
						if (g_sRepairParmAllTemp.ucParmState != 0){
                            g_ucSendData[5] = 0x38;
                        }else{
							g_ucSendData[5] = 0x00;
                        }							
						g_ucSendData[6] = 0;
                        for(ucCnt = 0; ucCnt < 6; ucCnt++){
							g_ucSendData[6] += g_ucSendData[ucCnt];
						}						
						g_ucSendSum = 7;            //The sum of SendData;
						g_sSysParam.ucUART_SendEn = 1;
					}else{
						g_ucFlagReciveDealWith = 0;
					}
			    }else{
					g_ucFlagReciveDealWith = 0;
				}
				break;
	  #if ENERGY_TEST_EN == 1
			case 0x17:
				if(g_ucReciveCnt != 2){ //�ж����ݳ��ȣ���0��ʼ
					g_ucFlagReciveDealWith = 0;
					break;
				}
				/* ���ݳ��Ⱥ��� */
				if((unsigned char)(g_ucReciveData[0]+ g_ucReciveData[1]) == g_ucReciveData[2]){//�жϺ�У��
					/* ������� */
					ulTemp = ReadEnergyDataFromFlash();
					if (g_uiEnergySaveEn != 0){ 
					  ulTemp += g_uiEnergySaveEn;
					}
					uiEnergyCntTemp = GET_ENERGY_TIM_CNT();
					dTemp = g_sRepairParmAll.uPowerCheck.f * (g_sRepairParmAll.uiPowerTimeCheck/1000.0) / g_sRepairParmAll.uiPowerCntCheck;
					//unit:0.1Wh
					ulEnergyCurrent_100mWh = (unsigned long)(dTemp * ENERGY_SAVE_CNT * ulTemp/360.0 + dTemp * uiEnergyCntTemp/360.0);  
					//Dim and return Power/Status
					if (ucFlagDimEn == 1){
						if (g_ucReciveData[1] <= 0XC8){ 
							Dim(g_ucReciveData[1]);
						}
					}
					 //return status
					g_ucSendData[0] = 0x97;
					g_ucSendData[1] = fg_cTemperature;
					g_ucSendData[2] = (unsigned char)g_sSysParam.uiVoltageOut; 
					g_ucSendData[3] = g_sSysParam.uiVoltageOut>>8;
					g_ucSendData[4] = (unsigned char)g_sSysParam.uiCurrentOut;
					g_ucSendData[5] = g_sSysParam.uiCurrentOut>>8;
					g_ucSendData[6] = (unsigned char)g_sSysParam.uiVoltageIn; 
					g_ucSendData[7] = g_sSysParam.uiVoltageIn>>8;
					g_ucSendData[8] = (unsigned char)g_sSysParam.uiCurrentIn; 
					g_ucSendData[9] = g_sSysParam.uiCurrentIn>>8;
					g_ucSendData[10] = (unsigned char)g_sSysParam.uiPowerIn;
					g_ucSendData[11] = g_sSysParam.uiPowerIn>>8; 
					g_ucSendData[12] = g_sSysParam.ucFactor; 
					g_ucSendData[13] = (unsigned char)g_sSysParam.uiState;
					g_ucSendData[14] = g_sSysParam.uiState>>8; 
					g_ucSendData[15] = g_sSysParam.iBrightness; 
					if (g_ucReciveData[1] <= 0XC8){
						g_ucSendData[16] = 0;  /* �ն�ֵ */
						g_ucSendData[17] = (unsigned char)ulEnergyCurrent_100mWh;
						g_ucSendData[18] = (unsigned char)(ulEnergyCurrent_100mWh >> 8);
						g_ucSendData[19] = (unsigned char)(ulEnergyCurrent_100mWh >> 16);
						g_ucSendData[20] = (unsigned char)(ulEnergyCurrent_100mWh >> 24);
						g_ucSendData[21] = 0;
						for(ucCnt = 1; ucCnt < 21; ucCnt++){
							g_ucSendData[21] += g_ucSendData[ucCnt];
						}
						g_ucSendSum = 22;            //The sum of SendData;
						g_sSysParam.ucUART_SendEn = 1;
					}		    
				}
				g_ucFlagReciveDealWith = 0;
				break;
		#endif
			case 0x30: //��㲥
				ucXorCheck = g_ucReciveData[0] ^ g_ucReciveData[1] ^ g_ucReciveData[2];
			    ucSumCheck = g_ucReciveData[0] + g_ucReciveData[1] + g_ucReciveData[2];
			    if ((ucXorCheck == g_ucReciveData[3]) && (ucSumCheck == g_ucReciveData[4]) && g_ucReciveCnt == 4){
					if((fg_ucGroupNumber == g_ucReciveData[1]) || (g_ucReciveData[1] == 0)){
						if (g_ucReciveData[2] <= 0XC8){
                           Dim(g_ucReciveData[2]);
						}
			        }
				}
				g_ucFlagReciveDealWith = 0;
				break;
			case 0x31:
				ucXorCheck = g_ucReciveData[0] ^ g_ucReciveData[1] ^ g_ucReciveData[2];
			    ucSumCheck = g_ucReciveData[0] + g_ucReciveData[1] + g_ucReciveData[2];
			    if ((ucXorCheck == g_ucReciveData[3]) && (ucSumCheck == g_ucReciveData[4]) && g_ucReciveCnt == 4){
					switch(g_ucReciveData[1]){
					    case 0: //set group
							if ((fg_ucGroupNumber != g_ucReciveData[2]) && (g_ucReciveData[2] != 0)){
								if (1 == SetGroupNumber(g_ucReciveData[2])){
									fg_ucGroupNumber = g_ucReciveData[2];
								}
						    }
						case 1: //read group
							g_ucSendData[0] = 0xB1;
						    g_ucSendData[1] = fg_ucGroupNumber;
						    g_ucSendData[2] = g_ucSendData[0] + g_ucSendData[1]; 
						    g_ucSendSum = 3;            //The sum of SendData;
						    g_sSysParam.ucUART_SendEn = 1;
							break;
						default:
							g_ucFlagReciveDealWith = 0;
							break;
					}					
				}else{
					g_ucFlagReciveDealWith = 0;
				}
				break;
		#if LED_MODULE_OPEN_TEST_EN == 1
		  case 0x19:
				if(g_ucReciveData[1] == 0x01 && g_ucReciveData[2]==0X1A){
					g_ucFlagReciveDealWith = 0;
					g_ucSendData[0] = 0x99;
				  g_ucSendData[1] = fg_ucOpenLoadFlag;
			    g_ucSendData[2] = g_ucSendData[0] + g_ucSendData[1];
				  g_ucSendSum = 3;            //The sum of SendData;
				  g_sSysParam.ucUART_SendEn = 1;
				}
				break;
		#endif
			default:
				g_ucFlagReciveDealWith = 0;
				break;
		}
   #elif   PROTOCOL == PLC_DISABLE
   #endif
/**********�����ƾ�ͨ��PLCУ����Դ����************/
#if  PARM_REPAIR_EN == 1
    //////////////////////????////////////////////
    if(g_ucReciveData[0] == 0xA1){
			switch(g_ucReciveData[1]){
				case 0x01:    //�ָ�Ĭ������
					if(CheckUartRecive(5)){
						//У��ͨ��
						switch(g_ucReciveData[2]){
							case 0x00://���в����ָ�Ĭ������
								SetRepairParmAllToDefault(&g_sRepairParmAll);
								SaveRepairParmToBothArea(g_sRepairParmAll);
								ucAck = 0x01;
								break;
							default:
								break;
						}
						//������Ӧ
						g_ucSendData[0] = 0x8A;
						g_ucSendData[1] = 0x01;
						g_ucSendData[2] = g_ucReciveData[2];
						g_ucSendData[3] = ucAck;
						g_ucSendData[4] = 0x00;
						for(ucCnt = 0; ucCnt < 4;ucCnt++){
							g_ucSendData[4] += g_ucSendData[ucCnt];
						}
						g_ucSendSum = 5;
						g_sSysParam.ucUART_SendEn = 1;
					}
					break;
				/* ������������У�� */
				case 0x05: //���빦��У��
					if(CheckUartRecive(28)){//У��ͨ��
			      ucAck = 0x01; //��־Ĭ��ΪУ���ɹ�
			     //У�����빦��   g_uiVoltageCnt, g_uiVoltageTime
						ePowerCheck.l = Char2Float(&(g_ucReciveData[2])); 
						eVoltageCheck.l = Char2Float(&(g_ucReciveData[6])); 
			      eCurrentCheck.l = Char2Float(&(g_ucReciveData[10]));
						//�жϲ�����Χ�Ƿ�Ϸ�
						fTemp = ePowerCheck.f * (float)fg_uiPowerTimeTemp / (float)fg_uiPowerCntTemp;
						if((fTemp < PT_DEFAULT_L) || (fTemp > PT_DEFAULT_H)){//�жϹ���У׼��������
							ucAck = 0x03;
							sRepairParmAllRead.ucRepairError |= IPA;
						}
						fTemp = eVoltageCheck.f * (float)fg_uiVoltageTimeTemp / (float)fg_uiVoltageCntTemp;
						if((fTemp < VT_DEFAULT_L) || (fTemp > VT_DEFAULT_H)){//�жϵ�ѹУ׼��������
							ucAck = 0x03;
							g_sRepairParmAll.ucRepairError |= IVA;
						}
						fTemp = eCurrentCheck.f * (float)fg_uiCurrentTimeTemp / (float)fg_uiCurrentCntTemp;
						if((fTemp < AT_DEFAULT_L) || (fTemp > AT_DEFAULT_H)){//�жϵ���У׼��������
							 ucAck = 0x03;
							 g_sRepairParmAll.ucRepairError |= ICA;
						}
						//У�������ѹ
						uTemp.l = Char2Float(&(g_ucReciveData[14]));       //Output Voltage
						uRatio.f = uTemp.f;         
						if((uRatio.f < RATIO_MIN * OUT_VOLTAGE_RATIO_DEFAULT) || (RATIO_MAX * OUT_VOLTAGE_RATIO_DEFAULT < uRatio.f)){
							//���õĲ�����������Χ��
							ucAck = 0x03;
		          g_sRepairParmAll.ucRepairError |= OVA;
						}else{
		          fOutputVoltageRatioTemp = uRatio.f;
						}
						//У���������
						uTemp.l = Char2Float(&(g_ucReciveData[18]));       //Output Current
						uRatio.f = uTemp.f; 
						if((uRatio.f < RATIO_MIN * OUT_CURRENT_RATIO_DEFAULT) || (RATIO_MAX * OUT_CURRENT_RATIO_DEFAULT < uRatio.f)){
								//���õĲ�����������Χ��
						  ucAck = 0x03;
		          g_sRepairParmAll.ucRepairError |= OCA;
						}else{
		          fOutputCurrentRatioTemp = uRatio.f;
						}
						uTemp.l = Char2Float(&(g_ucReciveData[22]));       //Output Current
						uRatio.f = uTemp.f; 
						if((uRatio.f < -500) || 500 < uRatio.f){ // |�ؾ�| < 500mA
								//���õĲ�����������Χ��
							ucAck = 0x03;
							g_sRepairParmAll.ucRepairError |= OCA;
						}else{
							fOutputCurrentInterceptTemp = uRatio.f;
						}
			      if(ucAck == 0x01){  //У���ɹ����������
				    //�������빦�ʲ���
				      g_sRepairParmAll.uPowerCheck.l = ePowerCheck.l;
							g_sRepairParmAll.uiPowerTimeCheck = fg_uiPowerTimeTemp;
							g_sRepairParmAll.uiPowerCntCheck = fg_uiPowerCntTemp;
							//���������ѹ����
							g_sRepairParmAll.uInVoltageCheck.l = eVoltageCheck.l;
							g_sRepairParmAll.uiInVoltageTimeCheck = fg_uiVoltageTimeTemp;
							g_sRepairParmAll.uiInVoltageCntCheck = fg_uiVoltageCntTemp;
							//���������������
							eCurrentCheck.f = eCurrentCheck.f;
							g_sRepairParmAll.uInCurrentCheck.l = eCurrentCheck.l;
							g_sRepairParmAll.uiInCurrentTimeCheck = fg_uiCurrentTimeTemp;
							g_sRepairParmAll.uiInCurrentCntCheck = fg_uiCurrentCntTemp;
							//���������ѹ�͵�������
							g_sRepairParmAll.fOutVoltageRatio = fOutputVoltageRatioTemp;
							g_sRepairParmAll.fOutCurrentRatio = fOutputCurrentRatioTemp;
							g_sRepairParmAll.fOutCurrentIntercept = fOutputCurrentInterceptTemp;
							//��־λ
							g_sRepairParmAll.ucParmState = (IPA | IVA | ICA | OVA | OCA);
							sRepairParmAllRead.ucRepairError = 0;
							//��У������д��FLASH
							SaveRepairParmToBothArea(g_sRepairParmAll);
						}else{
							//д��FLASH������У������λ
							SaveRepairParmToBothArea(g_sRepairParmAll);
						}
						//������Ӧ
						ParmRepairAck4(0x05,ucAck);
					}  
					break;
		#if  PARM_REPAIR_EN == 1 && SEECRET_EN == 1
				case 0X06:
				case 0X07:
					if(CheckUartRecive(5)){
						//������Ӧ
						g_ucSendData[0] = 0x8A;
						g_ucSendData[1] = g_ucReciveData[1];
						g_ucSendData[2] = (unsigned char)SOFT_VERSION;
						g_ucSendData[3] = (unsigned char)(SOFT_VERSION>>8);
						g_ucSendData[4] = (unsigned char)HARDWARE_VERSION;
						g_ucSendData[5] = DEVICE_NUMBER;
						g_ucSendData[6] = 0X00;
						if (g_ucReciveData[2] != DEVICE_NUMBER){
							g_ucSendData[6] = 0X01;
						}else{
							if (g_ucReciveData[1] == 0x06){
								SetRepairParmAllToDefault(&g_sRepairParmAll);//У���ڼ�ʹ��Ĭ�ϲ�������
								ReadDataFromFlash(g_ucUniqueDeviceIdTemp, DEVICE_ID_SAVE_ADR, 12); //���������ID
								ReadDeviceID(g_ucUniqueDeviceId); //����MCUΨһID
								for(ucCnt = 0; ucCnt < 12;ucCnt++){
									if (g_ucUniqueDeviceId[ucCnt] != g_ucUniqueDeviceIdTemp[ucCnt]){
										break;
									}
								}
								if (ucCnt != 12){ //ID��һ��
									 WriteDataToFlash(g_ucUniqueDeviceId, DEVICE_ID_SAVE_ADR, 12); //��MCU IDд��FLASH����lisence��
									 Delay(1);
									 ReadDataFromFlash(g_ucUniqueDeviceIdTemp, DEVICE_ID_SAVE_ADR, 12); //���������ID
									 for(ucCnt = 0; ucCnt < 12;ucCnt++){
										 if (g_ucUniqueDeviceId[ucCnt] != g_ucUniqueDeviceIdTemp[ucCnt]){
											 break;
										 }
									 }
									 if (ucCnt == 12){
										 g_ucSendData[6] = 0X02;
										 g_ucLisenceEn = 1;
									 }else{
										 g_ucSendData[6] = 0X03;
									 }
								}else{
									 g_ucSendData[6] = 0X02;
								}
							}else{
								g_ucSendData[6] = 0X02;
								g_uiRepairingTimer = 0;
								g_ucRepairing = 1;
							}
						}
						g_ucSendData[7] = 0x00;
						for(ucCnt = 0; ucCnt < 7;ucCnt++){
							g_ucSendData[7] += g_ucSendData[ucCnt];
						}
						g_ucSendSum = 8;
						g_sSysParam.ucUART_SendEn = 1;
					}
					break;
		#endif
				case 8:
		#if ENERGY_TEST_EN == 1
				  if(CheckUartRecive(5)){
						//������Ӧ
						g_ucSendData[0] = 0x8A;
						g_ucSendData[1] = 8;
						g_ucSendData[2] = DEVICE_NUMBER;
						g_ucSendData[3] = 0X02;
						if (g_ucReciveData[2] != DEVICE_NUMBER){
							g_ucSendData[3] = 0X01;
						}else{ //��������
							g_ulEnergySaveCnt = 0;
							SaveEnergyData(0);
						}
						g_ucSendData[4] = 0x00;
						for(ucCnt = 0; ucCnt < 4;ucCnt++){
							g_ucSendData[4] += g_ucSendData[ucCnt];
						}
						g_ucSendSum = 5;
						g_sSysParam.ucUART_SendEn = 1;
					}
    #endif				
					break;
        default:break;
      }
    }else if(g_ucReciveData[0] == 0xA2){//��ȡ����
    //////////////////////��ȡ����////////////////////
			ReadRepairParmAll(&sRepairParmAllRead, PARAM_REPAIR_ADR1);
			switch(g_ucReciveData[1]){
				case 0x02:  //��ȡУ��״̬λ
					if (CheckUartRecive(4)){
						 //������Ӧ
							g_ucSendData[0] = 0x8B;
							g_ucSendData[1] = 0x02;           
							g_ucSendData[2] = (unsigned char)sRepairParmAllRead.ucParmState;
							g_ucSendData[3] = 0x00;
							for(ucCnt = 0; ucCnt < 3;ucCnt++){
									g_ucSendData[3] += g_ucSendData[ucCnt];
							}
							g_ucSendSum = 4;
							g_sSysParam.ucUART_SendEn = 1;
					}
					break;
				case 0x05: //��ȡ��������
					if (CheckUartRecive(4)){
							g_ucSendData[0] = 0x8B;
							g_ucSendData[1] = 0x05;
							g_ucSendData[2] = 0x01;
							g_ucSendData[3] = (unsigned char)(g_uiVoltageOutAdcValue >> 8);
							g_ucSendData[4] = (unsigned char) g_uiVoltageOutAdcValue;
							g_ucSendData[5] = (unsigned char)(g_uiCurrentOutAdcValue >> 8);
							g_ucSendData[6] = (unsigned char) g_uiCurrentOutAdcValue;
							g_ucSendData[7] = 0x00;
							for(ucCnt = 0; ucCnt < 7;ucCnt++){
									g_ucSendData[7] += g_ucSendData[ucCnt];
							}
							g_ucSendSum = 8;
							g_sSysParam.ucUART_SendEn = 1;
					}
					break;
			 #if SEECRET_EN == 1
				case 0x08: //��ȡ��ԴLisence״̬
					if (CheckUartRecive(4)){
								ReadDeviceID(g_ucUniqueDeviceId);
						ReadDataFromFlash(g_ucUniqueDeviceIdTemp, DEVICE_ID_SAVE_ADR, 12); //���������ID
						for(ucCnt = 0; ucCnt < 12;ucCnt++){
							if (g_ucUniqueDeviceId[ucCnt] != g_ucUniqueDeviceIdTemp[ucCnt]){
								break;
							}
						}
						if (ucCnt != 12){ //ID��һ��
							g_ucSendData[2] = 0X00;
						}else{//IDһ��
							g_ucSendData[2] = 0X01;
						}
						//������Ӧ
						g_ucSendData[0] = 0x8B;
						g_ucSendData[1] = 0x08;
						g_ucSendData[3] = 0x00;
						for(ucCnt = 0; ucCnt < 3;ucCnt++){
								g_ucSendData[3] += g_ucSendData[ucCnt];
						}
						g_ucSendSum = 4;
						g_sSysParam.ucUART_SendEn = 1;
					}
					break;
			 #endif
				case 0x09:
					if (CheckUartRecive(4)){
						//������Ӧ
						g_uiRepairingTimer = 0;
						g_ucRepairing = 1;
						g_ucSendData[0] = 0x8B;
						g_ucSendData[1] = 0x08;
						g_ucSendData[2] = g_ucSendData[0] + g_ucSendData[1];
						g_ucSendSum = 3;
						g_sSysParam.ucUART_SendEn = 1;
					}
					break;
				default:break;
			}
    }
#endif
 /*****************************************************************/
		/* ������ջ����� */
		for(ucCnt = 0; ucCnt < RECIVE_MAX; ucCnt++){
		  g_ucReciveData[ucCnt] = 0;
		}
    /* û�����ݻظ�����������մ����־�������ݻظ������ڻظ�������֮��������մ����־ */
    if(g_sSysParam.ucUART_SendEn == 0){ 
   	  g_ucFlagReciveDealWith = 0;
		}
  }
}

/*******************************************************************************
* Function Name : DimCurrentOut
* Description   : DimCurrentOut
* Input         : uiCurrentSet�����õ���ֵ  uiCurrentOut��ʵ���������ֵ  uiTIM3_Pulse: TIM3��PWM����
* Output        : None.
* Return        : �������TIM3��PWM����
*******************************************************************************/
static unsigned short int DimCurrentOut(unsigned int uiCurrentSet, unsigned int uiCurrentOut, unsigned short int uiTIM3_Pulse)
{
    unsigned short int uiReturnValue;
#ifndef REF_ADJUST_MODE
	#error "Please define REF_ADJUST_MODE!"
#elif REF_ADJUST_MODE == 1
    if (uiCurrentOut < uiCurrentSet){ /* ����ֵ����ʵ��ֵ */
	    if (uiCurrentSet - uiCurrentOut > uiCurrentSet * 0.1){
			if (uiTIM3_Pulse < TIM3_PERIOD - 150){
				uiTIM3_Pulse += 150;
			}else if(uiTIM3_Pulse < TIM3_PERIOD - 50){
				uiTIM3_Pulse += 50;
			}else if(uiTIM3_Pulse < TIM3_PERIOD - 10){
				uiTIM3_Pulse += 10;
			}else if(uiTIM3_Pulse < TIM3_PERIOD - 1){
				uiTIM3_Pulse += 1;
			}
		}else{
			if (uiCurrentSet - uiCurrentOut > 20){
                if (uiTIM3_Pulse < TIM3_PERIOD - 20){
					uiTIM3_Pulse += 20;
                }
			}else{
                if (uiTIM3_Pulse < TIM3_PERIOD - 1){
					uiTIM3_Pulse += 1;
                }
			}
		}
	}else{    /* ����ֵС��ʵ��ֵ */
	    if (uiCurrentOut - uiCurrentSet > uiCurrentSet * 0.1){
			if (uiTIM3_Pulse > 150 ){
				uiTIM3_Pulse -= 150;
			}else if(uiTIM3_Pulse > 10){
				uiTIM3_Pulse -= 10;
			}else if(uiTIM3_Pulse > 0){
				uiTIM3_Pulse--;
			}
		}else{
			if (uiCurrentOut - uiCurrentSet > 20){
				if (uiTIM3_Pulse > 20){
                    uiTIM3_Pulse -= 20;
				}else if(uiTIM3_Pulse > 0){
					uiTIM3_Pulse--;
				}
			}else{
				if (uiTIM3_Pulse > 0){
					uiTIM3_Pulse -= 1;
				}
			}
		}
	}
    /* �߽����� */
    if (uiTIM3_Pulse > TIM3_PERIOD){
        uiTIM3_Pulse = TIM3_PERIOD;
    }
	uiReturnValue = uiTIM3_Pulse;
#elif REF_ADJUST_MODE == 2
	g_sPiParmRefVol.Ref = uiCurrentSet;
	g_sPiParmRefVol.Fdb = uiCurrentOut;
	#if PARM_REPAIR_EN == 1
	    PiCalc(&g_sPiParmRefVol,g_ucRepairing);
	#else
	    PiCalc(&g_sPiParmRefVol,0);
	#endif
	uiReturnValue = g_sPiParmRefVol.Out;
#else
	#error "Please select REF_ADJUST_MODE right valude!"
#endif 
    
	return(uiReturnValue);
}

/*******************************************************************************
* Function Name : DimCurrentOutMCP41010
* Description   : ͨ�����ֵ�λ�������������
* Input         : uiCurrentSet�����õ���ֵ
* Output        : None.
* Return        : None.
*******************************************************************************/
#define  DELTA   20
static void DimCurrentOutMCP41010(unsigned char ucDim)
{  
    if (g_ucMCP41010SendEn == 0){
        ucDim = DIM_TO_DATA(ucDim); /* ������ֵת��Ϊ��λ����ֵ */
        if (fg_ucDimReal < ucDim){
            if (fg_ucDimReal < 255 - DELTA){
                fg_ucDimReal += DELTA;
                if (fg_ucDimReal > ucDim){
                    fg_ucDimReal = ucDim;
                }
            }else{
                fg_ucDimReal = ucDim;
            }
        }else if(fg_ucDimReal > ucDim){
            if (fg_ucDimReal > DELTA){
                fg_ucDimReal -= DELTA;
                if (fg_ucDimReal < ucDim){
                    fg_ucDimReal = ucDim;
                }
            }
        }
        g_ucMCP41010SendData = fg_ucDimReal;
        g_ucMCP41010SendEn = 1;
    }
}
#if CONSTANT_POWER_EN == 1
/*******************************************************************************
* Function Name : VolOutFilter
* Description   : Filter for voltage
* Input         : uiVolOutAdcValue; ucMode: 0:clear the filter sum; 1: filter working
* Output        : None.
* Return        : bit15: 0: Filter output unavailable ��1��Filter output available.
*******************************************************************************/
unsigned short int VolOutFilter(unsigned short int uiVolOutAdcValue, unsigned char ucMode)
{
	#define FILTER_CNT    16
	static unsigned long  s_ulSum = 0;
    static unsigned short int s_uiCnt = 0;
	static unsigned short int s_uiReturnValue = 0;
    if (ucMode == 0){
		s_ulSum = 0;
		s_uiCnt = 0;
		s_uiReturnValue &= 0x7FFF;
    }else{
		if (s_uiCnt < FILTER_CNT){
		    s_ulSum += uiVolOutAdcValue;
			s_uiCnt++;
			//s_uiReturnValue &= 0x7FFF;
			s_uiReturnValue = uiVolOutAdcValue;
			s_uiReturnValue |= 0x8000;
		}else{
			s_ulSum += uiVolOutAdcValue;
			s_uiReturnValue = s_ulSum / FILTER_CNT;
			s_ulSum -= s_uiReturnValue;
			s_uiReturnValue |= 0x8000;
		}
    }
    return(s_uiReturnValue);	
}
#endif
/*******************************************************************************
* Function Name : OutCurrentToAdValude
* Description   : Change real current to AD value.
* Input         : real current
* Output        : None.
* Return        : Current AD value.
*******************************************************************************/
static unsigned short int OutCurrentToAdValude(float fCurrent_A)
{
	unsigned short int uiReturnValue;
	uiReturnValue = (fCurrent_A * 1000 - g_sRepairParmAll.fOutCurrentIntercept) / g_sRepairParmAll.fOutCurrentRatio;
	if((signed short int)uiReturnValue < 0){  //��������
		uiReturnValue = 0;
	}
	return (uiReturnValue);
}
/*******************************************************************************
* Function Name : Dim
* Description   : DIM
* Input         : DimValue��
* Output        : None.
* Return        : None.
*******************************************************************************/
static void DimLighting(unsigned char DimValue)
{
	unsigned int uiCurrentAdcValue;
	unsigned short int uiCurrentSet;
#if CONSTANT_POWER_EN == 1
	float fVoutTemp_1V;
	float fPowerTemp_1W;
	unsigned short int uiVoltageAdcValueTemp;

#endif
#if FEEDBACK  == 2
	double x3, x2, x1, y;
	unsigned short int uiPWM;
#endif    
#if DIM_MODE == 2
	DimCurrentOutMCP41010(DimValue);
#else
	#if FEEDBACK  == 2
		x1 = DimValue * 0.5;//DimValue/200.0 * 100;
		x2 = x1 * x1;
		y = 0.00002739 * x2 + 0.00606027 * x1 + 0.01432074;
		uiPWM = TIM3_PERIOD * y;
		if(fg_uiTIM3_Pulse < uiPWM){
			if(fg_uiTIM3_Pulse + 20 < uiPWM){
				fg_uiTIM3_Pulse += 20;
			}else{
				fg_uiTIM3_Pulse = uiPWM;
			}
		}else if(fg_uiTIM3_Pulse > uiPWM){
			if(fg_uiTIM3_Pulse > uiPWM + 20){
				fg_uiTIM3_Pulse -= 20;
			}else{
				fg_uiTIM3_Pulse = uiPWM;
			}
		}
		TIM3_PWM_SET(fg_uiTIM3_Pulse);
	#else
		uiCurrentSet = OutCurrentToAdValude(CUR_RATED) * (DimValue/200.0); //��Ҫ���õĲο�����ת��ΪADֵ
		#if CONSTANT_POWER_EN == 1
			uiVoltageAdcValueTemp = VolOutFilter(g_uiVoltageOutAdcValue, 1);
			if (uiVoltageAdcValueTemp & 0x8000){ //�ж������ѹ�˲�ֵ�Ƿ���Ч
				uiVoltageAdcValueTemp &= 0x7FFF;
				fVoutTemp_1V = g_uiVoltageOut_100mV/10;//AD_TO_VOLTAGE_OUT(uiVoltageAdcValueTemp);
				#if 1
					if(fVoutTemp_1V > VOL_RATED){ //�㹦��ģʽ
						fPowerTemp_1W = (float)POW_LIMIT * (DimValue/200.0);
						uiCurrentSet = OutCurrentToAdValude(fPowerTemp_1W/fVoutTemp_1V);
					}else{ //�����ģʽ
						
					}
				#else
					fPowerTemp_1W = fVoutTemp_1V * CUR_RATED * DimValue/200.0;
					if(fPowerTemp_1W > POW_LIMIT){/* �ж���������Ƿ���ڶ���� */
						uiCurrentSet = uiCurrentSet * (float)POW_LIMIT / fPowerTemp_1W;
					}
				#endif
			}else{ //��Ч
				
			}
		#endif
		if(uiCurrentSet < OutCurrentToAdValude(CUR_RATED * (float)DIM_MIN/DIM_MAX)){ //��Сֵ����
			uiCurrentSet = OutCurrentToAdValude(CUR_RATED * (float)DIM_MIN/DIM_MAX);
		}
		uiCurrentAdcValue = g_uiCurrentOutAdcValue;//��������
		#if  TRIG == 1
			if(g_uiVinAC > VOLTAGE_BUS_TO_AD(VOL_AC_TRIG) *1.414){
			 fg_uiTIM3_Pulse = DimCurrentOut(uiCurrentSet, uiCurrentAdcValue, fg_uiTIM3_Pulse);
			 g_ucVinLowFlag = 0;
			}else{
			 g_ucVinLowFlag = 1;
			}
	  #else
		  fg_uiTIM3_Pulse = DimCurrentOut(uiCurrentSet, uiCurrentAdcValue, fg_uiTIM3_Pulse);
		#endif
      TIM3_PWM_SET(fg_uiTIM3_Pulse);
  #endif
#endif
}

/*******************************************************************************
* Function Name : VrefClear
* Description   : VrefClear
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
static void VrefClear(void)
{    
    FrqInit();
#if DIM_MODE == 2
    DimCurrentOutMCP41010(DimValue);
#else
    fg_uiTIM3_Pulse = TIM3_PERIOD/100;
    TIM3_PWM_SET(TIM3_PERIOD/100);
#endif
}
/*******************************************************************************
* Function Name : ProtectChek
* Description   : ProtectChek
* Input         : None.
* Output        : None.
* Return        : uiState: ������־
*******************************************************************************/
static unsigned short int ProtectChek(unsigned short int uiState)
{
	static unsigned char s_ucCurCnt = 0, s_ucVolCnt = 0, s_ucTepCnt = 0;
	unsigned int ui_VoltageInTemp; 
#if VOL_OUT_OVER_PROTECT_EN == 1
	static unsigned char s_ucVolFlag = 0;
#endif
	/* ��·�ж� */
	if(g_uiCurrentOut_mA > CUR_SHORT * 1000 && 
		g_uiVoltageOut_100mV < VOL_SHORT_CIRCUIT * 10 && ((uiState & SHORT_CIR) == 0)){
		s_ucCurCnt++;
		if (s_ucCurCnt >= PROTECT_CNT){
			s_ucCurCnt = 0;
			uiState |= SHORT_CIR; 
		}
	}else{
		if (s_ucCurCnt > 0){
			s_ucCurCnt--;
		}
  }
	/* �����ѹ�����ж� */
#ifndef  VOL_OUT_OVER_PROTECT_EN
  #erorr "Please define VOL_OUT_OVER_PROTECT_EN!"
#elif VOL_OUT_OVER_PROTECT_EN == 1
	if((uiState & OPEN_LOOP) == 0){  //û�п�·��������жϿ�·����
		if(g_uiVolOutOverProtectDelayCnt >= VOL_OUT_OVER_PROTECCT_DELAY){  /* �ϵ���ʱ������·���� */
		#if CONSTANT_POWER_EN == 1
			if(g_uiVoltageOut_100mV > (VOL_RATED + DELTA_VOUT + 4) * 10){
		#else
			if(g_uiVoltageOut_100mV > (VOL_RATED + DELTA_VOUT) * 10){
		#endif
			//�������С�ڶ������10%�����������ѹ���ڶ��ѹ2V������Ϊ��·
				s_ucVolCnt++;
				if((s_ucVolCnt >= PROTECT_CNT)){
					s_ucVolCnt = 0;
					uiState |= OPEN_LOOP;
				}
			}else{
					if(s_ucVolCnt > 0){
						s_ucVolCnt--;
					}
			}
		}
	}else{ //�п�·������������жϿ�·ȥ��
		if(/*g_uiCurrentOut_mA > CUR_RATED * 0.1 * 1000 */ g_uiVoltageOut_100mV < (VOL_RATED + RESTART_VOUT) * 10){
			//����������ڶ������10%������Ϊ����и��ء�
			s_ucVolCnt++;
			if((s_ucVolCnt >= PROTECT_CNT)){
					s_ucVolCnt = 0;
					uiState &= (~OPEN_LOOP);
			}
		}else{
			if(s_ucVolCnt > 0){
			  s_ucVolCnt--;
			}
		}
	}  
#endif    
/* ���� */
	if(g_ucFlagAdcEnd == 1){
		g_ucFlagAdcEnd = 0;
	/* ���±��� */
		if((uiState & TEMPERATURE_OVER) == 0){ /* û�г��ֹ��±������ж��Ƿ�ﵽ���±��� */
			if(fg_cTemperature > TEMPERATURE_OVER_VALUE){
				s_ucTepCnt++;
				if(s_ucTepCnt >= PROTECT_CNT){
					s_ucTepCnt = 0;
					uiState |= TEMPERATURE_OVER;
				}
			}else{
				if(s_ucTepCnt > 0){
					s_ucTepCnt--;
				}
			}
		}else{ /* �Ѿ����ֹ��±��������ж��Ƿ�ﵽ�ָ����� */
			if(fg_cTemperature < TEMPERATURE_OVER_RESTART_VALUE){
				s_ucTepCnt++;
				if(s_ucTepCnt > PROTECT_CNT){
					s_ucTepCnt = 0;
					uiState &= ~TEMPERATURE_OVER;
				}
			}else{
				if(s_ucTepCnt > 0){
					s_ucTepCnt--;
				}
			}
		}
/* ���±��� */
		if(fg_cTemperature < TEMPERATURE_UNDER_VALUE){
			uiState |= TEMPERATURE_UNDER;
		}else{
			uiState &= ~TEMPERATURE_UNDER;
		}
	}
	/* �����ѹ���� */
	if(fg_ucFlagInputParamComplete == 1){   /* CSE7759������� */
		ui_VoltageInTemp = g_sSysParam.uiVoltageIn/10;
#ifndef VOL_IN_PROTECT_EN
		#error "Please define VOL_IN_PROTECT_EN!"
#elif  VOL_IN_PROTECT_EN == 1
			/* �����ѹ���� */
		if((uiState & VOL_IN_OVER) == 0){  /* �������ѹ�������ж��Ƿ���ֹ�ѹ���� */
			if(ui_VoltageInTemp > VOL_IN_OVER_VALUE){
				if(s_ucVolFlag == 0){
					s_ucVolFlag = 1;
					g_uiVolInOverCnt = 0;
				}else{
					if(g_uiVolInOverCnt >= VOL_IN_OVER_CNT){
						uiState |= VOL_IN_OVER;
						s_ucVolFlag = 0;
					}
				}
			}else{
				s_ucVolFlag = 0;
			}
		}else{/* �Ѿ����������ѹ���������жϻָ� */
			if(ui_VoltageInTemp < VOL_IN_OVER_RESTART_VALUE){
				uiState &= ~VOL_IN_OVER;
			}
		}
		/* ����Ƿѹ���� */
		if((uiState & VOL_IN_UNDER) == 0){   /* ������Ƿѹ�������ж��Ƿ����Ƿѹ���� */
			if(ui_VoltageInTemp < VOL_IN_UNDER_VALUE){
				uiState |= VOL_IN_UNDER;
				g_uiUnderVolRestartDelay = 0;
			}
		}else{/* �Ѿ���������Ƿѹ���������жϻָ� */
			if(ui_VoltageInTemp > VOL_IN_UNDER_RESTART_VALUE){
				if(g_uiUnderVolRestartDelay >= UNDER_VOL_RESART_TIME){
					uiState &= ~VOL_IN_UNDER;
				}
			}
		}
#endif
		fg_ucFlagInputParamComplete = 0;
	}
	return(uiState);
}

#if REF_ADJUST_MODE == 2
/*******************************************************************************
* Function Name : InitPiParm
* Description   : InitPiParm
* Input         : None
* Output        : None.
* Return        : None.
*******************************************************************************/
void InitPiParm(void)
{
    g_sPiParmRefVol.Kp = 0.1; 
    g_sPiParmRefVol.OutMax = TIM3_PERIOD;
    g_sPiParmRefVol.OutMin = 0;
    g_sPiParmRefVol.DeltaMax = TIM3_PERIOD * 0.1;//TIM3_PERIOD * 0.1;
    g_sPiParmRefVol.Out = g_sPiParmRefVol.OutMin;
  
    g_sPiParmRefVol.Ki = 0.5;
    g_sPiParmRefVol.UiMax = (float)(TIM3_PERIOD);//g_sPiParmRefVol.DeltaMax;
    g_sPiParmRefVol.UiMin = -g_sPiParmRefVol.UiMax;//-g_sPiParmRefVol.DeltaMax;
    g_sPiParmRefVol.Ui = 0;
  
    g_sPiParmRefVol.Kd = 0;
    g_sPiParmRefVol.ErrPrevious = 0; 
}
#endif

/*******************************************************************************
* Function Name : InputParmTest
* Description   : InputParmTest
* Input         : None
* Output        : None.
* Return        : 0:���ڲ���  1���������
*******************************************************************************/
static unsigned char InputParmTest(void)
{   
  unsigned char ucReturnValue = 0;
	unsigned int  uiTemp;
	static unsigned char s_ucPcnt = 0;
	unsigned char ucCnt;
	if(g_ucCSE7759Flag & POWER_BIT){ 
		fg_uiPowerTimeCntBuffer[s_ucPcnt] = g_uiPowerTime;
		fg_uiPowerPluseCntBuffer[s_ucPcnt] = g_uiPowerCnt;
		if(++s_ucPcnt >= POWER_BUFFER_LONG){
			s_ucPcnt = 0;
		}
		for(ucCnt = 0,uiTemp = 0;ucCnt < POWER_BUFFER_LONG;ucCnt++){
			uiTemp += fg_uiPowerTimeCntBuffer[ucCnt];
		}
		fg_uiPowerTimeTemp = uiTemp / POWER_BUFFER_LONG;
		for(ucCnt = 0,uiTemp = 0;ucCnt < POWER_BUFFER_LONG;ucCnt++){
			uiTemp += fg_uiPowerPluseCntBuffer[ucCnt];
		}
		fg_uiPowerCntTemp = uiTemp / POWER_BUFFER_LONG;
		//��������빦��ʵ��ֵ
		g_sSysParam.uiPowerIn = (g_sRepairParmAll.uPowerCheck.f * g_sRepairParmAll.uiPowerTimeCheck * g_uiPowerCnt / (g_sRepairParmAll.uiPowerCntCheck * g_uiPowerTime)) * 10; //unit: 0.1W
#if ENERGY_TEST_EN == 1
		fg_ucCSE7759FlagBuffer |= POWER_BIT;
#else
		g_ucCSE7759Flag &= ~POWER_BIT; 
#endif
    ucReturnValue = 1;		
	}
	if(g_ucCSE7759Flag & VOL_BIT){
		//����������ѹʵ��ֵ 
		g_sSysParam.uiVoltageIn = (g_sRepairParmAll.uInVoltageCheck.f * g_sRepairParmAll.uiInVoltageTimeCheck * g_uiVoltageCnt / ((float)g_sRepairParmAll.uiInVoltageCntCheck * g_uiVoltageTime)) * 10;/* unit: 0.1V */
		fg_uiVoltageTimeTemp = g_uiVoltageTime;
		fg_uiVoltageCntTemp = g_uiVoltageCnt;
#if ENERGY_TEST_EN == 1
    fg_ucCSE7759FlagBuffer |= VOL_BIT;
#else
		g_ucCSE7759Flag &= ~VOL_BIT; 
#endif  
    ucReturnValue = 1;			
	}else if(g_ucCSE7759Flag & CUR_BIT){
		 //������������ʵ��ֵ
    g_sSysParam.uiCurrentIn = (g_sRepairParmAll.uInCurrentCheck.f * g_sRepairParmAll.uiInCurrentTimeCheck * g_uiCurrentCnt / (g_sRepairParmAll.uiInCurrentCntCheck * g_uiCurrentTime)) * 1000;  /* unit: 1mA */
		fg_uiCurrentTimeTemp = g_uiCurrentTime;
		fg_uiCurrentCntTemp = g_uiCurrentCnt;
#if ENERGY_TEST_EN == 1
    fg_ucCSE7759FlagBuffer |= CUR_BIT;
#else
		g_ucCSE7759Flag &= ~CUR_BIT; 
#endif 
		ucReturnValue = 1;	
	}
  return(ucReturnValue);
}

/*******************************************************************************
* Function Name : VrefInit
* Description   : VrefInit
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
static void VrefInit(void)
{
#if DIM_MODE == 2
	while(g_ucMCP41010SendEn == 1){
		IWDG_ReloadCounter();
	}
	/* �տ�ʼ */
	DimCurrentOutMCP41010(START_DIM);
	while(g_ucMCP41010SendEn == 1){
		IWDG_ReloadCounter();
	}
	/* �տ�ʼ */
	DimCurrentOutMCP41010(START_DIM);
	while(g_ucMCP41010SendEn == 1){
		IWDG_ReloadCounter();
	}
	/* �տ�ʼ */
	DimCurrentOutMCP41010(START_DIM);
	while(g_ucMCP41010SendEn == 1){
		IWDG_ReloadCounter();
	}
	/* �տ�ʼ */
	DimCurrentOutMCP41010(START_DIM);
	while(g_ucMCP41010SendEn == 1){
		IWDG_ReloadCounter();
	}
#else
  TIM3_PWM_SET(TIM3_PERIOD/100);
#endif
}

/*******************************************************************************
* Function Name : ReducePower
* Description   : ReducePower
* Input         : uiVoltageIn
* Output        : None.
* Return        : Brightness
*******************************************************************************/
static unsigned char ReducePower(unsigned int uiVoltageIn, char cTemperature, unsigned char ucBrightness)
{   
#if TEMP_REDUCE_EN == 1
  static unsigned char Temp = 0;
#endif
	unsigned char ucReturnValue;
	static unsigned char s_ucBrightnessLast = 200;
	static unsigned char s_ucBrightnessPre = 0;
	float  fVoltageIn;
	//unsigned int uiVoltageInTemp;
	ucReturnValue = ucBrightness;
	if (g_uiReduceCnt >= REDUCE_TIME){
		g_uiReduceCnt = 0;
/* ��ѹ���� */
#ifndef  VOLTAGE_REDUCE_EN
		#error "Please select the VOLTAGE_REDUCE_EN!"
#elif    VOLTAGE_REDUCE_EN == 1
		fVoltageIn = (float)uiVoltageIn/10.0;
		if(fVoltageIn < VOLTAGE_UP){
			if(fVoltageIn > VOLTAGE_DOWN){
				ucBrightness = ((fVoltageIn - VOLTAGE_DOWN)/(VOLTAGE_UP - VOLTAGE_DOWN) * (1 - VOLTAGE_RATE) + VOLTAGE_RATE) * ucBrightness;
			}else{
				ucBrightness = ucBrightness * VOLTAGE_RATE;
			}
		}
#endif 
		/* �¶Ƚ��� */
#ifndef  TEMP_REDUCE_EN
		#error "Please select the TEMP_REDUCE_EN!"
#elif    TEMP_REDUCE_EN == 1
		if(cTemperature >= TEMP_REDUCE){
			Temp = 1;
			ucBrightness = ucBrightness * TEMP_RATE;
		}else if(cTemperature > TEMP_RESTORE){
			if(Temp == 1){
				ucBrightness = ucBrightness * TEMP_RATE;
			}
		}else{ //cTemperature < TEMP_RESTORE
			Temp = 0;
		}
#endif
		if(s_ucBrightnessLast < ucBrightness){
			s_ucBrightnessLast++;
		}else if(s_ucBrightnessLast > ucBrightness){
			s_ucBrightnessLast--;
		}
	}
	/* ������������� */
	if (s_ucBrightnessPre != ucReturnValue){
		s_ucBrightnessPre = ucReturnValue;
		s_ucBrightnessLast = ucBrightness;
	}
	ucReturnValue = s_ucBrightnessLast;
	return(ucReturnValue);    
}

#if PARM_REPAIR_EN == 1
#if SEECRET_EN == 1
/*******************************************************************************
* Function Name : ReadDeviceID
* Description   : read MCU unique device ID
* Input         : *ucUniqueDeviceId
* Output        : None.
* Return        : None.
*******************************************************************************/
void ReadDeviceID(unsigned char *ucUniqueDeviceId)
{
	unsigned char ucCnt;
	for(ucCnt = 0; ucCnt < 12; ucCnt++){
		*(ucUniqueDeviceId + ucCnt) = *(unsigned char *)(DEVICE_ID_ADR + ucCnt);
	}
}
#endif
#endif


/*******************************************************************************
* Function Name : bubbleSort
* Description   : Set fg_uiTIM3_Pulse into half
* Input         : None
* Output        : None.
* Return        : None.
*******************************************************************************/
void bubbleSort(unsigned short int array[],unsigned char size )//��������д�С��������  
{  
    unsigned int temp;
    unsigned char i,j;
    
    for(i=0;i<size-1;i++)  
    {  
        for(j=0;j<size-1;j++)  
        {  
            if(array[j]>array[j+1])  
            {  
                temp=array[j];  
                array[j]=array[j+1];  
                array[j+1]=temp;  
            }  
        }  
    }    
} 
/*******************************************************************************
* Function Name : bubbleClear
* Description   : 
* Input         : None
* Output        : None.
* Return        : None.
*******************************************************************************/
void bubbleClear(unsigned short int array[],unsigned char size )//������������� 
{  
    unsigned char i;
    for(i=0;i<size;i++)  
    {  
       array[i] = 0; 
    }    
} 
/*******************************************************************************
* Function Name : bubbleClear
* Description   : 
* Input         : None
* Output        : None.
* Return        : None.
*******************************************************************************/
unsigned short int bubbleAverage(unsigned short int array[],unsigned char size )//���������ƽ���� 
{  
  unsigned long ulSum;
  unsigned char i;
	ulSum = 0;
  for(i=0;i<size;i++){  
    ulSum += array[i]; 
  }
  return(ulSum/size);    
} 
/*******************************************************************************
* Function Name : SetPwmHalf
* Description   : Set fg_uiTIM3_Pulse into half
* Input         : None
* Output        : None.
* Return        : None.
*******************************************************************************/
void SetPwmHalf(void)
{
	fg_uiTIM3_Pulse = fg_uiTIM3_Pulse>>2;
}
/*******************************************************************************
* Function Name : main
* Description   : main function
* Input         : None
* Output        : None.
* Return        : None.
*******************************************************************************/
int main(void)
{    
	EcmWorkState 	eWorkMode = START_UP;		// ��ʼ��״̬��
	unsigned char   ucFaultFlag = 0;            // ���ϱ�־: 0:�޹��ϣ�1���й���
	unsigned long   ulTemperatureSum = 0;
	unsigned short  int uiMeasureCouter = 0;
	unsigned char   ucCnt;
	unsigned short  int uiVinAdValue[8];
	unsigned short  int uiVinAdValueTemp;
	unsigned char   ucVinAdCnt = 0;
#if QUICK_OFF_EN == 1
	unsigned char   ucQuickOffFlag = 0;
#endif
/* Configure the system clocks */															   
	RCC_Configuration();
	NVIC_Configuration();
	Delay(10);
	FunComp_Init();
	GPIO_Interrupt();
/* TIMER initialize */
	TIM_Init();
	PWM_OFF;
/* ���ܼ������� */
#ifndef ENERGY_TEST_EN
  #error "Please define ENERGY_TEST_EN!"
#elif   ENERGY_TEST_EN == 1
  g_ulEnergySaveCnt = CheckAllEnergyPage();
#endif
   /* GPIO initialize */
  IO_Init();
	bubbleClear(uiVinAdValue,8);
/**����ģʽ���̶�Ƶ�����**/
#if  PWM_TEST == 1
	TIM1->ARR = PWM_TEST_PERIOD;
	TIM1->CCR1 = TIM1->ARR>>1;
	TIM_Cmd(TIM2, ENABLE);
	TIM_Cmd(TIM1, ENABLE);       /* ������ʱ������PWM */
	PWM_ON;
	TIM3_PWM_SET(TIM3_PERIOD*0.5);//
	fg_ucOrderOnOff = 1; 
	while(1){
	}
#endif
//repair
#if  PARM_REPAIR_EN == 1 
	ReadRepairParmAll(&g_sRepairParmAll, PARAM_REPAIR_ADR1);
	if(g_sRepairParmAll.uiStartFlag != WRITE_SUCCESS_FLAG || g_sRepairParmAll.uiStartFlag != WRITE_SUCCESS_FLAG){
		SetRepairParmAllToDefault(&g_sRepairParmAll);
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
		FLASH_ErasePage(PARAM_REPAIR_ADR1);
		WriteRepairParmAll(g_sRepairParmAll, PARAM_REPAIR_ADR1);
		FLASH_Lock();
	}
	SetParamAtStart();
	Delay(1);
  #if SEECRET_EN == 1
	ReadDataFromFlash(g_ucUniqueDeviceIdTemp, DEVICE_ID_SAVE_ADR, 12); //���������ID
	ReadDeviceID(g_ucUniqueDeviceId); //����MCUΨһID
	for(ucCnt = 0; ucCnt < 12;ucCnt++){
	  if(g_ucUniqueDeviceId[ucCnt] != g_ucUniqueDeviceIdTemp[ucCnt]){
		  break;
	  }
	}
	if(ucCnt == 12){
	  g_ucLisenceEn = 1;
	}else{
	 /* �жϴ洢���Ƿ���0XFF */
	  for(ucCnt = 0; ucCnt < 12;ucCnt++){
		  if(g_ucUniqueDeviceIdTemp[ucCnt] != 0XFF){
			  break;
		  }
	  }
	 /* ��������� */
	  if(ucCnt != 12){
		  FLASH_Unlock();
		  FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
		  FLASH_ErasePage(DEVICE_ID_SAVE_ADR);
		  FLASH_Lock();
	  }
	  g_ucLisenceEn = 0;
	}
	#endif
#endif
 ///////////////////////////////////
/* UART initialize */
	UART_Init();
/* Enable WatchDoog */
	WatvhDog_inti();
    /* DAC initialize */
//    FunDAC_Init();
//    DAC_SetChannel1Data(DAC_Align_12b_R, 3072);  /* ���û�׼Ϊ2.5V */
  /* ADC initialize */
  FunADC_Init();	
// Initial the Variable
	VariableInit();
  g_ucMCP41010SendEn = 0;
  fg_uiTIM3_Pulse = 0;
	fg_uiState = 0;
  g_sSysParam.ucLampEn = 1;
  g_sSysParam.iBrightness = DIM_MAX;
	fg_ucOrderOnOff = 1; /* Ĭ��Ϊ���� */
#if REF_ADJUST_MODE == 2  /* ��ʼ��PID */
  InitPiParm();
#endif   
	/* ��ȡ��� */
  fg_ucGroupNumber = ReadGroupNumber();
	//����ʱ��
#if PROTOCOL == GRT_V15
	/* ��ȡ����ʱ�� */
	ReadRunTime(&g_ulRunTime, &g_ucRepairEn);
#endif
#if LED_MODULE_OPEN_TEST_EN == 1
    //ReadLedVolHis();
#endif
	eWorkMode = START_UP;
  VrefInit();
  SEL_CF1_VOLTAGE;
    //SEL_CF1_CURRENT;
  Delay(10);
  LED_OFF;  
  while(1){   
   //���������ѹ����
		if(g_cUART2ReciveComplete == 1){
      g_uiVoltageOut_100mV = (g_sRepairParmAll.fOutVoltageRatio * g_uiVoltageOutAdcValue) * 10;
      g_uiCurrentOut_mA = g_sRepairParmAll.fOutCurrentRatio * g_uiCurrentOutAdcValue + g_sRepairParmAll.fOutCurrentIntercept;
    }
    /* ��������״ָ̬ʾ�� */
    if(g_uiRunCnt >= RUN_DELAY){
      g_uiRunCnt = 0;
      LED_TURN;
    }
		/* Reload IWDG counter */
    IWDG_ReloadCounter();
		/* ϵͳ����״̬�� */
		switch(eWorkMode){
			case START_UP:
        fg_ucBrightnessTrue = g_sSysParam.iBrightness;
				eWorkMode = SOFT_START;
				TIM_Cmd(TIM2, ENABLE);
        TIM_Cmd(TIM1, ENABLE);       /* ������ʱ������PWM */
				break;
			case SOFT_START:
				/* ���������� */
				// ������������״̬
        fg_ucBrightnessTrue = g_sSysParam.iBrightness;
				eWorkMode = NORMAL_WORK;
				break;		
			case NORMAL_WORK:
				/* �����ж� */
        if(g_cUART2ReciveComplete == 1){   /* ��������˲� */
					g_uiRunCnt = 0;
					LED_TURN;
          fg_uiState = ProtectChek(fg_uiState);
          /* �����ж� */
          if(fg_uiState & FAULT_ALL){  /* ���ֹ��� */
            ucFaultFlag = 1;
            if(fg_uiState & (SHORT_CIR | OPEN_LOOP)){ /* ��·�������߿�·�����ͽ���FAULT����ģʽ���Ȼ������ */
               g_uiFaultDelay = 0;
               fg_uiTIM3_Pulse = TIM3_PERIOD * 0.2;
               eWorkMode = FAULT;
               VrefClear();
					  }
          }else{    /* �޹��� */
            ucFaultFlag = 0;
          }
          if(fg_ucDimEn == 1){
         #if   PROTOCOL == GRT_V15
						if(g_ucRepairEn != 0){ //��˥����
              DimLighting(fg_ucBrightnessTrue * CALCULATE_REPAIR(g_ulRunTime));							
            }else{
              DimLighting(fg_ucBrightnessTrue);
            }
         #else
						DimLighting(fg_ucBrightnessTrue);
         #endif
          }
          g_cUART2ReciveComplete = 0;
        }
        /* ��״̬��ֵ��PLCͨ�ŵ�״̬λ */
				if(g_uiTryToRestoreTime >= RESTORT_TIME){//������ʱ
          g_sSysParam.uiState = fg_uiState;
					fg_ucDaGeCnt = 0;
				}else{
				#if LED_MODULE_OPEN_TEST_EN == 1
					if (fg_ucDaGeFlag == 1){  
						if (g_sSysParam.iBrightness & OPEN_LOOP){//��·����
							if (g_uiCurrentOut_mA > CUR_DAGE * 1000){
								fg_ucDaGeFlag = 0;  //ÿ�δ���ֻ�ж�һ��
								if (fg_ucDaGeCnt < 10){
									fg_ucDaGeCnt++;
									if (fg_ucDaGeCnt >= 10){
										fg_ucOpenLoadFlag |= 0X02;
									}
								}
						    }
					    }
					}
				#endif
			  }
				break;
			case FAULT:
				if (g_uiFaultDelay >= FAULT_DELAY){
					fg_uiState &= ~SHORT_CIR;
					fg_uiState &= ~OPEN_LOOP;
					fg_ucBrightnessTrue = g_sSysParam.iBrightness;
					eWorkMode = NORMAL_WORK;
				#if REF_ADJUST_MODE == 2
          InitPiParm();
        #endif
					SetFre();
					g_uiTryToRestoreTime = 0;
					fg_ucDaGeFlag = 1; //�����������
				}
				break;
			default:break;
		}
     /* �ػ���״̬�£������ֵ�λ����ֵ��������ֵ */
		if(g_sSysParam.ucLampEn == 0){
			DimLighting(START_DIM);
			VrefClear();
			fg_ucBrightnessTrue = g_sSysParam.iBrightness;
		}
 /* CSE7759 test */
		if(1 == InputParmTest()){
			fg_ucFlagInputParamComplete = 1;
		}
	#if  PARM_REPAIR_EN == 1 && SEECRET_EN == 1
		if(g_ucLisenceEn == 0){
			if(g_uiDelayTime >= NO_LICENSE_TIME){
			 fg_ucOrderOnOff = 0;  //����ʱ�䵽��ǿ�ƹػ���
			}
		}
	#endif
/* �ϵ���ٹػ�����ֹ���ٿ��ػ�ʱ��������м�� */
#ifndef QUICK_OFF_EN
#error "Please define QUICK_OFF_EN!"
#elif QUICK_OFF_EN == 1
		/* ����ر� */
		if(ucQuickOffFlag == 0){
			if(g_uiVinAC < VOLTAGE_BUS_TO_AD(VOL_AC_DOWN) *1.414){
				ucQuickOffFlag = 1;
			}
		}else{
			if(g_uiVinAC > VOLTAGE_BUS_TO_AD(VOL_AC_RESTART) *1.414){ 
				ucQuickOffFlag = 0;
			}
		}
		if(ucQuickOffFlag == 1){
			ucFaultFlag = 1;
		}
#endif
		/* ���ػ� */
		if(fg_ucOrderOnOff == 1){ /* ����� */
			g_ucPowerState |= ORDER_EN;     /* ����� */
			if(g_sSysParam.ucLampEn == 0){ /* �������������£�������ǹص� */
				if(ucFaultFlag == 0){      /* �޹��ϾͿ��� */
					TurnOnLamp();
					g_sSysParam.ucLampEn = 1;
				}
			}
			if(g_sSysParam.ucLampEn == 1){ /* ���ǿ��� */ 
				if(ucFaultFlag == 1){        /* ���ֹ��� */
				 g_sSysParam.ucLampEn = 0;
				 TurnOffLamp();
				}
			}
		}else{  /* ����ػ� */
			g_ucPowerState &= ~ORDER_EN;
			TurnOffLamp();
			if(g_sSysParam.ucLampEn == 1){
				g_sSysParam.ucLampEn = 0;
			}
		}
/* ���ݵƵ�״̬���ñ�־λ */
		if(g_sSysParam.ucLampEn == 1){ /* ��ʾ����״̬ */
			g_ucPowerState |= LAMP_EN;
		}else{ /* ��ʾ�ػ�״̬ */
			g_ucPowerState &= ~LAMP_EN;
		}
		/* �Ƚ���״̬ */
		if(COMP_GetOutputLevel(COMP_Selection_COMP1) == COMP_OutputLevel_High){
			g_ucPowerState |= COMP_STATE;  /* �Ƚ���״̬ */
		}else{
			g_ucPowerState &= ~COMP_STATE; /* �Ƚ���״̬ */
		}
		/* �ж����PWM�����Ƿ�ﵽ������С */
		if(fg_uiTIM3_Pulse > TIM3_PERIOD -10){
			g_ucPowerState |= PWM_MAX; 
		}else{
			g_ucPowerState &= ~PWM_MAX; 
		}
		if(fg_uiTIM3_Pulse < 10){
			g_ucPowerState |= PWM_MIN; 
		}else{
			g_ucPowerState &= ~PWM_MIN; 
		}
		/* communicate with PLC */
		CommunicateWithPLC();
		//��������
		if(g_uiReduceDelayCnt >= REDUCE_START_DELAY_TIM){
			fg_ucBrightnessTrue = ReducePower(g_sSysParam.uiVoltageIn, fg_cTemperature, g_sSysParam.iBrightness);
			/* �߽����� */
			if(fg_ucBrightnessTrue > DIM_MAX){
				fg_ucBrightnessTrue = DIM_MAX;
			}
			if(fg_ucBrightnessTrue < DIM_MIN){
				fg_ucBrightnessTrue = DIM_MIN;
			}
		}
		/* ��ѹ���¶ȵ�ADC���� */
		if(g_ucAdcCnt >= ADC_TIME){//1msִ��һ��
      ulTemperatureSum += g_uiADC_ConvertedValue[1];
			uiVinAdValueTemp = g_uiADC_ConvertedValue[2];
			if(++uiMeasureCouter >= 128){
				fg_cTemperature = TemperatureCalculate(g_uiTemperature) * 0.8708 + 1.9875;
				g_uiTemperature = ulTemperatureSum >> 7;
				ulTemperatureSum = 0;
				uiMeasureCouter = 0;
				g_ucFlagAdcEnd = 1;
			}
			if(++ucVinAdCnt < 46){
				if(uiVinAdValueTemp > uiVinAdValue[0]){
					uiVinAdValue[0] = uiVinAdValueTemp;
					bubbleSort(uiVinAdValue,8);
				}
			}else{
				ucVinAdCnt = 0;
				g_uiVinAC = bubbleAverage(uiVinAdValue,8);
				bubbleClear(uiVinAdValue,8);
			}
      g_ucAdcCnt = 0;            
    }
#if PROTOCOL == GRT_V15
		/* ����ʱ����� */
		if(g_ucRunTimeUpdata == 1){
			SaveRunTime(g_ulRunTime, g_ucRepairEn);
			g_ucRunTimeUpdata = 0;
	  }
#endif
#if LED_MODULE_OPEN_TEST_EN == 1		
		if(g_uiLedModuleOpenTime >= LED_MODULE_TIME){
			g_uiLedModuleOpenTime = 0;
			if(fg_ucDimValueLast != g_sSysParam.iBrightness){//����󣬴��¼�ʱ
				fg_ucDimValueLast = g_sSysParam.iBrightness;
				fg_uiLedOnTimeDly = 0;
			}
			if(g_sSysParam.ucLampEn == 1){
				if(fg_uiLedOnTimeDly < LED_DLY){
					fg_uiLedOnTimeDly++;
				}else{
					if(g_uiVoltageOut_100mV > fg_uiVolOutLast_100mV + DELTA_VOL){//���뱨��
						fg_ucOpenLoadFlag |= 0x01;
					}
				}
				if(fg_uiLedOnTimeHis <LED_HIS){
					fg_uiLedOnTimeHis++;
				}else{
					//if ()
				}
				fg_uiVolOutLast_100mV = g_uiVoltageOut_100mV;
			}else{
				fg_uiLedOnTimeHis = 0;
				fg_uiLedOnTimeDly = 0;
			}
		}
#endif
/* ���ܼ��� */
#if ENERGY_TEST_EN == 1
		if(g_uiEnergySaveEn){     //
			if((fg_ucCSE7759FlagBuffer & POWER_BIT) && (fg_ucCSE7759FlagBuffer & (CUR_BIT | VOL_BIT))){
			    //�е���������Ҫ���棬Ҫ�ȵ����������֮���ٱ���
			  g_ulEnergySaveCnt += g_uiEnergySaveEn;
				g_uiEnergySaveEn = 0;
				SaveEnergyData(g_ulEnergySaveCnt);
				//����������֮�������־���������	
				g_ucCSE7759Flag &= ~POWER_BIT;
				g_ucCSE7759Flag &= ~(CUR_BIT | VOL_BIT);
				fg_ucCSE7759FlagBuffer = 0;
			}
		}else{
			//�����־���������	
			if (fg_ucCSE7759FlagBuffer & POWER_BIT){
				g_ucCSE7759Flag &= ~POWER_BIT;
				fg_ucCSE7759FlagBuffer &= ~POWER_BIT;
			}
			if (fg_ucCSE7759FlagBuffer & CUR_BIT){
				g_ucCSE7759Flag &= ~CUR_BIT;
				fg_ucCSE7759FlagBuffer &= ~CUR_BIT;
			}
			if (fg_ucCSE7759FlagBuffer & VOL_BIT){
				g_ucCSE7759Flag &= ~VOL_BIT;
				fg_ucCSE7759FlagBuffer &= ~VOL_BIT;
			}		
		}
#endif	
		/* Send data to PLC */
		SendDataToPLC();
		/* ��ѭ�����ڿ��ƣ�g_ucMainCycle��ʱ���ж�����1 */
		/*while(g_ucMainCycle == 0);
		g_ucMainCycle = 0;*/
	}
}

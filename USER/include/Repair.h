
#include "main.h"
#ifndef __REPAIR_H__
#define __REPAIR_H__

//===========================================================
//Ĭ�ϲ��� OR �ֶ�У������
#ifndef HEND_EN
  #error "Please define HEND_EN!"
#elif  HEND_EN == 0  //�������У��ֵĬ��ֵ
  #ifndef SR_IN
    #error "Please define SR_IN!"
  #elif SR_IN == 10                             /* �����������Ϊ10m�� */
	#define VT  0.1714
	#define AT  0.001488
	#define PT  0.4446       /* 1sһ�����壨��Ƶ��Ϊ1Hz��ʱ����Ӧ�Ĺ��� */
	#define TIME_CNT_DEFAULT   1000
	#define PLUSE_CNT_DEFAULT  1
	#define VT_DEFAULT_H     ((VT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*1.3)
	#define VT_DEFAULT_L     ((VT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*0.7)
	#define AT_DEFAULT_H     ((AT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*1.3)    
	#define AT_DEFAULT_L     ((AT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*0.7)
	#define PT_DEFAULT_H     ((PT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*1.3)    
	#define PT_DEFAULT_L     ((PT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*0.7)
  #elif SR_IN == 20                             /* �����������Ϊ20m�� */
	#define VT  0.1714
	#define AT  0.000744
	#define PT  0.2223       /* 1sһ�����壨��Ƶ��Ϊ1Hz��ʱ����Ӧ�Ĺ��� */
	#define TIME_CNT_DEFAULT   1000
	#define PLUSE_CNT_DEFAULT  1
	#define VT_DEFAULT_H     ((VT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*1.3)
	#define VT_DEFAULT_L     ((VT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*0.7)
	#define AT_DEFAULT_H     ((AT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*1.3)    
	#define AT_DEFAULT_L     ((AT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*0.7)
	#define PT_DEFAULT_H     ((PT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*1.3)    
	#define PT_DEFAULT_L     ((PT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*0.7)
  #else 
    #error "Please Set SR_IN 0.01 or 0.02!"
  #endif
#elif  HEND_EN == 1  //ʹ���ֶ�У���Ĳ���
  #define   IN_POWER_RATIO_DEFAULT            0.3941
	#define   IN_POWER_INTERCEPT_DEFAULT        -0.7
	#define   IN_VOLTAGE_RATIO_DEFAULT          0.16833
	#define   IN_VOLTAGE_INTERCEPT_DEFAULT      1.7
	#define   IN_CURRENT_RATIO_DEFAULT          0.00132966
	#define   IN_CURRENT_INTERCEPT_DEFAULT      0.030
#endif
//�������У��ֵĬ��ֵ
#define   OUT_POWER_INTERCEPT_DEFAULT       0
#define   OUT_VOLTAGE_RATIO_DEFAULT         (AD_TO_VOLTAGE_OUT(1))  //0.074067
#define   OUT_VOLTAGE_INTERCEPT_DEFAULT     0
#define   OUT_CURRENT_RATIO_DEFAULT         (AD_TO_CURRENT_OUT(1)*1000)  //0.0034292
#define   OUT_CURRENT_INTERCEPT_DEFAULT     0
//===========================================================
/* �����ƾ�У�� */
#if  PARM_REPAIR_EN == 1  
#ifndef   POWER_TYPE
  #error "Please define POWER_TYPE!"
#else
  #define  WRITE_SUCCESS_FLAG               POWER_TYPE   //�õ�Դ��������Ϊ��־�������ڵ�Դ�޸�Ϊ�������͵ĵ�Դʱ���Զ��ָ�ΪĬ�ϲ���
#endif
//�����Ĭ��ֵ
#define   RATE_POWER_DEFAULT                POW_LIMIT  //  
#define   RATE_VOLTAGE_DEFAULT              VOL_RATED  //     
#define   RATE_CURRENT_DEFAULT              CUR_RATED  //
/* У����Χ */
#define   RATE_MAX            1.3   //�ֵĬ��ֵ��1.3��
#define   RATE_MIN            0.7   //�ֵĬ��ֵ��0.7��
#define   RATIO_MAX           1.3   //б��Ĭ��ֵ��1.3��
#define   RATIO_MIN           0.7   //б��Ĭ��ֵ��0.7��
#define   INTERCEPT_MAX       8
#define   INTERCEPT_MIN       (-8)
/* У��״̬λ */
#define   IPA                0X0001
#define   IVA                0X0002
#define   ICA                0X0004
#define   OVA                0X0008
#define   OCA                0X0010
//union  define
typedef union
{
  float  f;
  long   l;
}UcmRepairParam;
//struct define
typedef struct 
{   /* start flag */
    unsigned short int        uiStartFlag;

    /* input parm */
    UcmRepairParam            uPowerCheck;         /* ?? */
    unsigned short int        uiPowerCntCheck;      /* ?? */
	unsigned short int        uiPowerTimeCheck;      /* ?? */
	
	UcmRepairParam            uInVoltageCheck;         /* ?? */
    unsigned short int        uiInVoltageCntCheck;      /* ?? */
	unsigned short int        uiInVoltageTimeCheck;      /* ?? */
	
    UcmRepairParam            uInCurrentCheck;         /* ?? */
    unsigned short int        uiInCurrentCntCheck;      /* ?? */
	unsigned short int        uiInCurrentTimeCheck;         /* ?? */

    /* output parm */
    float                     fOutVoltageRatio;           /* ?? */
    float                     fOutCurrentRatio;           /* ?? */
	float                     fOutCurrentIntercept;           /* ?? */
    /* repair state */
    unsigned char             ucParmState;            /* ????? */
	/* repair state */
    unsigned char             ucRepairError;            /* ????? */
    /* end flag */
    unsigned short int        uiEndFlag;
}ScmRepairParamAll;
#define   PARM_REPAIR_LONG       (sizeof(ScmRepairParamAll))   //
//????
#define   PARAM_REPAIR_ADR1             0x0800F000   //????(?????REPAIR_PARAM_EEPRO_START??)
#define   PARAM_REPAIR_COPY_ADR1        0x0800F400   //??????
#define   DEVICE_ID_SAVE_ADR            0X0800EC00   //

//��������
unsigned char ReadRepairParmAll(ScmRepairParamAll *sRepairParam, unsigned long uiAdr);
void WriteRepairParmAll(ScmRepairParamAll sRepairParam, unsigned long ulAdr);
void SetRepairParmAllToDefault(ScmRepairParamAll *sRepairParam);
void SaveRepairParmToBothArea(ScmRepairParamAll sRepairParam);
unsigned char CheckUartRecive(unsigned char ucTotal);
void ParmRepairAck4(unsigned char ucOrder, unsigned char ucAck);
void SetParamAtStart(void);
/* ���õ��ⲿ���� */
extern unsigned char               g_ucSendData[];
extern volatile unsigned char      g_ucReciveData[];
extern unsigned char               g_ucSendSum;
extern volatile ScmSysParam        g_sSysParam;
#elif PARM_REPAIR_EN == 0
//��ʹ��У����PARM_REPAIR_EN = 0��ʱʹ�����·��������������
#endif
//end
#endif

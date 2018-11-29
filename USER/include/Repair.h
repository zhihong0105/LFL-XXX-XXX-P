
#include "main.h"
#ifndef __REPAIR_H__
#define __REPAIR_H__

//===========================================================
//默认参数 OR 手动校正参数
#ifndef HEND_EN
  #error "Please define HEND_EN!"
#elif  HEND_EN == 0  //输入参数校正值默认值
  #ifndef SR_IN
    #error "Please define SR_IN!"
  #elif SR_IN == 10                             /* 输入采样电阻为10mΩ */
	#define VT  0.1714
	#define AT  0.001488
	#define PT  0.4446       /* 1s一个脉冲（即频率为1Hz）时所对应的功率 */
	#define TIME_CNT_DEFAULT   1000
	#define PLUSE_CNT_DEFAULT  1
	#define VT_DEFAULT_H     ((VT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*1.3)
	#define VT_DEFAULT_L     ((VT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*0.7)
	#define AT_DEFAULT_H     ((AT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*1.3)    
	#define AT_DEFAULT_L     ((AT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*0.7)
	#define PT_DEFAULT_H     ((PT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*1.3)    
	#define PT_DEFAULT_L     ((PT * TIME_CNT_DEFAULT / PLUSE_CNT_DEFAULT)*0.7)
  #elif SR_IN == 20                             /* 输入采样电阻为20mΩ */
	#define VT  0.1714
	#define AT  0.000744
	#define PT  0.2223       /* 1s一个脉冲（即频率为1Hz）时所对应的功率 */
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
#elif  HEND_EN == 1  //使用手动校正的参数
  #define   IN_POWER_RATIO_DEFAULT            0.3941
	#define   IN_POWER_INTERCEPT_DEFAULT        -0.7
	#define   IN_VOLTAGE_RATIO_DEFAULT          0.16833
	#define   IN_VOLTAGE_INTERCEPT_DEFAULT      1.7
	#define   IN_CURRENT_RATIO_DEFAULT          0.00132966
	#define   IN_CURRENT_INTERCEPT_DEFAULT      0.030
#endif
//输出参数校正值默认值
#define   OUT_POWER_INTERCEPT_DEFAULT       0
#define   OUT_VOLTAGE_RATIO_DEFAULT         (AD_TO_VOLTAGE_OUT(1))  //0.074067
#define   OUT_VOLTAGE_INTERCEPT_DEFAULT     0
#define   OUT_CURRENT_RATIO_DEFAULT         (AD_TO_CURRENT_OUT(1)*1000)  //0.0034292
#define   OUT_CURRENT_INTERCEPT_DEFAULT     0
//===========================================================
/* 测试制具校正 */
#if  PARM_REPAIR_EN == 1  
#ifndef   POWER_TYPE
  #error "Please define POWER_TYPE!"
#else
  #define  WRITE_SUCCESS_FLAG               POWER_TYPE   //用电源类型来作为标志，可以在电源修改为其他类型的电源时，自动恢复为默认参数
#endif
//额定参数默认值
#define   RATE_POWER_DEFAULT                POW_LIMIT  //  
#define   RATE_VOLTAGE_DEFAULT              VOL_RATED  //     
#define   RATE_CURRENT_DEFAULT              CUR_RATED  //
/* 校正范围 */
#define   RATE_MAX            1.3   //额定值默认值的1.3倍
#define   RATE_MIN            0.7   //额定值默认值的0.7倍
#define   RATIO_MAX           1.3   //斜率默认值的1.3倍
#define   RATIO_MIN           0.7   //斜率默认值的0.7倍
#define   INTERCEPT_MAX       8
#define   INTERCEPT_MIN       (-8)
/* 校正状态位 */
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

//函数声明
unsigned char ReadRepairParmAll(ScmRepairParamAll *sRepairParam, unsigned long uiAdr);
void WriteRepairParmAll(ScmRepairParamAll sRepairParam, unsigned long ulAdr);
void SetRepairParmAllToDefault(ScmRepairParamAll *sRepairParam);
void SaveRepairParmToBothArea(ScmRepairParamAll sRepairParam);
unsigned char CheckUartRecive(unsigned char ucTotal);
void ParmRepairAck4(unsigned char ucOrder, unsigned char ucAck);
void SetParamAtStart(void);
/* 引用的外部变量 */
extern unsigned char               g_ucSendData[];
extern volatile unsigned char      g_ucReciveData[];
extern unsigned char               g_ucSendSum;
extern volatile ScmSysParam        g_sSysParam;
#elif PARM_REPAIR_EN == 0
//不使能校正（PARM_REPAIR_EN = 0）时使用以下方法计算输入参数
#endif
//end
#endif

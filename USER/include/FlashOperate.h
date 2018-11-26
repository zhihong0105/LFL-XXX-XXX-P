#ifndef __FLASH_OPERATE_H__
#define __FLASH_OPERATE_H__
#include "main.h"
/* FLASH�洢�궨�� */
#define FLASH_WRITE_FLAG            0X53           /* FLASH ��д��־ */
#define FLASH_START                 0x0800FC00     /* FLASH �洢��ַ */

#ifndef LED_MODULE_OPEN_TEST_EN
  #error "Plese define LED_MODULE_OPEN_TEST_EN!"
#elif LED_MODULE_OPEN_TEST_EN == 1
  #define  LED_DATA_START_FLAG      0X5356         /* The flag of start */
  #define  LED_DATA_END_FLAG        0X1247         /* The flag of end */
  #define  LED_VOL_HIS_ADR          0X0800E800     /* LED���������ѹ�����ַ */
  #define  LED_VOL_HIS_COPY_ADR     0X0800E400     /* LED���������ѹ���ݱ����ַ */
#endif
/* ���ܴ洢 */
#if   ENERGY_TEST_EN == 1
#define ENERGY_SAVE_PAGE_NUM        10   
#define ENERGY_SAVE_END_ADR         (0X0800E400-1)  /* ������ַ���½�LED_VOL_HIS_COPY_ADR */   
#define ENERGY_SAVE_START_ADR       (ENERGY_SAVE_END_ADR - 1024 * ENERGY_SAVE_PAGE_NUM + 1) /* ��ʼ��ַ */ 
#define ENERGY_START_FLAG           0X1986
#define ENERGY_END_FLAG             0X1984
typedef struct{  
	unsigned long       ulEnergy;
	unsigned short int  uiEndFlag;
	unsigned short int  uiStartFlag;
}ScmEnergy;
#define ENERGY_DATA_LONG            8//sizeof(ScmEnergy) ����Ϊ8
#endif
        

void ReadDataFromFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum);
void WriteDataToFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum);
unsigned char FlashWriteCheck(void);

unsigned char ReadGroupNumber(void);
unsigned char SetGroupNumber(unsigned char ucNewGroup);
unsigned char SetGroupNumberAtStart(unsigned char ucNewGroup);
#ifndef GRT_V15
  #error "Please define GRT_V15!"
#elif   PROTOCOL == GRT_V15
  unsigned char SaveRunTime(unsigned long uiRunTime, unsigned char ucRepairEn);
  void ReadRunTime(unsigned long *ulRunTime, unsigned char *ucRepairEn);
#endif

#ifndef LED_MODULE_OPEN_TEST_EN
  #error "Plese define LED_MODULE_OPEN_TEST_EN!"
#elif LED_MODULE_OPEN_TEST_EN == 1
  unsigned char SetLedVolHis(unsigned long ulFlashAdr, unsigned short int uiVol);
  ScmLedData ReadLedVolHis(unsigned long ulFlashAdr, unsigned char ucEn);
#endif

#if   ENERGY_TEST_EN == 1
unsigned long  CheckAllEnergyPage(void);
void SaveEnergyData(unsigned long ulEnergyData);
unsigned long ReadEnergyDataFromFlash(void);
#endif
#endif

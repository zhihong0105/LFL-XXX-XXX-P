#include "stm32f0xx.h"
#include "FlashOperate.h"
#include "main.h"

static void Delay(unsigned short int uiTime)
{
	volatile static unsigned short int s_uiCnt;
	while(uiTime){
		for(s_uiCnt = 10000;s_uiCnt>0;s_uiCnt--);
		uiTime--;
		IWDG_ReloadCounter();
	}
}
/*******************************************************************************
* Function Name : WriteHalfWordToFlash
* Description   : Write 16 bit to Flash
* Input         : uiWord
* Output        : None.
* Return        : None.
*******************************************************************************/
static void WriteHalfWordToFlash(unsigned short int uiWord, unsigned long ulFlashAdr)
{
	 unsigned char ucTemp[2];
	 //��λ����ڵ͵�ַ
	 ucTemp[0] = (unsigned char)(uiWord>>8);   
	 ucTemp[1] = (unsigned char)uiWord;
	 WriteDataToFlash(&ucTemp[0], ulFlashAdr, 2);
}

/*******************************************************************************
* Function Name : ReadDataFromFlash
* Description   : Read data from Flash
* Input         : *RamAdrStar   uiSum: total of bytes reading from FLASH
* Output        : None.
* Return        : None.
*******************************************************************************/
void ReadDataFromFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum)
{
	 unsigned short int uiCnt;
	 for(uiCnt = 0; uiCnt < uiSum; uiCnt++){
	  //   *(RamAdrStar + uiCnt) = *((__IO uint32_t*)(ulFlashAdr + uiCnt));
		 *(RamAdrStar + uiCnt) = *((unsigned char *)(ulFlashAdr + uiCnt));
	 }
}
/*******************************************************************************
* Function Name : WriteDataToFlash
* Description   : Write data to Flash
* Input         : *RamAdrStar:RAM     uiSum: total of bytes writting to FLASH
* Output        : None.
* Return        : None.
*******************************************************************************/
void WriteDataToFlash(unsigned char *RamAdrStar, unsigned long ulFlashAdr, unsigned short int uiSum)
{
	 unsigned short int uiCnt;
	 FLASH_Unlock();
	 FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
	 //FLASH_ErasePage(FLASH_START);
	 for(uiCnt = 0; uiCnt < uiSum; uiCnt += 2){    /* write 2 bytes every time */
	     FLASH_ProgramHalfWord(ulFlashAdr + uiCnt, (*(RamAdrStar + uiCnt + 1)<<8) + *(RamAdrStar + uiCnt));
	 }
	 FLASH_Lock();
}

/*******************************************************************************
* Function Name : SetGroupNumberAtStart
* Description   : ���ýڵ����ţ���ű�������ʼ��ַ
* Input         : ucNewGroup���µ����
* Output        : None.
* Return        : 0������ʧ�ܣ�1�����óɹ�
*******************************************************************************/
unsigned char SetGroupNumberAtStart(unsigned char ucNewGroup)
{
	 unsigned char ucReturnValue = 0,ucTemp[2];
	 unsigned short int uiTemp;
	 FLASH_Unlock();
	 FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
	 FLASH_ErasePage(FLASH_START);
	 uiTemp = FLASH_WRITE_FLAG;
	 uiTemp = uiTemp<<8;
	 uiTemp |= ucNewGroup;
	 WriteHalfWordToFlash(uiTemp, FLASH_START);
	 FLASH_Lock();
	 ReadDataFromFlash(&ucTemp[0], FLASH_START, 2);
	 if (ucTemp[1] == ucNewGroup && ucTemp[0] == FLASH_WRITE_FLAG){
		   ucReturnValue = 1;
	 }
	 return(ucReturnValue);
}

/*******************************************************************************
* Function Name : SetGroupNumber
* Description   : ���ýڵ�����
* Input         : ucNewGroup���µ����
* Output        : None.
* Return        : 0������ʧ�ܣ�1�����óɹ�
*******************************************************************************/
unsigned char SetGroupNumber(unsigned char ucNewGroup)
{
	  unsigned char ucReturnValue = 0 ,ucTemp[2];
	  unsigned short int uiCnt, uiTemp;
	  FLASH_Unlock();
	  FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
	
	  /* û�����ù���žͷ���0 */
	  for(uiCnt = 0; uiCnt < 1000; uiCnt += 2){
	      ReadDataFromFlash(&ucTemp[0], FLASH_START + uiCnt,2);
		  if (ucTemp[0] == 0xff && ucTemp[1] == 0xff){
			  uiTemp = FLASH_WRITE_FLAG;
	          uiTemp = uiTemp<<8;
	          uiTemp |= ucNewGroup;
	          WriteHalfWordToFlash(uiTemp, FLASH_START + uiCnt);
			  ucReturnValue = 1;
			  break;
		  }
	  }
	  FLASH_Lock();
	  return(ucReturnValue);
}
/*******************************************************************************
* Function Name : ReadGroup
* Description   : ReadGroup
* Input         : None.
* Output        : None.
* Return        : ���
*******************************************************************************/
unsigned char ReadGroupNumber(void)
{
	 unsigned char ucReturnValue = 0 ,ucTemp[2];
	 unsigned short int uiCnt;
	/* û�����ù���žͷ���0 */
	 for(uiCnt = 0; uiCnt < 1000; uiCnt += 2){
	     ReadDataFromFlash(&ucTemp[0], FLASH_START + uiCnt,2);
		 if (ucTemp[0] != FLASH_WRITE_FLAG){
			 break;
		 }
		 ucReturnValue = ucTemp[1];
	 }
	 if (uiCnt >= 800){
		 SetGroupNumberAtStart(ucReturnValue);
	 }
	 return(ucReturnValue);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if    PROTOCOL == GRT_V15
/*******************************************************************************
* Function Name : SaveRunTime
* Description   : ��������ʱ��
* Input         : uiRunTime������ʱ�䣻ucRepairEn��0��1
* Output        : None.
* Return        : 0��ʧ�ܣ�1���ɹ�
*******************************************************************************/
unsigned char SaveRunTime(unsigned long uiRunTime, unsigned char ucRepairEn)
{
	  unsigned char ucReturnValue = 0 ,ucTemp[4];
	  unsigned short int uiCnt, uiTemp;
	  FLASH_Unlock();
	  FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
	  /* û�б��������ʱ��ͷ���0 */
	  for(uiCnt = 0; uiCnt < 1000; uiCnt += 4){
	      ReadDataFromFlash(&ucTemp[0], SAVE_RUN_TIME_ADR + uiCnt,4);
		  if (ucTemp[0] == 0xff && ucTemp[1] == 0xff && ucTemp[2] == 0xff && ucTemp[3] == 0xff){
			  uiTemp = SAVE_FLAG;
			  if (ucRepairEn != 0){
				  uiTemp |= 0x80;
			  }
	          uiTemp = uiTemp<<8;
			  uiTemp |= (unsigned char)(uiRunTime >> 16);
	          WriteHalfWordToFlash(uiTemp, SAVE_RUN_TIME_ADR + uiCnt);
			  uiTemp = (unsigned char)(uiRunTime >> 8);
	          uiTemp = uiTemp<<8;
			  uiTemp |= (unsigned char)uiRunTime;
			  WriteHalfWordToFlash(uiTemp, SAVE_RUN_TIME_ADR + uiCnt + 2);
			  ucReturnValue = 1;
			  break;
		  }
	  }
	  if(uiCnt > 1000){//�Ѿ������ٲ���
		  FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
		  FLASH_ErasePage(SAVE_RUN_TIME_ADR);
		  uiTemp = SAVE_FLAG;
		  if (ucRepairEn != 0){
			  uiTemp |= 0x80;
		  }
		  uiTemp = uiTemp<<8;
		  uiTemp |= (unsigned char)(uiRunTime >> 16);
		  WriteHalfWordToFlash(uiTemp, SAVE_RUN_TIME_ADR);
		  uiTemp = (unsigned char)(uiRunTime >> 8);
		  uiTemp = uiTemp<<8;
		  uiTemp |= (unsigned char)uiRunTime;
		  WriteHalfWordToFlash(uiTemp, SAVE_RUN_TIME_ADR + 2);
		  FLASH_Lock();
	  }
	  FLASH_Lock();
	  return(ucReturnValue);
}
/*******************************************************************************
* Function Name : ReadRunTime
* Description   : ��������ʱ��
* Input         : ulRunTime������ʱ���ַ��ucRepairEn����˥ʹ�ܵ�ַ
* Output        : None.
* Return        : 0��ʧ�ܣ�1���ɹ�
*******************************************************************************/
void ReadRunTime(unsigned long *ulRunTime, unsigned char *ucRepairEn)
{
	 unsigned char ucTemp[4];
	 unsigned short int uiTemp;
	 unsigned long ulRunTimeTemp = 0;
	 unsigned short int uiCnt;
	/* û�����ù���žͷ���0 */
	 for(uiCnt = 0; uiCnt < 1000; uiCnt += 4){
	     ReadDataFromFlash(&ucTemp[0], SAVE_RUN_TIME_ADR + uiCnt,4);
		 if ((ucTemp[0] & ~0x80) != SAVE_FLAG){
		     break;
	     }
		 ulRunTimeTemp = ucTemp[0]; 
		 ulRunTimeTemp <<= 8;
		 ulRunTimeTemp |= ucTemp[1];
		 ulRunTimeTemp <<= 8;
		 ulRunTimeTemp |= ucTemp[2];
		 ulRunTimeTemp <<= 8;
		 ulRunTimeTemp |= ucTemp[3];
	 }
	 if (uiCnt == 0){ //�洢����û�д洢���ݣ��ͽ�����ʱ���Ϊ0
		 FLASH_Unlock();
		 FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
		 FLASH_ErasePage(SAVE_RUN_TIME_ADR);
		 uiTemp = SAVE_FLAG | DEFAULT_REPAIR_EN;
		 uiTemp = uiTemp<<8;
		 uiTemp |= 0;
		 WriteHalfWordToFlash(uiTemp, SAVE_RUN_TIME_ADR);
		 uiTemp = 0;
		 uiTemp = uiTemp<<8;
		 uiTemp |= 0;
		 WriteHalfWordToFlash(uiTemp, SAVE_RUN_TIME_ADR + 2);
		 FLASH_Lock();
     }else{
		 if (uiCnt >= 800 && uiCnt < 1000){ //�����ݴ���ڳ�ʼ��ַ
			 FLASH_Unlock();
			 FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
			 FLASH_ErasePage(SAVE_RUN_TIME_ADR);
			 uiTemp = ucTemp[0];
			 uiTemp = uiTemp<<8;
			 uiTemp |= ucTemp[1];
			 WriteHalfWordToFlash(uiTemp, SAVE_RUN_TIME_ADR);
			 uiTemp = ucTemp[2];
			 uiTemp = uiTemp<<8;
			 uiTemp |= ucTemp[3];
			 WriteHalfWordToFlash(uiTemp, SAVE_RUN_TIME_ADR + 2);
			 FLASH_Lock();
		 }
     }
	 /* ����ȡ�����ݸ�ֵ��ʵ�ʱ��� */
	 *ulRunTime = ulRunTimeTemp & 0x00ffffff;
	 if (ulRunTimeTemp & 0x80000000){
		*ucRepairEn = 1; 
	 }else{
		*ucRepairEn = 0;  
     }
}
#endif

#ifndef LED_MODULE_OPEN_TEST_EN
  #error "Plese define LED_MODULE_OPEN_TEST_EN!"
#elif LED_MODULE_OPEN_TEST_EN == 1
/*******************************************************************************
* Function Name : SetLedVolAtStart
* Description   : Set LED output voltage at the start of FLASH.
* Input         : ulAdr��Adress    uiData��data to be written.
* Output        : None.
* Return        : 0������ʧ�ܣ�1�����óɹ�
*******************************************************************************/
static unsigned char SetLedVolAtStart(unsigned long ulAdr, unsigned short int uiData)
{
	 unsigned char ucReturnValue = 0,ucTemp[2];
	 unsigned short int uiTemp;
	 ScmLedData  sLedDataTemp;
	 FLASH_Unlock();
	 FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
	 FLASH_ErasePage(ulAdr);
	 sLedDataTemp.uiStartFlag = LED_DATA_START_FLAG;
	 sLedDataTemp.uiVoloutHis = uiData;
	 sLedDataTemp.uiEndFlag = LED_DATA_END_FLAG;
	 WriteDataToFlash((unsigned char*)(&sLedDataTemp), ulAdr, LED_DATA_LONG);
	 FLASH_Lock();
	 sLedDataTemp.uiStartFlag = 0;
	 sLedDataTemp.uiVoloutHis = 0;
	 sLedDataTemp.uiEndFlag = 0;
	 /* ��ȡ��֤ */
	 ReadDataFromFlash((unsigned char*)(&sLedDataTemp), ulAdr, LED_DATA_LONG);
	 if (sLedDataTemp.uiStartFlag == LED_DATA_START_FLAG && 
		 sLedDataTemp.uiVoloutHis == uiData &&
	     sLedDataTemp.uiEndFlag   == LED_DATA_END_FLAG){
		 ucReturnValue = 1;
	 }
	 return(ucReturnValue);
}

/*******************************************************************************
* Function Name : SetLedVolHis
* Description   : Save LED output voltage to FLASH
* Input         : uiVol: Output Voltage to be saved.
* Output        : None.
* Return        : 0������ʧ�ܣ�1�����óɹ�
*******************************************************************************/
unsigned char SetLedVolHis(unsigned long ulFlashAdr, unsigned short int uiVol)
{
	  unsigned char ucReturnValue = 0 ,ucTemp[2];
	  ScmLedData  sLedDataTemp;
	  unsigned short int uiCnt, uiTemp;
	  FLASH_Unlock();
	  FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
	  /* û�����ù���žͷ���0 */
	  for(uiCnt = 0; uiCnt < 1023-LED_DATA_LONG; uiCnt += LED_DATA_LONG){
	      ReadDataFromFlash((unsigned char*)(&sLedDataTemp), ulFlashAdr + uiCnt,LED_DATA_LONG);
		  if (sLedDataTemp.uiStartFlag == 0xffff && sLedDataTemp.uiEndFlag == 0xffff){
			  sLedDataTemp.uiStartFlag = LED_DATA_START_FLAG;
	          sLedDataTemp.uiVoloutHis = uiVol;
			  sLedDataTemp.uiEndFlag = LED_DATA_END_FLAG;
			  WriteDataToFlash((unsigned char*)(&sLedDataTemp), ulFlashAdr + uiCnt, LED_DATA_LONG);
			  ucReturnValue = 1;
			  break;
		  }
	  }
	  FLASH_Lock();
	  return(ucReturnValue);
}
/*******************************************************************************
* Function Name : ReadLedVolHis
* Description   : Read LED history output voltage from FLASH
* Input         : ucEn: 0:������ȡ��1�����洢�Ŀռ䲻���ʱ�򣬽����ݱ�������ʼλ��
* Output        : None.
* Return        : ��ʼ��־��ʱ�䣬������־
*******************************************************************************/
ScmLedData ReadLedVolHis(unsigned long ulFlashAdr, unsigned char ucEn)
{
	 unsigned short int uiCnt;
	 ScmLedData  sLedDataReturn, sLedDataTemp;
     sLedDataReturn.uiStartFlag = 0;	
	 /* ��ȡ���� */
	 for(uiCnt = 0; uiCnt < 1023 - LED_DATA_LONG; uiCnt += LED_DATA_LONG){
	     ReadDataFromFlash((unsigned char*)(&sLedDataTemp), ulFlashAdr + uiCnt,LED_DATA_LONG);
		 if (sLedDataTemp.uiStartFlag != LED_DATA_START_FLAG){
			 break;
		 }
		 sLedDataReturn = sLedDataTemp;
	 }
	 if (ucEn == 1 && sLedDataReturn.uiStartFlag == LED_DATA_START_FLAG){
		 if (uiCnt >= 800){
			 SetLedVolAtStart(ulFlashAdr, sLedDataReturn.uiVoloutHis);
		 }
     }
	 /* û�б��������ݣ�startFlag = 0 */
	 return(sLedDataReturn);
}
#endif


/* ���ܼ������� */
#ifndef ENERGY_TEST_EN
  #error "Please define ENERGY_TEST_EN!"
#elif   ENERGY_TEST_EN == 1
static unsigned long fg_ulNextEnergyAdr;  /* The next energy data save adress. */
/*******************************************************************************
* Function Name : ReadEnergyData
* Description   : Read one enerty data from flash.
* Input         : ulAdr: start adress.
* Output        : None.
* Return        : Read one enerty data.
*******************************************************************************/
static ScmEnergy ReadEnergyData(unsigned long ulAdr)
{
	ScmEnergy sReturnValue;
	unsigned short int uiStart,uiEnd;
    unsigned long  ulData;	
	ReadDataFromFlash((unsigned char*)(&uiStart), ulAdr, 2);
	ReadDataFromFlash((unsigned char*)(&ulData),  ulAdr+2, 4);
	ReadDataFromFlash((unsigned char*)(&uiEnd),   ulAdr+6, 2);
	//ReadDataFromFlash((unsigned char*)(&sReturnValue), ulAdr, ENERGY_DATA_LONG);
	sReturnValue.uiStartFlag = uiStart;
	sReturnValue.ulEnergy = ulData;
	sReturnValue.uiEndFlag = uiEnd;
	return sReturnValue;
}
/*******************************************************************************
* Function Name : WriteEnergyData
* Description   : Write one enerty data to flash.
* Input         : ulAdr: start adress.
* Output        : None.
* Return        : None.
*******************************************************************************/
static void WriteEnergyData(unsigned long ulAdr, ScmEnergy sEnergyData)
{
	//WriteDataToFlash((unsigned char*)(&sEnergyData), ulAdr, ENERGY_DATA_LONG);
	WriteDataToFlash((unsigned char*)(&(sEnergyData.uiStartFlag)), ulAdr, 2);
	WriteDataToFlash((unsigned char*)(&(sEnergyData.ulEnergy)), ulAdr + 2, 4);
	WriteDataToFlash((unsigned char*)(&(sEnergyData.uiEndFlag)), ulAdr + 6, 2);
}
/*******************************************************************************
* Function Name : CheckEnergyData
* Description   : Check energy data whether it is available.
* Input         : sEnergyData: Data needs to be checked.
* Output        : None.
* Return        : 0x00: The data is available; 
                  0x01: The data is unavailable; 
                  0xff: The all data is 0XFF.   
*******************************************************************************/
static unsigned char CheckEnergyData(ScmEnergy sEnergyData)
{
	unsigned char ucCnt;
	unsigned char ucReturn;
	unsigned char* pucData;
	unsigned char ucSz[ENERGY_DATA_LONG];
	ucSz[0] = (unsigned char)(sEnergyData.uiStartFlag >> 8); 
	ucSz[1] = (unsigned char)(sEnergyData.uiStartFlag);
    ucSz[2] = (unsigned char)(sEnergyData.ulEnergy >> 24); 
	ucSz[3] = (unsigned char)(sEnergyData.ulEnergy >> 16); 
    ucSz[4] = (unsigned char)(sEnergyData.ulEnergy >> 8); 
	ucSz[5] = (unsigned char)(sEnergyData.ulEnergy); 
	ucSz[6] = (unsigned char)(sEnergyData.uiEndFlag >> 8); 
	ucSz[7] = (unsigned char)(sEnergyData.uiEndFlag);
	
	for(ucCnt=0;ucCnt<ENERGY_DATA_LONG;ucCnt++){
		if(ucSz[ucCnt] != (unsigned char)0xff){
			break;
		}
	}
	if (ucCnt == ENERGY_DATA_LONG){ //ȫΪ0xff
		ucReturn = 0xff;
	}else{
		if((sEnergyData.uiStartFlag == ENERGY_START_FLAG) && 
		   (sEnergyData.uiEndFlag == ENERGY_END_FLAG)){
			ucReturn = 0x00;   
		}else{
			ucReturn = 0x01; 
		}
	}
	return ucReturn;
}
/*******************************************************************************
* Function Name : EraseFlashPage
* Description   : ����ulFlashAdr���ڵ�Flashҳ�棬������֤�����Ƿ�ɹ�
* Input         : ulFlashAdr��
* Output        : None.
*******************************************************************************/
static void EraseFlashPage(unsigned long ulFlashAdr)
{
  unsigned char ucCnt;
  unsigned short int uiCnt;
  ScmEnergy  sEnergyData;
  unsigned long ulAdrTemp;
  FLASH_Unlock();
  ulAdrTemp = ulFlashAdr - ulFlashAdr % 1024;  //�����ҳFLASH����ʼ��ַ
  for(ucCnt=0;ucCnt<5;ucCnt++){
	  FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
	  FLASH_ErasePage(ulAdrTemp);
	  for(uiCnt=0; uiCnt<1024; uiCnt += ENERGY_DATA_LONG){
		  sEnergyData = ReadEnergyData(ulAdrTemp + uiCnt);
		  if((unsigned char)(0xff) != CheckEnergyData(sEnergyData)){
			  break;
		  }
	  }
	  if (uiCnt>=1024){
		  break;
	  }
  }
  FLASH_Lock();
}
/*******************************************************************************
* Function Name : LookForNextEnergyAdr
* Description   : Ѱ����һ��ֵ�Ĵ�ŵ�ַ�����Ұѵ�ǰ��ֵ��ȡ����
* Input         : ulNextAdr����һ�����ݴ�ŵ�ַ
* Output        : None.
* Return        : �������ֵ
˵�����ú��������ϵ��ʱ����FLASH�洢��
*******************************************************************************/
static unsigned long LookForNextEnergyAdr(unsigned long *ulNextAdr)
{
	unsigned long ulReturn = 0;
	unsigned char ucTemp, ucTempLast = 0;
	unsigned long ulAdr;
	unsigned long ulDataLast;
	ScmEnergy sEnergy;

	/* */
	for(ulAdr = ENERGY_SAVE_START_ADR; ulAdr < ENERGY_SAVE_END_ADR; ulAdr += ENERGY_DATA_LONG){
		sEnergy = ReadEnergyData(ulAdr);
		ucTemp = CheckEnergyData(sEnergy);
		if ((ulAdr != ENERGY_SAVE_START_ADR)&&(ucTemp == (unsigned char)0xff)){ //��һ�����ݺϷ����������ȫΪFF�����ҵ�����һ���洢��ַ
			if(ucTempLast == 0){
				ulReturn = ulDataLast;
				*ulNextAdr = ulAdr;
				break;
			}
		}else{
			if(ucTemp == 0){//�����������ݺϷ�
			}
		    if(ucTemp == 1){//�����������ݲ��Ϸ�
			}
		}
		ucTempLast = ucTemp;
		ulDataLast = sEnergy.ulEnergy;
    }
	return ulReturn;
}

/*******************************************************************************
* Function Name : CheckAllEnergyPage
* Description   : Check all flash which uses to save energy data. Find fg_ulNextEnergyAdr
* Input         : None.
* Output        : None.
* Return        : ��ǰFLASH�ڵĴ洢����
˵�����ú������Ծ������ܴ洢FLASH����FLASHδ��������ݣ���0������FLASH
*******************************************************************************/
unsigned long CheckAllEnergyPage(void)
{
	typedef struct{
		unsigned long ulAdr;               //��ҳ�׵�ַ
		unsigned long ulMaxData;           //��ҳ�����Ϸ�����
		unsigned char ucAllFfFlag;         //��ҳ�������ݶ���FF��־��0����ȫ��FF��1��ȫ��FF
		unsigned char ucNeedEraseFlag;     //��ҳ��Ҫ������־�� 0������Ҫ������1����Ҫ����
		unsigned char ucAviDataFlag;       //��ҳ�кϷ����ݵı�־��0���ޣ�1����
	}ScmFlashPage;
	#define  FLASH_PAGE_TOTAL           ENERGY_SAVE_PAGE_NUM  
	ScmFlashPage  sFlagPage[FLASH_PAGE_TOTAL];
	unsigned char ucTemp;
	unsigned long ulAdr;
	unsigned char ucPageCurrent = 0;
	unsigned char ucPageCurrentFirstRead = 1; /* ��ҳFLASH��һ�ζ����ݵı�־��0�����ǵ�һ�ζ���1���ǵ�һ�ζ� */
	unsigned long ulAviDataMax;        //���Ϸ�����
	unsigned char ucAviDataMaxIndex;   //���Ϸ��������ڵ������±�
	ScmEnergy sEnergy;
	unsigned char ucCnt;
	unsigned char ucFindAviDataFlag = 0;  //�ҵ��Ϸ����ݵı�־:  0:δ�ҵ���1���Ѿ��ҵ�
#if 1
	/* ��ʼ��flashҳ����� */
	for(ucCnt=0; ucCnt<FLASH_PAGE_TOTAL; ucCnt++){
		sFlagPage[ucCnt].ulAdr = 0;
		sFlagPage[ucCnt].ulMaxData = 0;
		sFlagPage[ucCnt].ucAllFfFlag = 1;
		sFlagPage[ucCnt].ucNeedEraseFlag = 0;
		sFlagPage[ucCnt].ucAviDataFlag = 0;
	}
	/* �ҳ�ÿһҳ����ʼ��ַ�����Ϸ����ݣ��ж��Ƿ�ȫFF���Ƿ���Ҫ���� */
	for(ulAdr = ENERGY_SAVE_START_ADR; ulAdr < ENERGY_SAVE_END_ADR; ulAdr += ENERGY_DATA_LONG){
		ucPageCurrent = (ulAdr - ENERGY_SAVE_START_ADR)/1024; //���㵱ǰ���ڵ�FLASHҳ��
		if (sFlagPage[ucPageCurrent].ulAdr == 0){ //����ҳ�׵�ַ
			ucPageCurrentFirstRead = 1;
			sFlagPage[ucPageCurrent].ulAdr = ENERGY_SAVE_START_ADR + (unsigned long)(ucPageCurrent) * 1024;
		}
		sEnergy = ReadEnergyData(ulAdr);
		ucTemp = CheckEnergyData(sEnergy);
		if (ucTemp == (unsigned char)0xff){ //������������ȫ��0XFF��δд����
		}else{
			if(ucTemp == 0){//�����������ݺϷ�
				if (sFlagPage[ucPageCurrent].ulMaxData < sEnergy.ulEnergy){ //���汾ҳ���Ϸ�����
					sFlagPage[ucPageCurrent].ulMaxData = sEnergy.ulEnergy;
                }					
				/* һ��FLASHҳ��ǰ���ȡ������Ϊ0XFF�������ȡ�����ݺϷ���˵����ҳ�쳣 */
				if (sFlagPage[ucPageCurrent].ucAllFfFlag && (ucPageCurrentFirstRead == 0)){
					sFlagPage[ucPageCurrent].ucNeedEraseFlag = 1; // �������
				}
				ucFindAviDataFlag = 1;
				sFlagPage[ucPageCurrent].ucAviDataFlag = 1;//��ҳFLASH���ںϷ����ݡ�
			}
		    if(ucTemp == 1){//�����������ݲ��Ϸ�
                sFlagPage[ucPageCurrent].ucNeedEraseFlag = 1; //�зǷ����ݣ�����Ҫ������ҳ
			}
			sFlagPage[ucPageCurrent].ucAllFfFlag = 0; //��ҳ���ݲ�ȫ��FF
		}
		ucPageCurrentFirstRead = 0;
    }
#endif
#if 1
	/* �������� */
	/* �ҳ����Ϸ����ݺ����ڵ������±� */
	if (ucFindAviDataFlag){ //�кϷ�����
		ulAviDataMax = 0;
		ucAviDataMaxIndex = 0;
		for(ucCnt=0; ucCnt<FLASH_PAGE_TOTAL; ucCnt++){
			if ((ulAviDataMax <= sFlagPage[ucCnt].ulMaxData) && (sFlagPage[ucCnt].ucAviDataFlag == 1)){
				ulAviDataMax = sFlagPage[ucCnt].ulMaxData;
				ucAviDataMaxIndex = ucCnt;
			}
		}
	    /* ������ȫ��FF��FLASHҳ������������ڵ�FLASHҳ���� */
		for(ucCnt=0; ucCnt<FLASH_PAGE_TOTAL; ucCnt++){
			if (ucCnt == ucAviDataMaxIndex){
				continue;
			}
			if (sFlagPage[ucCnt].ucNeedEraseFlag || (sFlagPage[ucCnt].ucAllFfFlag == 0)){
				if (sFlagPage[ucCnt].ulAdr != 0){
				/* дһ����Ч�����ݣ�ԭ��δ֪��Ȩ��֮�ơ�*/
		            WriteEnergyData(sFlagPage[ucCnt].ulAdr, sEnergy); 
				/* ���� */
					EraseFlashPage(sFlagPage[ucCnt].ulAdr);
				}
			}
		}
		/* ������ݴ��ڵ�ҳ���ǷǷ��� */
		if (sFlagPage[ucAviDataMaxIndex].ucNeedEraseFlag == 1){
			/* �����Ϸ�����д���������ݿ飬���Ҳ������Ϸ��������ڵ�FLASHҳ�� */
			sEnergy.uiStartFlag = ENERGY_START_FLAG;
			sEnergy.uiEndFlag = ENERGY_END_FLAG;
			sEnergy.ulEnergy = sFlagPage[ucAviDataMaxIndex].ulMaxData;
			for(ucCnt=0; ucCnt<FLASH_PAGE_TOTAL; ucCnt++){
				if (ucCnt != ucAviDataMaxIndex){
					WriteEnergyData(sFlagPage[ucCnt].ulAdr, sEnergy);
					/* дһ����Ч�����ݣ�ԭ��δ֪��Ȩ��֮�ơ�*/
		            WriteEnergyData(sFlagPage[ucAviDataMaxIndex].ulAdr, sEnergy); 
					EraseFlashPage(sFlagPage[ucAviDataMaxIndex].ulAdr);
					break;
				}
			}
		}
	}else{ //û�кϷ�����
		/* ������ȫ��FF��FLASHҳ */
		for(ucCnt=0; ucCnt<FLASH_PAGE_TOTAL; ucCnt++){
			if (sFlagPage[ucCnt].ucNeedEraseFlag  || (sFlagPage[ucCnt].ucAllFfFlag == 0)){
				if (sFlagPage[ucCnt].ulAdr != 0){
					/* дһ����Ч�����ݣ�ԭ��δ֪��Ȩ��֮�ơ�*/
		            WriteEnergyData(sFlagPage[ucCnt].ulAdr, sEnergy);
					/* ���� */
					EraseFlashPage(sFlagPage[ucCnt].ulAdr);
				}
			}
		}
		/* ������Ϊ0д��洢 */
		sEnergy.uiStartFlag = ENERGY_START_FLAG;
		sEnergy.ulEnergy = 0;
		sEnergy.uiEndFlag = ENERGY_END_FLAG;
		WriteEnergyData(ENERGY_SAVE_START_ADR, sEnergy);
	}
#endif
	/* �ҳ���һ��Ҫ��ŵĵ�ַ */
#if 1
	return(LookForNextEnergyAdr(&fg_ulNextEnergyAdr));
#else
	return(0);
#endif
}
/*******************************************************************************
* Function Name : SaveEnergyData
* Description   : Save one enerty data to flash.
* Input         : sEnergyData: Energy data needs to be saved.
* Output        : None.
* Return        : None.
*******************************************************************************/
void SaveEnergyData(unsigned long ulEnergyData)
{
	unsigned short int uiTemp;
	unsigned long  ulEraseAdr;
	ScmEnergy sEnergyData;
	sEnergyData.uiStartFlag = ENERGY_START_FLAG;
	sEnergyData.ulEnergy = ulEnergyData;
	sEnergyData.uiEndFlag = ENERGY_END_FLAG;
	FLASH_ClearFlag(FLASH_FLAG_BSY);
	WriteEnergyData(fg_ulNextEnergyAdr, sEnergyData);
	/* �ж��Ƿ�ҳ */
	uiTemp = (fg_ulNextEnergyAdr - ENERGY_SAVE_START_ADR) % 1024;
	/* �Ѿ���ҳ */
	if (uiTemp == 0){
		if (fg_ulNextEnergyAdr == ENERGY_SAVE_START_ADR){/* ��ʼҳ */
			ulEraseAdr = ENERGY_SAVE_START_ADR + 1024 * (ENERGY_SAVE_PAGE_NUM - 1); /* �������һҳ */
		}else{/* ������һҳ */
			ulEraseAdr = fg_ulNextEnergyAdr - 1024;
		}
		/* дһ����Ч�����ݣ���֤���²�������һҳ�����߲������Ǳ�ҳ����ReadEnergyData���ܴﵽ���Ч����
		   ԭ��δ֪��Ȩ��֮�ơ�*/
		WriteEnergyData(ulEraseAdr, sEnergyData); 
		//ReadEnergyData(ulEraseAdr);
		FLASH_Unlock();
		FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
		FLASH_ErasePage(ulEraseAdr);
		while(FLASH->SR & FLASH_FLAG_BSY);
		FLASH_Lock();
	}
	/* ������һ�������ַ */
	if ((fg_ulNextEnergyAdr + ENERGY_DATA_LONG) < ENERGY_SAVE_END_ADR){
		fg_ulNextEnergyAdr += ENERGY_DATA_LONG;
	}else{
		fg_ulNextEnergyAdr = ENERGY_SAVE_START_ADR;
	}
	
}
/*******************************************************************************
* Function Name : ReadEnergyDataFromFlash
* Description   : Read one enerty data from flash.ʹ�õ���ȫ�ֱ���
* Input         : ulAdr: start adress.
* Output        : None.
* Return        : Read one enerty data.
*******************************************************************************/
unsigned long ReadEnergyDataFromFlash(void)
{
	ScmEnergy sTemp;
	unsigned long ulAdr;
	/* �������ݴ洢�ĵ�ַ */
	if (fg_ulNextEnergyAdr == ENERGY_SAVE_START_ADR){
		/* ���һ����ַ */
		ulAdr = ENERGY_SAVE_START_ADR  + ENERGY_SAVE_PAGE_NUM * 1024 - ENERGY_DATA_LONG;
	}else{
		ulAdr = fg_ulNextEnergyAdr - ENERGY_DATA_LONG;
	}
	sTemp = ReadEnergyData(ulAdr);
	return(sTemp.ulEnergy);
}
#endif

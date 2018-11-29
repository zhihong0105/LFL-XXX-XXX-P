#include "stm32f0xx.h"
#include "Repair.h"
#include "FlashOperate.h"

/**************************** 测试制具校验参数*****************************/
#if  PARM_REPAIR_EN == 1 
extern volatile unsigned char      g_ucReciveCnt;
/***********************************************************/
/* ????: ?????????                           */
/* ????: sRepairParam:????;                     */
/*            uiAdr:???????                        */
/* ????: ?                                           */
/* ???:   ?                                           */
/* ??:     ?                                           */
/***********************************************************/
void SetParamAtStart(void)
{
    ScmRepairParamAll  sRepairParam;
    unsigned short int uiCnt;
    if (2 == ReadRepairParmAll(&sRepairParam, PARAM_REPAIR_ADR1)){
        FLASH_Unlock();
	    FLASH_ClearFlag(FLASH_FLAG_BSY |FLASH_FLAG_EOP |FLASH_FLAG_PGERR);
	    FLASH_ErasePage(PARAM_REPAIR_ADR1);
        WriteRepairParmAll(sRepairParam, PARAM_REPAIR_ADR1);
        FLASH_Lock();
    }       
}
/***********************************************************/
/* function name: ReadRepairParmAll                        */
/* input: sRepairParam: save read parm;                    */
/*        uiAdr: the start adress of reading               */
/* return: 0: have no save param
           1: only save one group param;              
           2: save many groups   param                     */
/***********************************************************/
unsigned char ReadRepairParmAll(ScmRepairParamAll *sRepairParam, unsigned long ulAdr)
{
	unsigned int uiCnt;
	unsigned char ucReturnValue;
	ScmRepairParamAll sRepairParamTemp, sRepairParamTempLast;
	for(uiCnt = 0; uiCnt < 1024 - PARM_REPAIR_LONG; uiCnt += PARM_REPAIR_LONG){
		ReadDataFromFlash((unsigned char *)(&sRepairParamTemp), ulAdr + uiCnt, PARM_REPAIR_LONG);
		if(sRepairParamTemp.uiStartFlag != WRITE_SUCCESS_FLAG && sRepairParamTemp.uiEndFlag != WRITE_SUCCESS_FLAG){
			break;
		}
		sRepairParamTempLast = sRepairParamTemp;
	}
	if(uiCnt == 0){                       // have no param;
		ucReturnValue = 0;
	}else if(uiCnt == PARM_REPAIR_LONG){  // save only one group param
		*sRepairParam = sRepairParamTempLast;
		ucReturnValue = 1;
	}else{                                // have many groups param 
		*sRepairParam = sRepairParamTempLast;
		ucReturnValue = 2;
	}
	/* 返回值 */
	return(ucReturnValue);
}

/*******************************************************************************
* Function Name : WriteRepairParmAll
* Description   : ?????????EEPROM
* Input         : sRepairParam:??????;uiAdr:????????
* Output        : None.
* Return        : Brightness
*******************************************************************************/
void WriteRepairParmAll(ScmRepairParamAll sRepairParam, unsigned long ulAdr)
{
  unsigned short int uiCnt;
	unsigned short int uiIndex;
  unsigned char ucWriteEn = 0;  /* We can write the FLASH */
  ScmRepairParamAll  sRepairParamTemp;
	for(uiCnt = 0; uiCnt < 1024 - PARM_REPAIR_LONG; uiCnt += PARM_REPAIR_LONG){
	  ReadDataFromFlash((unsigned char *)(&sRepairParamTemp), ulAdr + uiCnt, PARM_REPAIR_LONG); 
//		for(uiIndex = 0; uiIndex < PARM_REPAIR_LONG; uiIndex++){   //check if all the data is 0xff;
//			if(*((unsigned char *)((&sRepairParamTemp) + uiIndex)) != (unsigned char)0xff){
//				break;
//		    }
//	    }
//        if (uiIndex == PARM_REPAIR_LONG){  //yes
//            ucWriteEn = 1;
//            break;
//        }
		if(sRepairParamTemp.uiStartFlag == 0xffff && sRepairParamTemp.uiEndFlag == 0xffff){
			ucWriteEn = 1;
      break;
	  }
	}
	/* write data to flash */
	if(ucWriteEn == 1){
			WriteDataToFlash((unsigned char *)(&sRepairParam), ulAdr + uiCnt, PARM_REPAIR_LONG);
	}
}

/*******************************************************************************
* Function Name : SaveRepairParmToBothArea
* Description   : ?sRepairParam??????????
* Input         : ?????????
* Output        : None.
* Return        : None
*******************************************************************************/
void SaveRepairParmToBothArea(ScmRepairParamAll sRepairParam)
{
   WriteRepairParmAll(sRepairParam, PARAM_REPAIR_ADR1);
   //WriteRepairParmAll(sRepairParam, PARAM_REPAIR_COPY_ADR1);
}
/*******************************************************************************
* Function Name : SetRepairParmAllToDefault
* Description   : ?sRepairParam??????
* Input         : ???????????
* Output        : None.
* Return        : None
*******************************************************************************/
void SetRepairParmAllToDefault(ScmRepairParamAll *sRepairParam)
{ 
	/* ??? */
	sRepairParam->uiStartFlag = WRITE_SUCCESS_FLAG;
	/* Input param default */
	sRepairParam->uPowerCheck.f = PT;                              /* ?? */
	sRepairParam->uiPowerCntCheck = PLUSE_CNT_DEFAULT;            /* ?? */
	sRepairParam->uiPowerTimeCheck = TIME_CNT_DEFAULT;            /* ?? */
	sRepairParam->uInVoltageCheck.f = VT;                          /* ?? */
	sRepairParam->uiInVoltageCntCheck = PLUSE_CNT_DEFAULT;        /* ?? */
	sRepairParam->uiInVoltageTimeCheck = TIME_CNT_DEFAULT;        /* ?? */
	sRepairParam->uInCurrentCheck.f = AT;                          /* ?? */
	sRepairParam->uiInCurrentCntCheck = PLUSE_CNT_DEFAULT;        /* ?? */
	sRepairParam->uiInCurrentTimeCheck = TIME_CNT_DEFAULT;        /* ?? */

	/* Output param default */
	sRepairParam->fOutVoltageRatio = AD_TO_VOLTAGE_OUT(1);       /* ?? */
	sRepairParam->fOutCurrentRatio = AD_TO_CURRENT_OUT(1) * 1000;       /* ?? */
	sRepairParam->fOutCurrentIntercept = 0;
	/* ?? */
	sRepairParam->ucParmState = 0;
	/* ??? */
	sRepairParam->uiEndFlag = WRITE_SUCCESS_FLAG;
}
/*******************************************************************************
* Function Name : CheckUartRecive
* Description   : ??PLC????????????
* Input         : ucTotal:???????
* Output        : None.
* Return        : 0:?????;1:????
*******************************************************************************/
unsigned char CheckUartRecive(unsigned char ucTotal)
{
    unsigned char ucCheckXor, ucCheckSum;
    unsigned char ucCnt;
    unsigned char ucReturn = 0;
    if (ucTotal < 2 || ucTotal != (g_ucReciveCnt+1)){
       return(ucReturn);
    }
    ucCheckXor = 0;
    ucCheckSum = 0;
    for(ucCnt = 0; ucCnt < ucTotal - 2; ucCnt++){
        ucCheckXor ^= g_ucReciveData[ucCnt];
        ucCheckSum += g_ucReciveData[ucCnt];
    }
    if (ucCheckXor == g_ucReciveData[ucTotal - 2] && ucCheckSum == g_ucReciveData[ucTotal - 1]){
        //????
        ucReturn = 1;
    }
    return(ucReturn);
}
/*******************************************************************************
* Function Name : ParmRepairAck4
* Description   : ?????4??????
* Input         : ucOrder:????;ucAck:????
* Output        : None.
* Return        : 0:?????;1:????
*******************************************************************************/
void ParmRepairAck4(unsigned char ucOrder, unsigned char ucAck)
{
    unsigned char ucCnt;
    g_ucSendData[0] = 0x8A;
    g_ucSendData[1] = ucOrder;
    g_ucSendData[2] = ucAck;
    g_ucSendData[3] = 0x00;
    for(ucCnt = 0; ucCnt < 3;ucCnt++){
        g_ucSendData[3] += g_ucSendData[ucCnt];
    }
    g_ucSendSum = 4;
    g_sSysParam.ucUART_SendEn = 1;
}
#endif

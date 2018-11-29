#include "PidControl.h"

/*******************************************************************************
* Function Name : PiCalc
* Description   : PiCalc.
* Input         : ucMode: 0:正常模式；1：校正期间使用的模式
* Output        : None.
* Return        : None.
*******************************************************************************/
void PiCalc(ScmPiReg *u, unsigned char ucMode)
{	
    float fKpTemp,fKiTemp;
	/* 计算误差 */
	 u->Err = u->Ref - u->Fdb;
	if (ucMode == 0){//正常模式，带死区功能
		if (-100 < u->Err && u->Err < 100){
			fKpTemp = 0;
			fKiTemp = u->Ki * 0.05;
			//fKpTemp = u->Kp;
			//fKiTemp = u->Ki;
		}else{
			fKpTemp = u->Kp;
			fKiTemp = u->Ki;
		}
    }else{//校正模式，取消死区功能
		fKpTemp = u->Kp;
	    fKiTemp = u->Ki;
    }		
	/* 计算P参数 */
   u->Up = fKpTemp * u->Err;
  
  /* 计算积分值 */
   u->Ui += u->Err * fKiTemp;
   if (u->Ui < u->UiMin){
       u->Ui = u->UiMin;
   }
   if (u->Ui > u->UiMax){
       u->Ui = u->UiMax;
   }
  /* 计算输出增量 */
   u->OutTemp = u->Up + u->Ui;
   u->Out = u->OutTemp;
  /* 边界值限制 */
   if (u->Out > u->OutMax){
       u->Out = u->OutMax;
   }
   if (u->Out < u->OutMin){
       u->Out = u->OutMin;
   }

}

/*******************************************************************************
* Function Name : PiCalcDelta
* Description   : PiCalcDelta.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void PiCalcDelta(ScmPiReg *u)
{	
	/* 计算误差 */
	 u->Err = u->Ref - u->Fdb;
  
	/* 计算P参数 */
   u->Up = u->Kp * u->Err;
  
  /* 计算积分值 */
   u->Ui += u->Err * u->Ki;
   if (u->Ui < u->UiMin){
       u->Ui = u->UiMin;
   }
   if (u->Ui > u->UiMax){
       u->Ui = u->UiMax;
   }
  /* 计算微分值 */
   u->DeltaErr = u->Err - u->ErrPrevious;
   u->Ud = u->DeltaErr * u->Kd;
   u->ErrPrevious = u->Err;
  /* 计算输出 */
   u->OutTemp = u->Up + u->Ui + u->Ud;
  
  /* 增量最大值限制 */
   if (u->OutTemp > u->DeltaMax){
       u->OutTemp = u->DeltaMax;
   }
   if (u->OutTemp < (-u->DeltaMax)){
       u->OutTemp = -u->DeltaMax;
   }
   u->Out = u->OutTemp;
}


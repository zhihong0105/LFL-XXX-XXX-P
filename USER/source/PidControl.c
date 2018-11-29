#include "PidControl.h"

/*******************************************************************************
* Function Name : PiCalc
* Description   : PiCalc.
* Input         : ucMode: 0:����ģʽ��1��У���ڼ�ʹ�õ�ģʽ
* Output        : None.
* Return        : None.
*******************************************************************************/
void PiCalc(ScmPiReg *u, unsigned char ucMode)
{	
    float fKpTemp,fKiTemp;
	/* ������� */
	 u->Err = u->Ref - u->Fdb;
	if (ucMode == 0){//����ģʽ������������
		if (-100 < u->Err && u->Err < 100){
			fKpTemp = 0;
			fKiTemp = u->Ki * 0.05;
			//fKpTemp = u->Kp;
			//fKiTemp = u->Ki;
		}else{
			fKpTemp = u->Kp;
			fKiTemp = u->Ki;
		}
    }else{//У��ģʽ��ȡ����������
		fKpTemp = u->Kp;
	    fKiTemp = u->Ki;
    }		
	/* ����P���� */
   u->Up = fKpTemp * u->Err;
  
  /* �������ֵ */
   u->Ui += u->Err * fKiTemp;
   if (u->Ui < u->UiMin){
       u->Ui = u->UiMin;
   }
   if (u->Ui > u->UiMax){
       u->Ui = u->UiMax;
   }
  /* ����������� */
   u->OutTemp = u->Up + u->Ui;
   u->Out = u->OutTemp;
  /* �߽�ֵ���� */
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
	/* ������� */
	 u->Err = u->Ref - u->Fdb;
  
	/* ����P���� */
   u->Up = u->Kp * u->Err;
  
  /* �������ֵ */
   u->Ui += u->Err * u->Ki;
   if (u->Ui < u->UiMin){
       u->Ui = u->UiMin;
   }
   if (u->Ui > u->UiMax){
       u->Ui = u->UiMax;
   }
  /* ����΢��ֵ */
   u->DeltaErr = u->Err - u->ErrPrevious;
   u->Ud = u->DeltaErr * u->Kd;
   u->ErrPrevious = u->Err;
  /* ������� */
   u->OutTemp = u->Up + u->Ui + u->Ud;
  
  /* �������ֵ���� */
   if (u->OutTemp > u->DeltaMax){
       u->OutTemp = u->DeltaMax;
   }
   if (u->OutTemp < (-u->DeltaMax)){
       u->OutTemp = -u->DeltaMax;
   }
   u->Out = u->OutTemp;
}


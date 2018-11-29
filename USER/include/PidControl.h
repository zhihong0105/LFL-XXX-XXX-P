
#ifndef __PID_CONTROL_H__
#define __PID_CONTROL_H__
	
#define  Q16(x)	   ((x)*65535)
//#define  Q15(x)	   ((x)*32768)
typedef struct
{
  signed long Ref;	  // refrence;
  signed long Fdb;	  // feed back;
  signed long Err;	  //  error;
  /* �������� */
  double Kp;		// ��������
  double Up;		// ����ֵ
  /* ���ֲ��� */
  double Ki;		// ���ֲ���
  double Ui;		// ����ֵ
  double UiMax;
  double UiMin;
  /* ΢�ֲ��� */
  float Kd;         // ΢�ֲ���
  float DeltaErr;   /* ���仯�� */
  float ErrPrevious;     /* �ϴε����ֵ */
  float Ud;

  signed long OutMax;		  // ������ֵ
  signed long OutMin;		  // �����Сֵ
  signed long Out;		    // ���;
  signed long OutTemp;		// ���;
  signed long DeltaMax;  // �仯���ֵ

}ScmPiReg; 


void PiCalc(ScmPiReg *u, unsigned char ucMode);
void PiCalcDelta(ScmPiReg *u);
#endif

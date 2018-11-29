
#ifndef __PID_CONTROL_H__
#define __PID_CONTROL_H__
	
#define  Q16(x)	   ((x)*65535)
//#define  Q15(x)	   ((x)*32768)
typedef struct
{
  signed long Ref;	  // refrence;
  signed long Fdb;	  // feed back;
  signed long Err;	  //  error;
  /* 比例参数 */
  double Kp;		// 比例参数
  double Up;		// 比例值
  /* 积分参数 */
  double Ki;		// 积分参数
  double Ui;		// 积分值
  double UiMax;
  double UiMin;
  /* 微分参数 */
  float Kd;         // 微分参数
  float DeltaErr;   /* 误差变化量 */
  float ErrPrevious;     /* 上次的误差值 */
  float Ud;

  signed long OutMax;		  // 输出最大值
  signed long OutMin;		  // 输出最小值
  signed long Out;		    // 输出;
  signed long OutTemp;		// 输出;
  signed long DeltaMax;  // 变化最大值

}ScmPiReg; 


void PiCalc(ScmPiReg *u, unsigned char ucMode);
void PiCalcDelta(ScmPiReg *u);
#endif

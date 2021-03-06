
#include "Temperature_Test.h"
 /*************************************************
  Temperature resistance table
	Vcc = 3.3V
	Rup = 103F3450
	Rdown = 10k
	ADC = 12bit
 **************************************************/
 const short int TempTable[151] = {
279 ,294 ,310 ,327 ,345 ,363 ,382 ,401 ,422 ,443 ,464 ,487 ,510 ,534 ,559 ,585 ,611 ,638 ,666 ,
694 ,724 ,754 ,784 ,816 ,848 ,881 ,914 ,949 ,983 ,1019 ,1055 ,1091 ,1128 ,1166 ,1204 ,1242 ,
1281 ,1320 ,1359 ,1399 ,1439 ,1480 ,1520 ,1561 ,1601 ,1642 ,1683 ,1724 ,1765 ,1806 ,1847 ,
1887 ,1928 ,1968 ,2008 ,2048 ,2088 ,2127 ,2166 ,2205 ,2243 ,2281 ,2318 ,2355 ,2392 ,2428 ,
2464 ,2499 ,2534 ,2568 ,2602 ,2635 ,2667 ,2699 ,2731 ,2762 ,2792 ,2822 ,2851 ,2880 ,2908 ,
2936 ,2963 ,2989 ,3015 ,3041 ,3065 ,3090 ,3113 ,3137 ,3159 ,3182 ,3203 ,3224 ,3245 ,3265 ,
3285 ,3304 ,3323 ,3341 ,3360 ,3377 ,3394 ,3411 ,3427 ,3443 ,3458 ,3473 ,3488 ,3502 ,3516 ,
3530 ,3543 ,3556 ,3569 ,3581 ,3593 ,3604 ,3616 ,3627 ,3638 ,3648 ,3658 ,3668 ,3678 ,3688 ,
3697 ,3706 ,3715 ,3723 ,3731 ,3740 ,3747 ,3755 ,3763 ,3770 ,3777 ,3784 ,3791 ,3798 ,3804 ,
3811 ,3817 ,3823 ,3828 ,3834 ,3840 ,3845 ,3850 ,3855 ,3861
 };
 
short int TemperatureCalculate(short int iAD_Data)
{
	  short int iTempIndex;
    /* Calculate temperature from the table */
    for(iTempIndex = 0;iTempIndex<151;iTempIndex++)
		{
       if(iAD_Data < TempTable[iTempIndex])break;
    }
		return(iTempIndex - 30);
}

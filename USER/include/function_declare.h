
//=======================================
//GPIO初始化函数
void IO_Init(void);

//=====================================
//延时函数
void Delay(unsigned int t);
//======================================
//系统时钟配置
void RCC_Configuration(void);
//======================================
//
void NVIC_Configuration(void);
//======================================
//定时器初始化函数
void TIM_Init(void);

//=======================================
//ADC初始化
void FunADC_Init(void);
//======================================
void FunDAC_Init(void);
//======================================
//UART初始化函数
void UART_Init(void);
//=======================================
//
void GPIO_Interrupt(void);
//=======================================
//
void FunComp_Init(void);

void WatvhDog_inti(void);

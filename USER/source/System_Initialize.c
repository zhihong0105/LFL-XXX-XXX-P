/******************** (C) COPYRIGHT 2013 HID ******************************
* File Name 	: System_Initialize.c
* Author 		  : ZhihongWang
* Version 		: V0.0.1
* Date 			  : 2013-12-09
* Description : 
*******************************************************************************/
#include "stm32f0xx.h"
#include "System_Initialize.h"
#include "main.h"
#include "Repair.h"
/*******************************************************************************
* Function Name : IO_Init
* Description   : GPIO initialize
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void IO_Init(void)
{
	// Define the GPIO structure
	GPIO_InitTypeDef GPIO_InitStructure;
	/* GPIO Periph clock enable */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOF, ENABLE); 

	/* Set GPIOA for ADC*/
	/* Set the gpio for ADC----AN0(FB), AN3-Temperature, AN7-Vbus */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_3  | GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AN;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	/* COMP1 INPUT */ 
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;  
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	/* IO口设置 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; /* CF1*/
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; /* CF*/
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_0);
	/* OUTPUT */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_6; /* PA12(SI)  PA6(SEL) */
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
/* test */
//  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14; 
//  GPIO_Init(GPIOA,&GPIO_InitStructure);
#ifdef CF_CF1_OUTPUT
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_5; /* CF CF1*/
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
#endif
#ifdef PA6_COMP1_OUTPUT
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; /* PA6(OUTPUT) */
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_7);
#endif
	/* Alternate functions selecte */
	/* set TIM1 for LLC PWM, Set TIM3 for Dimming PWM */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;   						/* TIM1_CH1 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP ;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF_2);		/* important */   
	/* Set GPIO for UART */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_15;   /* USART1_TX, USART1_RX, USART2_RX */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);	
	/* Alternate functions selecte */
#ifdef  PA9_IO
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;  
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
#else
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9,GPIO_AF_1); 		/* important */
#endif
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10,GPIO_AF_1); 
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource15,GPIO_AF_1);       	
	
  /* Set PortB */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_4;	/* TIM1_CH1N , TIM3_CH1 */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd =  GPIO_PuPd_UP;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	/* Alternate functions selecte */
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_2);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_1); 
#if DIM_MODE == 2
  /* OUTPUT */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; /* PB4(SCK) */
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
#endif
  /* Set PortF */
	/* Set GPIOF for LED an Control AFPC IC */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;	/* PF6--Control APFC, PF7--LED */
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_Init(GPIOF,&GPIO_InitStructure);
} 

/*******************************************************************************
* Function Name : RCC_Configuration
* Description   : Set the system clock as 48MHz,when the HSE is 12MHz.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void RCC_Configuration(void)
{  
	RCC_DeInit(); 							// Set the RCC Registers
#ifdef   USE_HSE
	RCC_HSEConfig(RCC_HSE_ON); 	// Waiting.......
	while(RCC_WaitForHSEStartUp() != SUCCESS)
	{
	}
#endif
	/* Enable Prefetch Buffer */
	FLASH_PrefetchBufferCmd(ENABLE);
	/* Flash 1 wait state */
	FLASH_SetLatency(FLASH_Latency_1);  //Set flash wait state.
	/* AHB clock = SYSCLK */
	RCC_HCLKConfig(RCC_SYSCLK_Div1);  
	/* APB clock = HCLK */
	RCC_PCLKConfig(RCC_HCLK_Div1);  
#ifdef  USE_HSE
	RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);  	/* PLLCLK = 8MHz * 6 = 48 MHz */
#else           
	RCC_PLLConfig(RCC_PLLSource_HSI_Div2, RCC_PLLMul_12); /* PLLCLK = 8/2MHz * 12 = 48 MHz */
#endif
	/* Enable PLL */ 
	RCC_PLLCmd(ENABLE);

	/* Wait till PLL is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
	{
	}
	/* Select PLL as system clock source */
	RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
	/* Wait till PLL is used as system clock source */
	while(RCC_GetSYSCLKSource() != 0x08)
	{
	}
}
/*******************************************************************************
* Function Name : NVIC_Configuration
* Description   : interrupt initialize.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
  /* Enable the ADC global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
#ifndef REGULATE_FREQUENCY_MODER
	#error "Please define REGULATE_FREQUENCY_MODER!"
#elif REGULATE_FREQUENCY_MODER == 1
	/* Enable the TIM1 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#elif REGULATE_FREQUENCY_MODER == 2
	/* Enable the TIM1 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM15_IRQn;;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#else
	#error "Please set REGULATE_FREQUENCY_MODER in correct value!"
#endif
	/* Enable the TIM2 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* Enable the TIM6 global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_DAC_IRQn;;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	
	/* Enable the UART global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	/* Enable EXIT Interrupt */
	//    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_3_IRQn;
	//    NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	//    NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
/*******************************************************************************
* Function Name : TIM_Init
* Description   : TIM initialize.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void TIM_Init(void)
{	  
	TIM_TimeBaseInitTypeDef  TIM_Struct;
	TIM_OCInitTypeDef        TIM_OCStruct;
	TIM_BDTRInitTypeDef      TIM_BDTRInitStruct;
	TIM_ICInitTypeDef        TIM_ICStruct;
	/*********************** TIMER1 for LLC PWM ***********************/
	/* set clock of TIM1 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
  /* set TIM1 as default */
	TIM_DeInit(TIM1);
  /* set TIM1 */
	TIM_Struct.TIM_Period = TIM1_PERIOD_SHORT;	// set the Frequcy of LLC pwm
	TIM_Struct.TIM_Prescaler = 0;        	// 1
	TIM_Struct.TIM_ClockDivision = 0;			//
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;//TIM_CounterMode_CenterAligned1; // mode
	TIM_TimeBaseInit(TIM1,&TIM_Struct);
	TIM_Cmd(TIM1, DISABLE);								// Disable TIM1 
  /* set PWM */
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;                	// set PWM module
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;    	// Enable P out
	TIM_OCStruct.TIM_OutputNState = TIM_OutputNState_Enable;    // Enable N out		
	TIM_OCStruct.TIM_Pulse = TIM1_PERIOD_SHORT / 2;             // Set the PWM duty
	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;         	// set the polartity of P out	
	TIM_OCStruct.TIM_OCNPolarity = TIM_OCPolarity_High;         // set the polartity of N out
	TIM_OCStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;     	// set the idle state.
	TIM_OCStruct.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    TIM_OC1Init(TIM1,&TIM_OCStruct);
	// ARR PreLoad
	TIM_ARRPreloadConfig(TIM1, ENABLE);
    // OC1 PreLoad
    TIM_OC1PreloadConfig(TIM1, ENABLE);    
  /* interupt setting */
    TIM_ClearFlag(TIM1, TIM_FLAG_CC1);	 //???????
#if REGULATE_FREQUENCY_MODER == 1
    TIM_ITConfig(TIM1,TIM_IT_CC1,ENABLE); //????
#elif REGULATE_FREQUENCY_MODER == 2
    TIM_ITConfig(TIM1,TIM_IT_CC1,DISABLE); //????
#endif
  /* disable TIM1 */
    TIM_Cmd(TIM1, DISABLE);
	
	/******* Advanced-control timers (TIM1) specific features ********/
	TIM_BDTRInitStruct.TIM_AutomaticOutput = TIM_AutomaticOutput_Enable;	// AOE Enable
	TIM_BDTRInitStruct.TIM_Break = TIM_Break_Enable;											// break in enable
	TIM_BDTRInitStruct.TIM_BreakPolarity = TIM_BreakPolarity_High;				// set the polarity of Break IN
	TIM_BDTRInitStruct.TIM_DeadTime = DEAD_TIME;		  // set the deadtime
	TIM_BDTRInitStruct.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
	TIM_BDTRInitStruct.TIM_OSSIState = TIM_OSSIState_Enable;
	TIM_BDTRInitStruct.TIM_OSSRState = TIM_OSSRState_Enable;
	TIM_BDTRConfig(TIM1,&TIM_BDTRInitStruct);	
	/* TIM1 Main Output Enable */
	TIM_CtrlPWMOutputs(TIM1, ENABLE);
  
  /*********************** Use TIMER15 to TEST ENERGY ***********************/
   /* set clock of TIM15 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM15, ENABLE);
  /* set TIM15 as default */
	TIM_DeInit(TIM15);
  /* set TIM15 */
    TIM_Struct.TIM_Period = TIM15_PERIOD;
    TIM_Struct.TIM_Prescaler = 0;
    TIM_Struct.TIM_ClockDivision = 0;//
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM15,&TIM_Struct);  
  /* TIM2_CH1 */
//    TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;                       //set PWM module
//	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;           //Enable P out	   
//	TIM_OCStruct.TIM_Pulse = 0;                                      //Set the  comparer count
//	TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_High;               //set the polartity of P out				
//	TIM_OCStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;            //set the idle state.
//    TIM_OC1Init(TIM2,&TIM_OCStruct);	    
  /* ????? */
	TIM_ARRPreloadConfig(TIM15, ENABLE);      //使能预装载                          
    TIM_UpdateDisableConfig(TIM15, DISABLE);	 //使能更新事件 
  /* Repetitive Mode selection */ 
	TIM_SelectOnePulseMode(TIM15, TIM_OPMode_Repetitive); //循环模式
	/* 配置CH1下降沿有效，并且CH1接到T1 */
	TIM_ICStruct.TIM_Channel = TIM_Channel_1;
	TIM_ICStruct.TIM_ICFilter = 0X10;
	TIM_ICStruct.TIM_ICPolarity = TIM_ICPolarity_Falling;
	TIM_ICStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;
	TIM_ICInit(TIM15, &TIM_ICStruct);
	/* 选择CH1作为TRGI */
	TIM_SelectInputTrigger(TIM15, TIM_TS_TI1FP1);
	/* 选择外部1时钟模式 */
	TIM_SelectSlaveMode(TIM15, TIM_SlaveMode_External1);
	/* 使能从模式 */
	TIM_SelectMasterSlaveMode(TIM15, TIM_MasterSlaveMode_Enable);
	/* interupt setting */
    TIM_ClearFlag(TIM15, TIM_FLAG_Update);	 //清除更新中断标志位
    TIM_ClearFlag(TIM15, TIM_FLAG_CC1);
	TIM_ITConfig(TIM15,TIM_IT_Update,ENABLE); //使能更新中断
	TIM_ITConfig(TIM15,TIM_IT_CC1,ENABLE); 
  /* Enable TIM2 */
    TIM_Cmd(TIM15, ENABLE);
  /*********************** TIMER3 Dimming PWM ***********************/
	/* set clock of TIM3 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  /* set TIM3 as default */
	TIM_DeInit(TIM3);
  /* set TIM3 */
	TIM_Struct.TIM_Period = TIM3_PERIOD;
	TIM_Struct.TIM_Prescaler = TIM3_DIV - 1;        		// prescaler 16
	TIM_Struct.TIM_ClockDivision = 0;										//
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up; 		// Counte up mode
	TIM_TimeBaseInit(TIM3,&TIM_Struct);
  /* set PWM */
	TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM1;                      // set PWM module
	TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;          // Enable P out	   
	TIM_OCStruct.TIM_Pulse = 1;                      								// Set the  comparer count
	TIM_OCStruct.TIM_OCPolarity =  TIM_OCPolarity_High;           	// set the polartity of P out			
	TIM_OCStruct.TIM_OCIdleState = TIM_OCIdleState_Reset;           // set the idle state. 
	TIM_OC1Init(TIM3,&TIM_OCStruct);  			          
  // Auto Pre Load
	TIM_ARRPreloadConfig(TIM3, ENABLE);  
#if  DIM_MODE == 1
	/* Enables the TIM3 Capture Compare Channel 1 */
	// TIM_CCxCmd(TIM3, TIM_Channel_1, ENABLE);
	/* TIM3 Main Output Enable */
    TIM_CtrlPWMOutputs(TIM3, ENABLE);
//	/* enable TIM3 */
    TIM_Cmd(TIM3, ENABLE);
#endif
	/*********************** Use TIMER6 to delay ***********************/
	/* set clock of TIM6 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
  /* set TIM6 as default */
	TIM_DeInit(TIM6);
  /* set TIM6 */
    TIM_Struct.TIM_Period = TIM6_PERIOD;
	TIM_Struct.TIM_Prescaler = 0;        //1??
	TIM_Struct.TIM_ClockDivision = 0;//
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up; //????
	TIM_TimeBaseInit(TIM6,&TIM_Struct);          
  /* ????? */
	TIM_ARRPreloadConfig(TIM6, ENABLE);                             
    TIM_UpdateDisableConfig(TIM6, DISABLE);	 // 
	/* interupt setting */
    TIM_ClearFlag(TIM6, TIM_FLAG_Update);	 //???????
    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); //????
  /* disable TIM6 */
    TIM_Cmd(TIM6, ENABLE);
/*********************** Use TIMER2 to start ADC ***********************/
	/* set clock of TIM2 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  /* set TIM2 as default */
    TIM_DeInit(TIM2);
  /* set TIM2 */
#if REGULATE_FREQUENCY_MODER == 1
	TIM_Struct.TIM_Period = TIM2_PERIOD;
#elif REGULATE_FREQUENCY_MODER == 2
    TIM_Struct.TIM_Period = TIM2_PERIOD;
#endif
	TIM_Struct.TIM_Prescaler = 0;        	// 1
	TIM_Struct.TIM_ClockDivision = 0;			//
	TIM_Struct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2,&TIM_Struct); 
  /* Select internal clock as TIM2 clock */
    TIM_InternalClockConfig(TIM2);	
  /* Enables TIM15 peripheral Preload register on ARR */
	//TIM_ARRPreloadConfig(TIM15, DISABLE); 
	// Enables the TIMx Update event
	// 清除溢出中断标志
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
#if REGULATE_FREQUENCY_MODER == 1
	TIM_UpdateDisableConfig(TIM2,DISABLE);   //关闭TIM2中断
#elif REGULATE_FREQUENCY_MODER == 2
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //开启TIM2中断
#endif
	/* setting TRGO */
    TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);
	TIM_SelectMasterSlaveMode(TIM2, TIM_MasterSlaveMode_Enable);
  /* Disable TIM2 */
	TIM_Cmd(TIM2, DISABLE);
}
/*******************************************************************************
* Function Name : FunADC_Init
* Description   : ADC initialize.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void FunADC_Init(void)
{
    ADC_InitTypeDef     ADC_InitStructure;
    DMA_InitTypeDef     sDMA_InitStructure;
	/* ADC1 Periph clock enable */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
  /* DMA1 clock enable */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1 , ENABLE);
	
  /* DMA1 Channel1 Config */
    DMA_DeInit(DMA1_Channel1);
    sDMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)ADC1_DR_ADR ;//(uint32_t)(ADC1_BASE + 0x40);
    sDMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)g_uiADC_ConvertedValue;
    sDMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    sDMA_InitStructure.DMA_BufferSize = SUM_OF_CHANNEL;
    sDMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    sDMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    sDMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    sDMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    sDMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//DMA_Mode_Normal;
    sDMA_InitStructure.DMA_Priority = DMA_Priority_High;
    sDMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel1, &sDMA_InitStructure);
  /* DMA1 Channel1 enable */
    DMA_Cmd(DMA1_Channel1, ENABLE);
  
  /* ADC1 DeInit */  
    ADC_DeInit(ADC1);
  /* ADC DMA request in circular mode */
    ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
  /* Initialize ADC structure */
    ADC_StructInit(&ADC_InitStructure);
  /* Configure the ADC1 in continous mode with a resolutuion equal to 12 bits  */
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; 
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    ADC_InitStructure.ADC_ExternalTrigConv =  ADC_ExternalTrigConv_T2_TRGO;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
    ADC_Init(ADC1, &ADC_InitStructure); 
  /* Configures for the selected ADC and sampling time */	
	ADC_ChannelConfig(ADC1,ADC_Channel_0 | ADC_Channel_3  | ADC_Channel_7, ADC_SampleTime_7_5Cycles);  
	ADC_DiscModeCmd(ADC1,DISABLE); 
	ADC_ContinuousModeCmd(ADC1,DISABLE);
	/* Enable ADC1 external trigger conversion */ 
    ADC_ClearITPendingBit(ADC1, ADC_IT_EOSEQ);   // Clear Pending Bit
    ADC_ITConfig(ADC1, ADC_IT_EOSEQ, DISABLE);
  /* ADC Calibration */
    ADC_GetCalibrationFactor(ADC1);
  /* Enable ADC_DMA */
    ADC_DMACmd(ADC1, ENABLE);        //
  /* Enable ADC1 */
    ADC_Cmd(ADC1, ENABLE);     
  
  /* Wait the ADCEN falg */
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN)); 
  
  /* ADC1 regular Software Start Conv */ 
    ADC_StartOfConversion(ADC1);
}

/*******************************************************************************
* Function Name : UART_Init
* Description   : UART initialize.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void UART_Init(void)
{
    USART_InitTypeDef USART_InitStruct;
	/* UART1 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);	   //??UART2??
    USART_DeInit(USART1);		
    USART_InitStruct.USART_BaudRate = BAUD_RATE;	
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;	
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1,&USART_InitStruct);
  /* interrupt */ 
    USART_ClearFlag(USART1,USART_FLAG_RXNE); 
	USART_ClearFlag(USART1, USART_FLAG_TXE); 
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 	
    USART_Cmd(USART1, ENABLE); 
	
	/* UART2 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);	   //??UART2??
    USART_DeInit(USART2);		
    USART_InitStruct.USART_BaudRate = 9215;//8914;//9600;	
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;	
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = /*USART_Mode_Tx | */USART_Mode_Rx;
    USART_Init(USART2,&USART_InitStruct);
  /* interrupt */ 
    USART_ClearFlag(USART2,USART_FLAG_RXNE); 
	USART_ClearFlag(USART2, USART_FLAG_TXE); 
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); 	
    USART_Cmd(USART2, ENABLE);
}
/*******************************************************************************
* Function Name : WatchDog_inti
* Description   : WatvhDog initialize.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void WatvhDog_inti(void)
{
    /* IWDG timeout equal to 250 ms (the timeout may varies due to LSI frequency
       dispersion) */
    /* Enable write access to IWDG_PR and IWDG_RLR registers */
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

    /* IWDG counter clock: LSI/32 */
    IWDG_SetPrescaler(IWDG_Prescaler_32);

    /* Set counter reload value to obtain 250ms IWDG TimeOut.
       Counter Reload Value = 250ms/IWDG counter clock period
                          = 250ms / (LSI/32)
                          = 0.25s / (LsiFreq/32)
                          = LsiFreq/(32 * 4)
                          = LsiFreq/128
	     LsiFreq = 40000;
    */
    IWDG_SetReload(40000/128);

    /* Reload IWDG counter */
    IWDG_ReloadCounter();

    /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
    IWDG_Enable();
}

/*******************************************************************************
* Function Name : FunDAC_Init
* Description   : DAC initialize
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void FunDAC_Init(void)
{
	 DAC_InitTypeDef            DAC_InitStructure;
   /* DAC Periph clock enable */
     RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
   /* The sine wave has been selected */
     DAC_DeInit();         
   /* DAC channel1 Configuration */
     DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;//DAC_Trigger_T2_TRGO;
     DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
   /* DAC Channel1 Init */
     DAC_Init(DAC_Channel_1, &DAC_InitStructure);
   /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is 
   automatically connected to the DAC converter. */
     DAC_Cmd(DAC_Channel_1, ENABLE);
}
/*******************************************************************************
* Function Name : FunComp_Init
* Description   : Comparator initialize.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void FunComp_Init(void)
{ 
	 COMP_InitTypeDef  COMP_InitStruct;
	 EXTI_InitTypeDef EXTI_InitStructure;
	//COMP interrupt setting
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);           //important
	/* COMP Peripheral clock enable */
     RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);        //important
  //SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC,EXTI_PinSource14);
	 /* COMP1 interrupt */
	 EXTI_InitStructure.EXTI_Line = EXTI_Line21;
     EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
     EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
     EXTI_InitStructure.EXTI_LineCmd = ENABLE;//DISABLE; 	
     EXTI_Init(&EXTI_InitStructure);
	 COMP_DeInit();
	 COMP_InitStruct.COMP_InvertingInput = COMP_InvertingInput_3_4VREFINT;//COMP_InvertingInput_DAC1;  //Switch DAC1 to '-'
     COMP_InitStruct.COMP_Output = COMP_Output_TIM1BKIN;              //COMP_Output_TIM1OCREFCLR; //Switch compare output to TIMER3 OCREFCLR
     COMP_InitStruct.COMP_OutputPol = COMP_OutputPol_NonInverted;     //?????
     COMP_InitStruct.COMP_Hysteresis = COMP_Hysteresis_No;            //COMP_Hysteresis_Low;//            //???????
     COMP_InitStruct.COMP_Mode = COMP_Mode_HighSpeed;                 //Compare speed set
	 COMP_Init(COMP_Selection_COMP1, &COMP_InitStruct);              
	 COMP_SwitchCmd(DISABLE);                                         //disconnect PA1 to PA4
	 COMP_WindowCmd(DISABLE);                                         //Does not use Window compare
	 COMP_Cmd(COMP_Selection_COMP1, ENABLE);                          //Enable COMP1
	 COMP_Cmd(COMP_Selection_COMP2, DISABLE);                         //Disable COMP2
	 COMP_LockConfig(COMP_Selection_COMP1);                           //Lock COMP1 setting
	 COMP_LockConfig(COMP_Selection_COMP2);                           //Lock COMP2 setting
}
/*******************************************************************************
* Function Name : GPIO_Interrupt
* Description   : GPIO Interrupt.
* Input         : None.
* Output        : None.
* Return        : None.
*******************************************************************************/
void GPIO_Interrupt(void)
{
	 EXTI_InitTypeDef EXTI_InitStructure;
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);           //important
     SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource2);
	 SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource5);
	 EXTI_InitStructure.EXTI_Line = EXTI_Line2 | EXTI_Line5;
     EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
     EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
     EXTI_InitStructure.EXTI_LineCmd = ENABLE;
     EXTI_Init(&EXTI_InitStructure);
}

#include "stm32f10x.h"      // Device header
#include <stdio.h>
#include "AD7606_SPI.h"
#include "PWM.h"
#include "usart.h"
#include "delay.h"

uint8_t start_index = 0;
uint16_t FIFO[BUFFER_SIZE] = {0};  		// 上传到上位机的缓冲区
uint16_t freq = 0;		  				// 采样率
uint16_t freq_num = 0;					// 采样点数
uint16_t count = 0;						// 记录采集位置

//FIFO_T	Fifo_Buff;	// 定义一个交换缓冲区，存储AD采集数据，并传到上位机 
///*
//*	@brief:  FifoBuff_Init
//*	@param:  void 
//*	@return: void
//*	@note:   初始化环形缓冲区
//*/
//void FifoBuff_Init(void){

//	Fifo_Buff.usWrite = 0;
//	Fifo_Buff.usRead = 0;
//	Fifo_Buff.usCount = 0;
//}

///*
//*	@brief:  Write_FifoBuff
//*	@param:  写入的数据 
//*	@return: 0 / 1
//*	@note:   向缓冲区写入数据
//*/
//uint16_t Write_FifoBuff(uint16_t data){
//	// 判断缓冲区是否已满
//	if(Fifo_Buff.usCount >= FIFO_SIZE){
//		
//		return FIFOBUFF_ERR;
//	}
//	Fifo_Buff.usBuf[Fifo_Buff.usWrite] = data;
//	Fifo_Buff.usWrite = (Fifo_Buff.usWrite + 1) % FIFO_SIZE;
//	Fifo_Buff.usCount++;
//	return FIFOBUFF_OK;
//}

///*
//*	@brief:  Read_FifoBuff
//*	@param:  读出的数据 
//*	@return: 0 / 1
//*	@note:   从环形缓冲区读出一个数据
//*/
//uint16_t Read_FifoBuff(uint16_t* r_data){
//	
//	// 判断缓冲区是否为空
//	if(Fifo_Buff.usCount == 0){
//		return FIFOBUFF_ERR;
//	}
//	*r_data = Fifo_Buff.usBuf[Fifo_Buff.usRead];
//	Fifo_Buff.usRead = (Fifo_Buff.usRead + 1) % FIFO_SIZE;
//	Fifo_Buff.usCount--;
//	return FIFOBUFF_OK;
//}


/*
*	@brief:  InitAD7606
*	@param:  void
*	@return: void
*	@note:   初始化AD7606各接口
*/
void InitAD7606(void){	
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;	
	// 1.
	/* AD_SPI_MISO_GPIO,  
	   and AD_SPI_SCK_GPIO Periph clock enable */
	/* 开启SPI2端口时钟 */
	RCC_APB2PeriphClockCmd(AD_SPI_MISO_GPIO_CLK | AD_SPI_SCK_GPIO_CLK, ENABLE);
	
	/* AD_SPI Periph clock enable */  /* 初始化SPI2时钟*/
	RCC_APB1PeriphClockCmd(AD_SPI_CLK, ENABLE); 
	
	/* Configure AD_SPI pins: SCK */ /* SPI主模式下SCK设置为复用推挽输出 */
	GPIO_InitStructure.GPIO_Pin = AD_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  // 复用推挽输出  默认复用功能
	GPIO_Init(AD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
	
	/* Configure AD_SPI pins: MISO */ /* SPI主模式 全双工 浮空输入 */
	GPIO_InitStructure.GPIO_Pin = AD_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_Init(AD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);

	/* 片选 推挽输出 */ // PB3引脚重定义为GPIO  没问题
	RCC_APB2PeriphClockCmd(AD_CS_GPIO_CLK | RCC_APB2Periph_AFIO, ENABLE);   // 开启AFIO时钟
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);    			// GPIO_Remap_SWJ_JTAGDisable
    GPIO_InitStructure.GPIO_Pin = AD_CS_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  					    // 推挽输出 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* AD_SPI Config */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // 全双工
	//SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Rx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;  		// 主模式
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;  // 16位
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;         // 时钟极性CPOL=1，表示时钟信号的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;		// 这里修改了源代码 时钟相位CPHA=0，数据在第一个边沿（下降沿）采样，并且数据在时钟信号接下来的上升沿移出。
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;			// NSS为软件模式 采用普通IO口拉低电平来开启数据传输
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;  // 36M/4=9MHZ 波特率 传输速率
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	// 高位在前
	SPI_InitStructure.SPI_CRCPolynomial = 7;			// 这里虽然配置了CRC但是没有使用 因为CRC校验仅是全双工模式下使用的 收发双方都要有CRC
	SPI_Init(AD_SPI, &SPI_InitStructure);
	
	SPI_Cmd(AD_SPI, ENABLE); /* AD_SPI enable */ /* 使能SPI2外设 */
	
	/* 配置其它的GPIO */

	/* 使能GPIO时钟 */
	RCC_APB2PeriphClockCmd(AD_RESET_GPIO_CLK | AD_OS0_GPIO_CLK 
		| AD_OS1_GPIO_CLK | AD_OS2_GPIO_CLK | AD_RANGE_GPIO_CLK, ENABLE);

	/* 配置RESET GPIO */  // 没问题
	GPIO_InitStructure.GPIO_Pin = AD_RESET_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 推挽输出
	GPIO_Init(AD_RESET_GPIO_PORT, &GPIO_InitStructure);
	

	/* 配置OS0-2 GPIO */  /* 推挽输出 */  // 没问题
	GPIO_InitStructure.GPIO_Pin = AD_OS0_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 推挽输出
	GPIO_Init(AD_OS0_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AD_OS1_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 推挽输出
	GPIO_Init(AD_OS1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AD_OS2_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 推挽输出
	GPIO_Init(AD_OS2_GPIO_PORT, &GPIO_InitStructure);

    /* 配置RANGE GPIO */  // 没问题
    GPIO_InitStructure.GPIO_Pin = AD_RANGE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 推挽输出
	GPIO_Init(AD_RANGE_GPIO_PORT, &GPIO_InitStructure);

	// 初始化BUSY外部中断 下降沿触发  不在这里初始化，应该在开始记录时初始化

	// 初始化CONVST(PWM) 不在这里初始化，应该在开始记录时初始化
	
	/* 设置过采样模式 */
	ad7606_SetOS(0);
}

/*
*	@brief:  ad7606_SetInputRange
*	@param:  range: ±5V  ±10V
*	@return: void
*	@note:   配置AD7606模拟信号量输入量程
*/
void ad7606_SetInputRange(uint8_t range){
	// 低电平5V
	if(range == 0){
		AD_RANGE_5V(); 
	}
	else{
		AD_RANGE_10V(); 
	}
}

/*
*	@brief:  ad7606_Reset
*	@param:  void
*	@return: void
*	@note:   硬件复位AD7606
*/
void ad7606_Reset(void){
	/* AD7606是高电平复位，要求最小脉宽50ns */
	
	AD_RESET_LOW();
	
	AD_RESET_HIGH();  /* 仅用于延迟。 RESET复位高电平脉冲宽度最小50ns。 */
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	
	AD_RESET_LOW();
}


/*
*	@brief:  ad7606_SetOS
*	@param:  _ucMode : 0-6  0表示无过采样，1表示2倍，2表示4倍，3表示8倍，4表示16倍
*	@return: void
*	@note:   设置过采样模式（数字滤波，硬件求平均值)
*/
void ad7606_SetOS(uint8_t _ucMode){

	switch(_ucMode){
		case 1: AD_OS2_0(); AD_OS1_0(); AD_OS0_1();
		break;
		case 2: AD_OS2_0(); AD_OS1_1(); AD_OS0_0();
		break;
		case 3: AD_OS2_0(); AD_OS1_1(); AD_OS0_1();
		break;
		case 4: AD_OS2_1(); AD_OS1_0(); AD_OS0_0();
		break;
		case 5: AD_OS2_1(); AD_OS1_0(); AD_OS0_1();
		break;
		case 6: AD_OS2_1(); AD_OS1_1(); AD_OS0_0();
		break;
		default: AD_OS2_0(); AD_OS1_0(); AD_OS0_0();
		break;
	}
}


/*
*	@brief:  ad7606_StartConv
*	@param:  void
*	@return: void
*	@note:   启动AD7606的ADC转换
*/
void ad7606_StartConv(void){
	/* 上升沿开始转换，低电平持续时间至少25ns  */
	AD_CONVST_LOW();
	AD_CONVST_LOW();
	AD_CONVST_LOW();	/* 连续执行2次，低电平约50ns */
	
	AD_CONVST_HIGH();
}


/*
*	@brief:  spi_ReadBytes
*	@param:  void
*	@return: 读取到的一个通道的数据：16位
*	@note:   读取AD7606的采样结果
*/
uint16_t spi_ReadBytes(void){
		
	uint16_t usData = 0;  // 16位

	/* Wait until the transmit buffer is empty */ /* 等待发送区空 */
	while (SPI_I2S_GetFlagStatus(AD_SPI, SPI_I2S_FLAG_TXE) == RESET)
	{
	}

	/* Send the byte */ /* 发送一个0xFFFF */
	SPI_I2S_SendData(AD_SPI, 0xFFFF);

	/* Wait until a data is received */ /* 等待接受区空 */
	while (SPI_I2S_GetFlagStatus(AD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
	{
	}

	/* Get the received data */ /* 存储最近接收的两个byte*/
	usData = SPI_I2S_ReceiveData(AD_SPI);
	
	/* Return the shifted data */ /* 返回接收到的两个byte */
	return usData;
}


/*
*	@brief:  ad7606_ReadBytes
*	@param:  void
*	@return: void
*	@note:   定时调用本函数，用于读取AD转换器数据  存储在FIFO缓冲区中
*/
void ad7606_ReadBytes(void){			
	AD_CS_LOW();								 /* 读取数据 先将CS拉低 拉低CS后，不能马上读取 示波器监测，CS低电平持续时间 35us  */
	for (uint8_t i = 0; i < CH_NUM; i++)		 // 采集两通道
	{
		// 方式
		FIFO[count] = spi_ReadBytes();
		count++;
		if(count == freq_num)
		{
			ad7606_StopRecord();
			count = 0;
			start_index = 1;
		}
		// Serial_SendPacket(spi_ReadBytes());
	}
	
	AD_CS_HIGH();						 // CS拉高
}



/*
*	@brief:  ad7606_StartRecord
*	@param:  _ulFreq : 采样频率, 单位 HZ: 1000时为1KHz
*	@return: void
*	@note:   开始采集
*/
void ad7606_StartRecord(uint32_t _ulFreq){
	// ad7606_StartConv();
	/* 设置GPIO的初始状态 */
	ad7606_Reset();				/* 硬件复位AD7606 */
	AD_RANGE_5V();              /* 设置AD7606输入范围±5V */
	AD_CONVST_HIGH();			/* CONVST起始为高电平 */
	ad7606_SetOS(0);

	// ad7606_StartConv();				/* 启动采样，避免第1组数据全0的问题 */

	BUSY_EXTI_Init();               	/* 使能BUSY下降沿中断 */

	PWM_Config();						/* 使能PWM */
	PWM_Init(_ulFreq, _ulFreq - 1);		// 设置采样频率 1KHz 占空比999 //
}


/*
*	@brief:  ad7606_StopRecord
*	@param:  void
*	@return: void
*	@note:   停止采集
*/
void ad7606_StopRecord(void){

	TIM_Cmd(TIM4, DISABLE);					/* 失能TIM4定时器 */
	/* 将PB6重新配置为普通输出口 */
	GPIO_InitTypeDef GPIO_InitStructure;
	/* 使能GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  // 推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &GPIO_InitStructure);
	AD_CS_HIGH();		 // CS高电平
	AD_CONVST_HIGH();    // CONVST设置为高电平
}


/*
*	@brief:  BUSY_EXTI_Init
*	@param:  void
*	@return: void
*	@note:   BUSY中断
*/
void BUSY_EXTI_Init(void){

	// 开启GPIOA外设的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    // 开启AFIO外设的时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);	// 重定义PA15为IO口
	// EXTI 和 NVIC（内核外设） 外设时钟一直都是打开着的，不需要自己手动开启

    // 配置GPIO
    GPIO_InitTypeDef GPIO_InitStrcuture;
	
    // 上拉输入模式 在参考手册里有GPIO外设配置表

    GPIO_InitStrcuture.GPIO_Mode = GPIO_Mode_IPU; 
        // PA15      
    GPIO_InitStrcuture.GPIO_Pin = GPIO_Pin_15;          
    GPIO_InitStrcuture.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStrcuture);

    // 配置AFIO  
    // 用的PA15号引脚
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15);

    // 配置EXTI
    // 将EXTI的第15个线路配置为外部中断模式，下降沿触发
    EXTI_InitTypeDef EXTI_InitStructure;
        // 15线路
    EXTI_InitStructure.EXTI_Line = EXTI_Line15;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        // 中断模式
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        // 下降沿触发
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
        // 开启中断
    EXTI_Init(&EXTI_InitStructure);

    // 配置NVIC  在misc.h中
        // 配置优先级分组  分组方式整个芯片只能用一种
        // 按理说这个分组的代码在整个工程只需要执行一次就可以了
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
        // 初始化NVIC
    NVIC_InitTypeDef NVIC_InitStructure;
        // 在STM32F10X_MD中等密度的芯片中找 EXTI的第15线路在EXTI15_10_IRQn里  
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
        // 指定所选通道的抢占优先级
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
        // 指定所选通道的响应优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
}


/*
*	@brief:  EXTI15_10_IRQHandler
*	@param:  void
*	@return: void
*	@note:   中断函数（名字固定），在Start中的startup_stm32f10x_md.s中找 不需要在头文件声明 自动调用
*/
void EXTI15_10_IRQHandler(void){
    // 判断EXTI15的中断标志位是不是为1  // 因为这个函数EXTI10-EXTI15都能进来
    if(EXTI_GetITStatus(EXTI_Line15) == SET){
        // 执行中断函数
        /*如果出现数据乱跳的现象，可再次判断引脚电平，以避免抖动*/
		if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15) == 0)
		{
			// 进行SPI读取
			ad7606_ReadBytes();
			
		}
        // 将EXTI15的中断标志位置为0  因为只要中断标志位置1，就会执行中断函数
        // 如果不进行清除，就会一直触发中断
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}






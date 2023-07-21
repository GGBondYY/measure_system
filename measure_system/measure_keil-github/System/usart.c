#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "usart.h"
#include <string.h>

uint8_t Serial_RxData;				// 处理接收的一个数据
uint8_t Serial_RxFlag;				// 接收完成标志位

uint8_t Serial_RxPacket[7];			// 接收数据包  包括包头0xFA和包尾0xFB 格式:FA AA XX XX XX XX FB 开始采集    FA BB XX XX XX XX FB 停止采集
uint8_t Serial_TxPacket[4];			// 发送数据包

/*
*	@brief:  串口初始化
*	@param:  void
*	@return: void
*/
void Serial_Init(void){

    // 1. 开启时钟 USART3    
    RCC_APB1PeriphClockCmd(USART3_CLK, ENABLE);       	// 开启USART3的时钟   
    RCC_APB2PeriphClockCmd(USART_GPIO_CLK , ENABLE);	// 开始GPIO时钟

    // 2. 初始化GPIO引脚
    
    GPIO_InitTypeDef GPIO_InitStructure;
    // TX PB10复用推挽输出
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
    GPIO_InitStructure.GPIO_Pin = USART_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_GPIO_PORT, &GPIO_InitStructure);

    // RX
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = USART_RX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(USART_GPIO_PORT, &GPIO_InitStructure);

    // 3. 初始化USART
    USART_InitTypeDef USART_InitStructure;
        
    USART_InitStructure.USART_BaudRate = 115200;										// 波特率
        
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		// 硬件流控制
        
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;						// 串口模式 
        
    USART_InitStructure.USART_Parity = USART_Parity_No;									// 校验位 无校验
        
    USART_InitStructure.USART_StopBits = USART_StopBits_1;								// 停止位 1位
        
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;							// 字长 不需要校验，选择8位
        
    USART_Init(USART3, &USART_InitStructure);											// 初始化

    // 4. 开启中断  选择RXNE的中断  开启中断一般都需要配置这个ITconfig
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);

    // 5. 配置NVIC
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);										// 分组
	// 配置NVIC的USART3通道
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;							
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    // 6.USART3使能
    USART_Cmd(USART3, ENABLE);
}


/*
*	@brief:  发送一个字节
*	@param:  uint8_t Byte：一个字节数据
*	@return: void
*/
void Serial_SendByte(uint8_t Byte){
    // 调用发送函数  Byte变量写入TDR
    USART_SendData(USART3, Byte);
    // 等待将TDR数据搬运至发送移位寄存器，如果不等待会出现数据覆盖
    // 判断发送数据寄存器是否为空，TXE标志位是否置1  不需要手动清除标志位为0，下一次写入的时候，会自动清0
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
}


/*
*	@brief:  发送一个数组函数
*	@param:  Array数组，Length数组长度
*	@return: void 
*/
void Serial_SendArray(uint8_t* Array, uint16_t Length){
	
    uint16_t i;
    for (i = 0; i < Length; ++i){
        Serial_SendByte(Array[i]);
    }
}

 
/*
*	@brief:  发送数据包   数据包大小：4字节
*	@param:  two_byte:FIFO缓冲区的一个数据
*	@return: void   
*/
void Serial_SendPacket(uint16_t two_byte){
	
	uint8_t High, Low;
	
	// 高8位
	High = (two_byte >> 8) & 0xFF;
	// 低8位
	Low = two_byte & 0xFF;

	
	// 包头0xFA
	Serial_TxPacket[0] = 0xFA;
	// 高8位
    Serial_TxPacket[1] = High;
	// 低8位
	Serial_TxPacket[2] = Low;
	// 包尾0xFB
	Serial_TxPacket[3] = 0xFB;
	// 发送数据包
    Serial_SendArray(Serial_TxPacket, 4);
}

/*
*	@brief:  获取接收数据RXEN标志位
*	@param:  void
*	@return: void
*/
uint8_t Serial_GetRxFlag(void){
    if(Serial_RxFlag == 1){
        Serial_RxFlag = 0;
        return 1;
    }
    return 0;
}
 
/*
*	@brief:  中断函数  接收上位机指令
*	@param:  void
*	@return: void
*/
void USART3_IRQHandler(void){

    static uint8_t RxState = 0;
    static uint8_t pRxPacket = 0;  // 接收数据的个数
    // 在中断中判断标志位都是带IT的Get
    if(USART_GetITStatus(USART3, USART_IT_RXNE) == SET){
        uint8_t RxData = USART_ReceiveData(USART3);
        // 状态机
        // 等待包头
        if(RxState == 0){
            // 收到包头 状态转移
            if(RxData == 0xFA){
                RxState = 1;
                pRxPacket = 0;
				// 写入包头
				Serial_RxPacket[pRxPacket] = RxData;
				pRxPacket++;
            }
        }
        // 接收数据
        else if(RxState == 1){
            Serial_RxPacket[pRxPacket] = RxData;
            pRxPacket++;
            // 数据接受完 状态转移
            if(pRxPacket == 6){
                RxState = 2;
            }
        }
        // 接收包尾
        else if(RxState == 2){
            // 状态转移
            if(RxData == 0xFB){
				Serial_RxPacket[pRxPacket] = RxData;
                RxState = 0;
                // 一个数据包接收完毕的数据位
                Serial_RxFlag = 1;
            }
        }
        
        // 清除标志位
        // 在中断中清除标志位都是带IT的Clear
        USART_ClearITPendingBit(USART3, USART_IT_RXNE);
    }
}



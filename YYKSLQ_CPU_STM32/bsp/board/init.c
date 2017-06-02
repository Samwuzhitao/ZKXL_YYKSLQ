/**
  ******************************************************************************
  * @file   	init.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	platform init functions
  ******************************************************************************
  */

#include "main.h"
#include "nrf.h"
#include "app_timer.h"
#include "task_spi_message.h"
#include "task_find_card.h"

typedef struct
{
	uint8_t header;
	uint8_t cmd;
	uint8_t channel;
	uint8_t data_xor;
	uint8_t end;
}cpu_spi_cmd_typedef;
	
/* Private variables ---------------------------------------------------------*/
spi_cmd_type_t 			    spi_cmd_type;
nrf_communication_t	    nrf_data;
uint8_t 						    gbf_hse_setup_fail = 0;		// 外部晶振起振标志
RF_TypeDef 				      rf_var;
extern wl_typedef       wl;
extern uint8_t dtq_self_inspection_flg;

void systick_timer_init( void );
/*******************************************************************************
  * @brief  硬件平台初始化
  * @param  None
  * @retval None
  * @note 	None
*******************************************************************************/
void Platform_Init(void)
{
	uint8_t status = 0;

	/* disable all IRQ */
	DISABLE_ALL_IRQ();

	/* initialize system clock */
	SysClockInit();

	/* initialize gpio port */
	GpioInit();

	Usart1_Init();

	/* get mcu uuid */
	get_mcu_uid();

	/* initialize the spi interface with nrf51822 */
	nrf51822_spi_init();

	/* eeprom init and white_list init*/
	Fee_Init(FEE_INIT_POWERUP);
	get_white_list_from_flash();

	/* init software timer */
	sw_timer_init();
	system_timer_init();
	card_timer_init();
	spi_rev_timer_init();

	/* 复位并初始化RC500 */
	status = mfrc500_init();
	printf("[ INIT ] MF1702 INIT: %s !\r\n", (status == 0) ? "OK" : "FAIL");

	/* led 、蜂鸣器声音提示初始化完毕 */
#ifndef OPEN_SILENT_MODE
	BEEP_EN();
#endif

	status = clicker_config_default_set();
	printf("[ INIT ] SPI SET CPU RF : %s !\r\n", (status == 0) ? "OK" : "FAIL");

	IWDG_Configuration();

	/* enable all IRQ */
	ENABLE_ALL_IRQ();
	ledOn(LRED);
	ledOn(LBLUE);
	DelayMs(200);
	BEEP_DISEN();
	ledOff(LBLUE);
	ledOff(LRED);
}

/****************************************************************************
* 名    称：void Usart1_Init(void)
* 功    能：串口1初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无
****************************************************************************/
void Usart1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(USART1pos_CLK , ENABLE);

	GPIO_InitStructure.GPIO_Pin = USART1pos_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(USART1pos_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USART1pos_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART1pos_GPIO, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
	USART_Init(USART1pos, &USART_InitStructure);

	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART1_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART1_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//中断配置..Only IDLE Interrupt..
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

	/* Enable the USART1 */
	USART_Cmd(USART1pos, ENABLE);
}

/****************************************************************************
* 名    称：void Usart2_Init(void)
* 功    能：串口2初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无
****************************************************************************/
void Usart2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(USART2pos_CLK , ENABLE);

	GPIO_InitStructure.GPIO_Pin = USART2pos_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(USART2pos_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USART2pos_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART2pos_GPIO, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
	USART_Init(USART2pos, &USART_InitStructure);

	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART2_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART2_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//中断配置..Only IDLE Interrupt..
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

	/* Enable the USART1 */
	USART_Cmd(USART2pos, ENABLE);
}

#ifdef ZL_RP551_MAIN_E
void GPIOInit_SE2431L(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = SE2431L_CTX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SE2431L_CTX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SE2431L_CPS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SE2431L_CPS_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SE2431L_CSD_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SE2431L_CSD_PORT, &GPIO_InitStructure);

	SE2431L_Bypass();
}

void SE2431L_LNA(void)
{
	GPIO_SetBits(SE2431L_CPS_PORT, SE2431L_CPS_PIN);	//110 LNA. 111 TX. 010 bypass.
	GPIO_SetBits(SE2431L_CSD_PORT, SE2431L_CSD_PIN);
	GPIO_ResetBits(SE2431L_CTX_PORT, SE2431L_CTX_PIN);
}

void SE2431L_Bypass(void)
{
	GPIO_ResetBits(SE2431L_CPS_PORT, SE2431L_CPS_PIN);	//110 LNA. 111 TX. 010 bypass.
	GPIO_SetBits(SE2431L_CSD_PORT, SE2431L_CSD_PIN);
	GPIO_ResetBits(SE2431L_CTX_PORT, SE2431L_CTX_PIN);
}

void SE2431L_TX(void)
{
	GPIO_SetBits(SE2431L_CPS_PORT, SE2431L_CPS_PIN);	//110 LNA. 111 TX. 010 bypass.
	GPIO_SetBits(SE2431L_CSD_PORT, SE2431L_CSD_PIN);
	GPIO_SetBits(SE2431L_CTX_PORT, SE2431L_CTX_PIN);
}
#endif

/* Private functions ---------------------------------------------------------*/
static uint8_t hal_nrf_rw(SPI_TypeDef* SPIx, uint8_t value)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPIx, value);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	return(SPI_I2S_ReceiveData(SPIx));
}

uint8_t spi_read_tx_payload(SPI_TypeDef* SPIx, uint8_t *rx_data_len, uint8_t *rx_data)
{
	typedef struct
	{
		uint8_t 				spi_header;
		uint8_t         cmd;
		uint8_t         signal;
		uint8_t					data_len;
		uint8_t 				data[BUFFER_SIZE_MAX];
		uint8_t 				xor;
	} nrf_to_stm32_cmd_type_t;

	nrf_to_stm32_cmd_type_t spi_revice_data;
	uint8_t retval[BUFFER_SIZE_MAX];
	uint8_t i = 0;
	uint8_t *temp_data = NULL;
	uint8_t revice_cal_xor = 0;

	*rx_data_len = 0;
	memset(spi_revice_data.data, 0xFF, BUFFER_SIZE_MAX);
	spi_revice_data.spi_header = 0x86;
	spi_cmd_type.data_len = 0x06;
	temp_data = (uint8_t *)&spi_cmd_type;

	/* 开始SPI传输 */
	NRF1_CSN_LOW();	
	memset(retval, 0, BUFFER_SIZE_MAX);
	//printf("SPI_RX:");

	for(i=0; i<spi_cmd_type.data_len; i++)
	{
		retval[i] = hal_nrf_rw(SPIx, *(temp_data+i));
		//printf(" %02x",retval[i]);
		if(i ==  3 && retval[0] == 0x86 && retval[3] < BUFFER_SIZE_MAX )
		{
			*rx_data_len = retval[3];
			spi_cmd_type.data_len += *rx_data_len;
		}
	}
	//printf("\r\n");

	/* 关闭SPI传输 */
	NRF1_CSN_HIGH();	
	revice_cal_xor = XOR_Cal((uint8_t *)&(retval[1]), spi_cmd_type.data_len-3);
  //printf("%02x ?= %02x \r\n",retval[spi_cmd_type.data_len-2],revice_cal_xor);

  /* 若接收到数据校验正确 */
	if(retval[spi_cmd_type.data_len-2] == revice_cal_xor) 			
	{
		rx_data[0] = retval[2];
		memcpy(rx_data+1, &retval[4],*rx_data_len);
		*rx_data_len = *rx_data_len + 1;
		return 0;
	}
	else
	{
		return 1;
	}
}

void spi_write_tx_payload(const uint8_t *tx_pload, uint8_t length, uint8_t count, uint8_t delay100us)
{
	uint8_t retval[BUFFER_SIZE_MAX];
	uint16_t i = 0;
	uint8_t *pdata;

	/* 封装指令 */
	spi_cmd_type.spi_header = 0x86;
	spi_cmd_type.cmd        = 0x10;
	spi_cmd_type.count      = count;
	spi_cmd_type.delay100us = delay100us;
	spi_cmd_type.data_len   = length;
	memcpy(spi_cmd_type.data, tx_pload, length);

	spi_cmd_type.data[spi_cmd_type.data_len] = XOR_Cal((uint8_t *)&(spi_cmd_type.cmd), spi_cmd_type.data_len+4);
	spi_cmd_type.data[spi_cmd_type.data_len+1] = 0x76;
	
	/* 开始SPI传输 */
	NRF2_CSN_LOW();	
	memset(retval, 0, BUFFER_SIZE_MAX);
	//printf("SPI_TX:");
	pdata = (uint8_t *)&spi_cmd_type;
	for(i=0; i<spi_cmd_type.data_len+7; i++)
	{
#ifdef ZL_RP551_MAIN_E
	retval[i] = hal_nrf_rw(SPI1, *(pdata+i));
#endif

#ifdef ZL_RP551_MAIN_F
		retval[i] = hal_nrf_rw(SPI2, *(pdata+i));
#endif
		//printf(" %02x",*(pdata+i));
	}
	//printf("\r\n");

	/* 关闭SPI传输 */
	NRF2_CSN_HIGH();
}


uint8_t spi_set_cpu_rx_signal_ch( uint8_t rx_ch )
{
	uint8_t status = 0;
	uint8_t set_count = 0;
	uint8_t nop = 0xFF;

	uint16_t data = rx_ch;
	cpu_spi_cmd_typedef spi_cmd_w,spi_cmd_r;
	uint8_t *pdata,i;
	
	/* 存储数据到FEE */
	EE_WriteVariable(CPU_RX_CH_POS_OF_FEE,data);

	/* 封装指令 */
	spi_cmd_w.header   = 0x86;
	spi_cmd_w.cmd      = 0x20;
	spi_cmd_w.channel  = rx_ch;
	spi_cmd_w.data_xor = XOR_Cal((uint8_t *)&(spi_cmd_w.cmd), 2);
	spi_cmd_w.end      = 0x76;

	do
	{
		/* 开始SPI传输 */
		NRF1_CSN_LOW();
		pdata = (uint8_t *)&spi_cmd_w;	

		//printf("SPI_RX_SET_CH  :");
		for(i=0; i<sizeof(cpu_spi_cmd_typedef); i++)
		{
			hal_nrf_rw(SPI1, *(pdata+i));
			//printf(" %02x",*(pdata+i));
		}
		//printf("\r\n");
		NRF1_CSN_HIGH();

    // 等待参数设置成功
		DelayMs(10);
		
		NRF1_CSN_LOW();
		pdata = (uint8_t *)&spi_cmd_r;	
		memset(pdata , 0, sizeof(cpu_spi_cmd_typedef));
		//printf("SPI_RX_CHECK_CH:");
		for(i=0; i<sizeof(cpu_spi_cmd_typedef); i++)
		{
			*(pdata+i) = hal_nrf_rw(SPI1, nop);
			//printf(" %02x",*(pdata+i));
		}
		//printf("\r\n");
		NRF1_CSN_HIGH();
		if( spi_cmd_r.cmd == 0x21 )
		{
			if( spi_cmd_w.channel == spi_cmd_r.channel )
			{
				status = 0;
				set_count = 3;
			}
			else
			{
				status = 1;
				set_count++;
			}
		}
		else
		{
			status = 1;
			set_count++;
		}
	}while( set_count < 3 );
	
	return status;
}

uint8_t spi_set_cpu_tx_signal_ch( uint8_t tx_ch )
{
	uint8_t status = 0;
	uint8_t set_count = 0;
	uint8_t nop = 0xFF;
	
	uint16_t data = tx_ch;
	cpu_spi_cmd_typedef spi_cmd_w,spi_cmd_r;
	uint8_t *pdata,i,retval[sizeof(cpu_spi_cmd_typedef)];
	
	/* 存储数据到FEE */
	EE_WriteVariable(CPU_TX_CH_POS_OF_FEE,data);
	
	/* 封装指令 */
	spi_cmd_w.header   = 0x86;
	spi_cmd_w.cmd      = 0x20;
	spi_cmd_w.channel  = tx_ch;
	spi_cmd_w.data_xor = XOR_Cal((uint8_t *)&(spi_cmd_w.cmd), 2);
	spi_cmd_w.end      = 0x76;

	do
	{
		/* 开始设置SPI数据 */
		NRF2_CSN_LOW();	
		//printf("SPI_TX_SET_CH  :");
		pdata = (uint8_t *)&spi_cmd_w;
		memset(retval, 0, sizeof(cpu_spi_cmd_typedef));
		for(i=0; i<sizeof(cpu_spi_cmd_typedef); i++)
		{
			#ifdef ZL_RP551_MAIN_E
			hal_nrf_rw(SPI1, *(pdata+i));
			#endif

			#ifdef ZL_RP551_MAIN_F
			hal_nrf_rw(SPI2, *(pdata+i));
			#endif
			//printf(" %02x",*(pdata+i));
		}
		//printf("\r\n");
		NRF2_CSN_HIGH();
		
    // 等待参数设置成功
		DelayMs(10);
		
		/* 读取设置结果 */
		NRF2_CSN_LOW();	
		pdata = (uint8_t *)&spi_cmd_r;
		memset(pdata , 0, sizeof(cpu_spi_cmd_typedef));
		//printf("SPI_TX_CHECK_CH:");
		for(i=0; i<sizeof(cpu_spi_cmd_typedef); i++)
		{
			#ifdef ZL_RP551_MAIN_E
			*(pdata+i) = hal_nrf_rw(SPI1, nop);
			#endif

			#ifdef ZL_RP551_MAIN_F
			*(pdata+i) = hal_nrf_rw(SPI2, nop);
			#endif
			//printf(" %02x",*(pdata+i));
		}
		//printf("\r\n");
		NRF2_CSN_HIGH();
		
	  if( spi_cmd_r.cmd == 0x21 )
		{
			if( spi_cmd_w.channel == spi_cmd_r.channel )
			{
				status = 0;
				set_count = 3;
			}
			else
			{
				status = 1;
				set_count++;
			}
		}
		else
		{
			status = 1;
			set_count++;
		}
	}while( set_count < 3 );

	return status;
}

/**************************************END OF FILE****************************/


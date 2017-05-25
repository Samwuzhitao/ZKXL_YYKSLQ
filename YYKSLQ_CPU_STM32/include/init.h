/**
  ******************************************************************************
  * @file   	init.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	platform init functions's declaritions
  ******************************************************************************
  */
#ifndef _INIT_H_
#define _INIT_H_

#include "define.h"
#include "whitelist.h"

typedef struct 
{
	uint8_t cmd;
	uint8_t	tx_buf[RF_NBUF];					
	uint8_t tx_len;
}RF_TypeDef;

typedef struct
{
	uint8_t  uid[4];
	uint16_t pos;
	uint8_t  rev_num;
	uint8_t  rev_seq;
	uint8_t  rssi;
}Clicker_Typedef;

typedef struct
{
	uint8_t  uid[4];
	uint8_t  sen_num;
	uint8_t  sen_seq;
	uint8_t  pre_seq;
	uint8_t  uart_pac_num;
	uint8_t  uart_seq_num;
	uint32_t data_statistic_count;
	uint8_t  addr_clone_flag;
}revicer_typedef;

typedef struct
{
	Clicker_Typedef uids[120];
	uint16_t   len;
	uint16_t   first_uid_pos;
	uint8_t    is_printf_clear_uid;
	uint8_t    clear_uid[4];
	uint8_t    switch_status;
	uint8_t    start;
	uint8_t    attendance_sttaus;
	uint8_t    match_status;
	uint8_t    weite_std_id_status;
}wl_typedef;

extern bool 						  gbf_hse_setup_fail; //�ⲿ16M���������־
extern RF_TypeDef 				rf_var;							// 2.4G���ݰ�����
extern uint8_t 						respon[];

void Platform_Init(void);
void Usart1_Init(void);
uint8_t spi_set_cpu_tx_signal_ch( uint8_t tx_ch );
uint8_t spi_set_cpu_rx_signal_ch( uint8_t rx_ch );
#endif //_INIT_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/


#ifndef __MY_RADIO_H
#define	__MY_RADIO_H

#include "my_radio.h"
#include "nrf_esb.h"
#include "nrf_error.h"
#include "my_uart.h"
#include "sdk_common.h"
#include "my_misc_fun.h"
#include "my_timer.h"
#include "nrf_delay.h"
#include "my_spi.h"
#include "ringbuf2.h"

#define 	 NRF_MAX_NUMBER_OF_RETRANSMITS		(3)					//����ط�����
//#define		 NRF_RETRANSMIT_DELAY				(280)				//0~255ms���ֵ
#define 	 NRF_PIPE						(0)						//2.4GƵ�㡢
#define		 NRF_ACK_PROTOCOL_LEN			(17)					//�Զ�������ģ��ACKЭ�����ݳ���
#define		 NRF_TOTAL_DATA_LEN				(250)				//2.4G�����ܳ���	
#define		 NRF_USEFUL_DATA_LEN		(NRF_TOTAL_DATA_LEN - NRF_ACK_PROTOCOL_LEN)	//��Ч���ݳ���
#define		 NRF_DATA_IS_USE				(0)
#define      NRF_DATA_IS_ACK				(1)
#define 	 NRF_DATA_NEED_ACK				(1)
#define		 NRF_DATA_NEEDLESS_ACK			(2)
#define 	 NRF_DATA_HEAD					(0x61)
#define      NRF_DATA_END                   (0x21)
#define      NRF_DATA_RESERVE				(0xFF)  //����λĬ��ֵ


extern nrf_esb_payload_t        tx_payload;
extern nrf_esb_payload_t        rx_payload;



uint32_t my_tx_esb_init(void);
uint32_t my_rx_esb_init(void);
void nrf_rx_data_handler(void);

#endif 


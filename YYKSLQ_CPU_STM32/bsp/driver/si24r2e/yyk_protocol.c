/**
  ******************************************************************************
  * @file   	yyk_protocol.c
  * @author  	Samwu
  * @version 	V1.0.0.0
  * @date   	2017.05.27
  * @brief   	si24r2e ��������
  ******************************************************************************
  */

#include "main.h"
#include "yyk_protocol.h"
#include "si24r2e.h"

/* �п�Ѷ��Э�� -------------------------------------------------------------*/
static const yyk_pro_tyedef zkxl_yyk_pro = 
{
	"�п�Ѷ��",
	{
		81,                                                     // tx_ch
		1000,                                                   // speed
		0,                                                      // tx_power
		5,                                                      // addr_len
		{ 0x01, 0x02, 0x03, 0x04, 0x05 },                       // address
		8,                                                      // data_len
		{ 0x00, 0x00, 0x00, 0x66, 0x55, 0x44, 0x00, 0x07 },     // data
		1100                                                    // send_delay
	}
};

yyk_pro_tyedef *yyk_pro_list[YYK_PROTOCOL_MUM] = 
{
	(yyk_pro_tyedef *)&zkxl_yyk_pro,
	NULL,
};

int8_t yyk_protocol_update( yyk_pro_tyedef *pprotocol )
{
	/* ��ӡЭ���� */
	printf("pro_name:%s",pprotocol->name);
	/* ͬ���ŵ� */
	if(pprotocol->conf.tx_ch < 127)
		txbuf[NVM_RF_CHA] = (txbuf[NVM_RF_CHA] & 0x80) | pprotocol->conf.tx_ch; 
	else
		return -1;

	/* ͬ�������� */
	if(pprotocol->conf.tx_speed <= 2000)
	{
		txbuf[NVM_RF_SETUP] = (txbuf[NVM_RF_SETUP] & 0xC7);
		switch( pprotocol->conf.tx_speed )
		{
			case 1000: break;
			case 2000: txbuf[NVM_RF_SETUP] |= 0x08; break;
			case 250 : txbuf[NVM_RF_SETUP] |= 0x20; break;
			default: break;
		}
	}
	else
		return -2;

	/* ͬ���������� */
	if(pprotocol->conf.tx_speed <= 2000)
	{
		txbuf[NVM_RF_SETUP] = (txbuf[NVM_RF_SETUP] & 0xC7);
		switch( pprotocol->conf.tx_speed )
		{
			case 1000: break;
			case 2000: txbuf[NVM_RF_SETUP] |= 0x08; break;
			case 250 : txbuf[NVM_RF_SETUP] |= 0x20; break;
			default: break;
		}
	}
	else
		return -2;

	/* ͬ�����͹��� */
	if((pprotocol->conf.tx_power <= 7) && (pprotocol->conf.tx_power >= -12))
	{
		txbuf[NVM_RF_SETUP] = (txbuf[NVM_RF_SETUP] & 0xF8);
		switch( pprotocol->conf.tx_power )
		{
			case -12: break;
			case -6 : txbuf[NVM_RF_SETUP] |= 0x01; break;
			case -4 : txbuf[NVM_RF_SETUP] |= 0x02; break;
			case  0 : txbuf[NVM_RF_SETUP] |= 0x03; break;
			case  1 : txbuf[NVM_RF_SETUP] |= 0x04; break;
			case  3 : txbuf[NVM_RF_SETUP] |= 0x05; break;
			case  4 : txbuf[NVM_RF_SETUP] |= 0x06; break;
			case  7 : txbuf[NVM_RF_SETUP] |= 0x07; break;
			default: break;
		}
	}
	else
		return -3;
	
	/* ͬ����ַ��Ϣ */
	if(pprotocol->conf.addr_len <= 5)
	{
		uint8_t no_addr = 0;
		txbuf[NVM_FEATURE] = (txbuf[NVM_FEATURE] & 0xFC);
		switch( pprotocol->conf.addr_len )
		{
			case  3 : txbuf[NVM_FEATURE] |= 0x01; break;
			case  4 : txbuf[NVM_FEATURE] |= 0x02; break;
			case  5 : txbuf[NVM_FEATURE] |= 0x03; break;
			default : no_addr = 1;                break;
		}
		if(no_addr == 0)
			memcpy(txbuf+NVM_TX_ADDR_0,pprotocol->conf.addr,
		         pprotocol->conf.addr_len);
	}
	else
		return -4;

	/* ͬ���������� */
	if(pprotocol->conf.data_len <= 32)
	{
		txbuf[NVM_FIFO_LEN]  = (txbuf[NVM_FIFO_LEN] & 0xC0);
		txbuf[NVM_FIFO_LEN] |= pprotocol->conf.data_len;
		memcpy(txbuf+NVM_TX_DATA_0,pprotocol->conf.data,
		         pprotocol->conf.data_len);
	}
	else
		return -5;

	return 0;
}




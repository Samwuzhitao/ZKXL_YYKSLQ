/**
  ******************************************************************************
  * @file   	si24r2e.c
  * @author  	Samwu
  * @version 	V1.0.0.0
  * @date   	2017.05.27
  * @brief   	si24r2e ��������
  ******************************************************************************
  */

#include "main.h"
#include "si24r2e.h"
#include "yyk_protocol.h"

u8 txbuf[52] ={0x74,0x51,0xc0,0x8d,0x05,0xdc,0x02,0x08,0x01,0x02,
							 0x03,0x04,0x05,0xef,0x03,0x01,0x07,0x33,0x34,0xa9};
u8 txdata[10]={0x00,0x00,0x00,0x66,0x55,0x44,0x00,0x07};

/******************************************************************************
 * ��������SPI_NRF_RW
 * ����  ��������NRF��/дһ�ֽ�����
 * ����  ��д�������
 * ���  ����ȡ�õ�����
 * ����  ���ڲ�����
 *****************************************************************************/
static u8 SPI_NRF_RW(u8 dat)
{  	
	/* �� SPI���ͻ������ǿ�ʱ�ȴ� */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
	/* ͨ�� SPI2����һ�ֽ����� */
  SPI_I2S_SendData(SPI2, dat);
	/* ��SPI���ջ�����Ϊ��ʱ�ȴ� */
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
  /* Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI2);
}

/******************************************************************************
 * ��������SPI_NRF_WriteReg
 * ����  ��������NRF�ض��ļĴ���д������
 * ����  ��reg:NRF������+�Ĵ�����ַ��
 		   dat:��Ҫ��Ĵ���д�������
 * ���  ��NRF��status�Ĵ�����״̬
 * ����  ���ڲ�����
 *****************************************************************************/
static u8 SPI_NRF_WriteReg(u8 reg,u8 dat)
{
 	u8 status;
	/*�õ�CSN��ʹ��SPI����*/
 	NRF2_CSN_LOW();		
	/*��������Ĵ����� */
	status = SPI_NRF_RW(reg); 
	/*��Ĵ���д������*/
	SPI_NRF_RW(dat);       
	/*CSN���ߣ����*/	   
	NRF2_CSN_HIGH();	
	/*����״̬�Ĵ�����ֵ*/
	return(status);
}

/******************************************************************************
 * ��������SPI_NRF_ReadReg
 * ����  �����ڴ�NRF�ض��ļĴ�����������
 * ����  ��reg:NRF������+�Ĵ�����ַ��
 * ���  ���Ĵ����е�����
 * ����  ���ڲ�����
 *****************************************************************************/
static u8 SPI_NRF_ReadReg(u8 reg)
{
 	u8 reg_val;
	/*�õ�CSN��ʹ��SPI����*/
 	NRF2_CSN_LOW();		
	/*���ͼĴ�����*/
	SPI_NRF_RW(reg); 
	/*��ȡ�Ĵ�����ֵ */
	reg_val = SPI_NRF_RW(0x00);        
	/*CSN���ߣ����*/
	NRF2_CSN_HIGH();		
	return reg_val;
}	

static void delay( int m)	
{ 
	int i,j,k;
	k=2*m;
	for(i=0;i<k;i++)
	{
		for(j=0;j<k;j++);
	}
}
/******************************************************************************
  Function:nrf_transmit_start
  Description:
  Input:	data_buff��	   Ҫ���͵�����
			data_buff_len��Ҫ���͵����鳤��
			nrf_data_type�������������ͣ���Ч����:NRF_DATA_IS_USEFUL
										ACK		:NRF_DATA_IS_ACK
  Output:
  Return:
  Others:ע�⣺ͨ�ŷ�ʽ���ƣ�����ͬһUID�������·����ݣ�ʱ��Ҫ���3S����
******************************************************************************/
int spi_wr_data(char addr, char data)
{		
	int reg_val;							
	char reg_val2;									
	NRF2_CSN_LOW();			
	reg_val=SPI_NRF_RW(addr); 
	reg_val2=SPI_NRF_RW(data); 
	NRF2_CSN_HIGH();
	reg_val=(reg_val<<8)+reg_val2;			
	return reg_val; 
}	

uint8_t si24r2e_read_nvm( uint8_t *pbuf )
{
	int i = 10000;
	int tep;
	uint8_t write_count = 0;
	uint8_t *pdata  = pbuf;
	NRF2_CSN_HIGH();
	/* ��ȡ״̬ */
	tep = SPI_NRF_ReadReg(0x07);
	while(!(0x40&tep) && i--)
	{
		tep=SPI_NRF_ReadReg(0x07); 
		printf("\r\n status: %x \r\n",tep); 
	}
	DEBUG_SI24R2E_LOG("status: %x \r\n",tep);
	delay(25);	

	/* ���� */
	delay(25);
	tep = spi_wr_data(0x5C,0xA7); // ��ֹ����
	delay(25);
  tep = spi_wr_data(0x2C,0x00);
	delay(25);
  tep = spi_wr_data(0x5E,0xA9);
	delay(25);
  tep = spi_wr_data(0x5A,0xA5); // ��ʼ���߼�
	delay(65);
	
	/* ��ȡ���� */
	DEBUG_SI24R2E_LOG("NVM_DATA:\r\n");
	for(i=0;i<64;i++)
	{
		SPI_NRF_ReadReg(0xC0+i);
		tep = SPI_NRF_ReadReg(0x15); 
		//printf("addr :%02d data:%02x \r\n",i,tep);
		switch(i)
		{
			case 0x0E:  /* ���͹��ʺ����� */
			{
				int8_t tx_power = 0;
				uint16_t speed = 0;
				switch(tep & 0x07)
				{
					case 0: tx_power = -12; break;
					case 1: tx_power = -6;  break;
					case 2: tx_power = -4;  break;
					case 3: tx_power = 0;   break;
					case 4: tx_power = 1;   break;
					case 5: tx_power = 3;   break;
					case 6: tx_power = 4;   break;
					case 7: tx_power = 7;   break;
					default:                break;
				}
				DEBUG_SI24R2E_LOG("���͹��ʣ�%d dBm\r\n",tx_power);
				switch((tep & 0x28)>>3)
				{
					case 0: speed = 1000; break;
					case 1: speed = 2000; break;
					case 4: speed = 250;  break;
					default:              break;
				}
				DEBUG_SI24R2E_LOG("�������ʣ�%d kbps \r\n",speed);
			}
			break;

			case 0x01:
				DEBUG_SI24R2E_LOG( "�ŵ���%d\r\n",tep & 0x7F );
			break;

			case 0x2:
				DEBUG_SI24R2E_LOG( "����ģʽ��%s\r\n",(tep & 0x7F)?"��ͨģʽ":"����ģʽ");
			break;

			case 0x07:
				DEBUG_SI24R2E_LOG( "���ݳ��ȣ�%d\r\n",tep & 0x3F );
			break;

			case 0x00:
				DEBUG_SI24R2E_LOG( "CRC״̬��%s\r\n",(tep & 0x40) ? "����":"�ر�");
				DEBUG_SI24R2E_LOG( "CRC���ȣ�%d\r\n",(tep & 0x20) ? 2:1);
			break;

			case 0x0D:
				DEBUG_SI24R2E_LOG( "��̬���أ�%s\r\n",(tep & 0x04) ? "����":"�ر�");
				DEBUG_SI24R2E_LOG( "��ַ���ȣ�%d\r\n",(tep & 0x03) ? (tep & 0x03)+2:0);
			break;

			case 0x08: DEBUG_SI24R2E_LOG( "TX_ADDR��%02X" ,tep);	break;
			case 0x09: DEBUG_SI24R2E_LOG( "%02X",tep);			      break;
			case 0x0A: DEBUG_SI24R2E_LOG( "%02X",tep);						break;
			case 0x0B: DEBUG_SI24R2E_LOG( "%02X",tep);						break;
			case 0x0C: DEBUG_SI24R2E_LOG( "%02X\r\n",tep);						break;

			case 0x14: *pdata = tep; pdata++; DEBUG_SI24R2E_LOG( "TX_DATA��%02X" ,tep);break;
			case 0x15: *pdata = tep; pdata++; DEBUG_SI24R2E_LOG( " %02X" ,tep);	break;
			case 0x16: *pdata = tep; pdata++; DEBUG_SI24R2E_LOG( " %02X" ,tep);	break;
			case 0x17: *pdata = tep; pdata++; DEBUG_SI24R2E_LOG( " %02X" ,tep);	break;
			case 0x18: *pdata = tep; pdata++; DEBUG_SI24R2E_LOG( " %02X" ,tep);	break;
			case 0x19: *pdata = tep; pdata++; DEBUG_SI24R2E_LOG( " %02X" ,tep);	break;
			case 0x1A: *pdata = tep; pdata++; DEBUG_SI24R2E_LOG( " %02X" ,tep);	break;
			case 0x1B: *pdata = tep; pdata++; DEBUG_SI24R2E_LOG( " %02X" ,tep);	break;

			default:
				break;
		}
	}
	delay(25);

	/* ��ȡ��̴��� */
	write_count = spi_wr_data(0x16,0x00);
	DEBUG_SI24R2E_LOG("\r\nProgram Count: %x \r\n",write_count); 

	tep = spi_wr_data(0x5A,0xA5);
	delay(25);
	tep = spi_wr_data(0x2C,0x00);
	delay(25);
	tep = spi_wr_data(0x5E,0xA9);
	delay(25);
	tep = spi_wr_data(0x63,0x00);
	delay(25);
	delay(1000);

	return write_count;
}

int rd_data_before_wr1()
{
	int tep,tep11;
	int i = 10000;
	NRF2_CSN_HIGH();	
	tep=SPI_NRF_ReadReg(0x07); 
	while(!(0x40&tep) && i--)
	{
		tep=SPI_NRF_ReadReg(0x07); 
	} 
	delay(10);
	tep=spi_wr_data(0x5C,0xA7);
	delay(10);
	tep=spi_wr_data(0x2C,0x00);
	delay(10);
	tep=spi_wr_data(0x5E,0xA9);
	delay(10);
	tep=spi_wr_data(0x5A,0xA5);
	delay(10);

	SPI_NRF_ReadReg(0xC0+0x00);
	tep11=SPI_NRF_ReadReg(0x15); 

	delay(10);
	tep=spi_wr_data(0x16,0x00);
	delay(10);
	tep=spi_wr_data(0x5A,0xA5);
	delay(10);
	tep=spi_wr_data(0x2C,0x00);
	delay(10);
	tep=spi_wr_data(0x5E,0xA9);
	delay(10);
	tep=spi_wr_data(0x63,0x00);
	delay(100);
	return tep11;
}

void pre_wr_data()
{
	int tep;
	tep=spi_wr_data(0x5A,0xA5);
	delay(25);
	tep=spi_wr_data(0x5b,0xA6);
	delay(25);
	tep=spi_wr_data(0x80,0xFB);
	delay(65);
	tep=spi_wr_data(0x5B,0xA6);
	delay(25);
	tep=spi_wr_data(0x5A,0xA5);
	delay(25);
	tep=spi_wr_data(0x63,0x00);
	delay(650);
}

void wr_data(char mode,uint8_t *pbuf)
{
	int tep;
	int i = 10000;
	uint8_t *pdata = pbuf;
	
	NRF2_CSN_HIGH();		
	delay(25);

	tep=SPI_NRF_ReadReg(0x07); 

	while(!(0x40&tep) && i--)
	{
		 tep=SPI_NRF_ReadReg(0x07); 
	}
  delay(25);
	tep=spi_wr_data(0x5C,0xA7);
	delay(25);
	tep=spi_wr_data(0x5A,0xA5);
	delay(25);
	tep=spi_wr_data(0x5B,0xA6);
	delay(25);
	 
	for(i=0;i<64;i++)
	{
		if(i>19) 
		{
			txbuf[i]  = *pdata;
			pdata++;
		}	
		if(mode)
		{
		 SPI_NRF_WriteReg((0x80 + (i & 0x3F)),txbuf[i]);
		}
		else SPI_NRF_WriteReg((0x80 + (i & 0x3F)),txbuf[i]);
		delay(65);		
	}
	tep=spi_wr_data(0x5B,0xA6);
	delay(25);
	tep=spi_wr_data(0x5A,0xA5);
	delay(25);
	tep=spi_wr_data(0x63,0x00);
	delay(25);
}

void si24r2e_write_nvm( uint8_t *pwbuf )
{
	int un_lock=0x00;
	int i;

	NRF2_CSN_HIGH();	
	for(i=0;i<10000;i++);
	
	un_lock = rd_data_before_wr1();

	if(un_lock)
	{
		pre_wr_data();
	}

	wr_data(0,pwbuf);

}

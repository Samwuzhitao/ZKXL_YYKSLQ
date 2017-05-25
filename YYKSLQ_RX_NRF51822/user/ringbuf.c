#include "ringbuf.h"

static uint8_t   buf_status = RING_BUF_EMPTY;
static uint8_t   buf_size[BUF_MAX_SIZE];
static uint8_t   *p_write = buf_size;
static uint8_t	 *p_read  = buf_size;
static uint8_t   *p_bottom = buf_size;
static uint8_t   *p_top = &buf_size[BUF_MAX_SIZE];
static uint16_t   use_ringbuf_len = 0;				//��ʹ�õĻ��峤��,�����ж�bufʹ��״̬

/******************************************************************************
  @����:ringbuf_get_status
  @����:����ringbuf״̬
  @����:
  @���:
  @����:�ⲿ
******************************************************************************/
uint8_t ringbuf_get_status(void)
{
	return buf_status;
}

/******************************************************************************
  @����:ringbuf_write_buf
  @����:
  @����:	buf��Ҫ���뻺����������
			buf_len��Ҫ��ȡ�����������鳤��
  @���:
  @����:�ⲿ
******************************************************************************/
void ringbuf_write_buf(uint8_t *buf, uint8_t *buf_len)
{
	uint8_t i;
	
	*p_write = *buf_len;
	p_write = (p_write == p_top) ? p_bottom : ++p_write;	//дָ��ָ����һ��
	
	for(i = 0;i < *buf_len;i++)
	{
		*p_write = buf[i];
		p_write = (p_write == p_top) ? p_bottom : ++p_write;
	}
	
	use_ringbuf_len = use_ringbuf_len + *buf_len + 1;
	
	if((BUF_MAX_SIZE - use_ringbuf_len) < PACK_MAX_SIZE)	//ʣ��Ļ���bufС�ڿ��ܵ��������buf���ȣ�����Ϊ����
		buf_status = RING_BUF_FULL;
	else
		buf_status = RING_BUF_USEING;
}

/******************************************************************************
  @����:ringbuf_read_buf
  @����:
  @����:	buf����ȡ��������Ӧ����
			buf_len����ȡ��������Ӧ����ĳ���
  @���:
  @����:�ⲿ
******************************************************************************/
void ringbuf_read_buf(uint8_t *buf, uint8_t *buf_len)
{
	uint8_t i;
	
	*buf_len = *p_read;
	p_read = (p_read == p_top) ? p_bottom : ++p_read;		//��ָ��ָ����һ��
	
	for(i = 0;i < *buf_len;i++)
	{
		buf[i] = *p_read;
		p_read = (p_read == p_top) ? p_bottom : ++p_read;
	}
	
	use_ringbuf_len = use_ringbuf_len - *buf_len - 1;
	
	if(use_ringbuf_len == 0)
		buf_status = RING_BUF_EMPTY;
	else
		buf_status = RING_BUF_USEING;
}

/******************************************************************************
  @����:ringbuf_test
  @����:
  @����:
  @���:
  @����:�ⲿ
******************************************************************************/
void ringbuf_test(void)
{
	uint8_t i;
	uint8_t write_len = 32;
	uint8_t write_buf[32];
	uint8_t read_len;
	uint8_t read_buf[250];

	while(1)
	{
		for(i = 1;i < 18;i++)
		{
			write_len = i;
			memset(write_buf,write_len,write_len);
			if(ringbuf_get_status() != RING_BUF_FULL)
			{
				ringbuf_write_buf(write_buf,&write_len);
			}
		}

		do
		{
			memset(read_buf,0x00,250);
			ringbuf_read_buf(read_buf,&read_len);
			
			for(i = 0;i < read_len;i++)
				printf("%02X ",read_buf[i]);
			printf("\r\n");
			
		}while((ringbuf_get_status() != RING_BUF_EMPTY));
		
		nrf_delay_ms(1500);
	}
}




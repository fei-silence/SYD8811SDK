#include "uart.h"
#include "gpio.h"
#include "config.h"
#include <string.h>

static UART_CTRL_TYPE * UART_0_CTRL = ((UART_CTRL_TYPE *) UART_0_CTRL_BASE);
static UART_CTRL_TYPE * UART_1_CTRL = ((UART_CTRL_TYPE *) UART_1_CTRL_BASE);

uint8_t tx_len=0,rx_len=0, rx_buf[RX_BUF_MAX];

//-------------------------------------------------------------------------------------------------
void uart_0_init(void)
{
	PIN_CONFIG->PIN_15_SEL = PIN_SEL_UART_RXD0;
	PIN_CONFIG->PIN_16_SEL = PIN_SEL_UART_TXD0; 
	
	
	PIN_CONFIG->PAD_21_INPUT_PULL_UP = 0;
	PIN_CONFIG->PAD_22_INPUT_PULL_UP = 0;

	UART_0_CTRL->CLK_SEL = 0;		/* 1=32M, 0=16M */
	
	UART_0_CTRL->BAUDSEL = UART_BAUD_115200;
	UART_0_CTRL->FLOW_EN = UART_RTS_CTS_DISABLE;
	
	UART_0_CTRL->RX_INT_MASK = 1;	/* 1=MASK */
	UART_0_CTRL->TX_INT_MASK = 1;	/* 1=MASK */
	
	UART_0_CTRL->PAR_FAIL_INT_MASK = 1; /* 1=MASK */
	UART_0_CTRL->par_rx_even = 0;	/* 1=Even, 0=Odd */
	UART_0_CTRL->par_rx_en = 0;		/* 1=Rx Parity check enable */

	UART_0_CTRL->par_tx_even = 0;	/* 1=Even, 0=Odd */
	UART_0_CTRL->par_tx_en = 0;		/* 1=Tx Parity check enable */

	//clr Int Flag
	UART_0_CTRL->RI = 0;
	UART_0_CTRL->TI = 0;
	UART_0_CTRL->PAR_FAIL = 1;

	UART_0_CTRL->RX_FLUSH = 1;		/* clr rx fifo and set RX_FIFO_EMPTY */

	NVIC_EnableIRQ(UART0_IRQn);

	UART_0_CTRL->UART_EN = 1;
}

/****************************************************************************
UART0����д����
����: uint8_t data Ҫ���͵�����
ע��: 
	  ʹ�������������ʹ��Tx�ж�
*****************************************************************************/
void uart_0_write(uint8_t data)
{
	UART_0_CTRL->TX_DATA = data;
	while(UART_0_CTRL->TI == 0);
	UART_0_CTRL->TI = 0;
}

/****************************************************************************
UART0���ڶ�Rx FIFO����
����:
	pcnt - ���ض������ֽ���
	pbuf - ָ�����ݴ�ŵ�buf
ע��: FIFO����Ϊ4��Ҫ�����buf��С����Ϊ4�ֽ�
****************************************************************************/
void uart_0_read(uint8_t *pcnt, uint8_t *pbuf)
{
	uint8_t i = 0;
	volatile uint8_t dly = 0;		//�����64Mʱ�ӣ�����Ҫ��delay
	
	while(!UART_0_CTRL->RX_FIFO_EMPTY)
	{
		*(pbuf+i) = UART_0_CTRL->RX_DATA;
		i++;
		
		//��ʱ400ns����
		dly++;
		dly++;
		dly++;		
	}

	*pcnt = i;
}

void uart_0_close(void)
{
	UART_0_CTRL->UART_EN = 0;
	NVIC_DisableIRQ(UART0_IRQn);
}

/*****************************************************************************
UART0�����жϺ���
ע��:
	  RX_FLUSHֻ����λRX_FIFO_EMPTY,�������RI
*****************************************************************************/
void UART0_IRQHandler(void)
{
	if ((UART_0_CTRL->RI) == 1)
	{
		//�����ݵ�buf[]��
		uint8_t len, buf[4];
		UART_0_CTRL->RI = 0;		//�����־���ٶ�����			
		uart_0_read(&len, buf);
	}

#if 1
	if ((UART_0_CTRL->TI) == 1)
	{
		UART_0_CTRL->TI = 0;
	}
#endif

	if((UART_0_CTRL->PAR_FAIL) == 1)
	{
		UART_0_CTRL->PAR_FAIL = 1;
	}
}

//-------------------------------------------------------------------------------------------------
void uart_1_init(void)
{
	PIN_CONFIG->PIN_0_SEL = PIN_SEL_UART_RXD1;
	PIN_CONFIG->PIN_1_SEL = PIN_SEL_UART_TXD1;	
	
	
	PIN_CONFIG->PIN_5_SEL = PIN_SEL_UART_RXD1;
	PIN_CONFIG->PIN_6_SEL = PIN_SEL_UART_TXD1;
	
	PIN_CONFIG->PAD_5_INPUT_PULL_UP = 0;
	PIN_CONFIG->PAD_6_INPUT_PULL_UP = 0;
	
	

	UART_1_CTRL->CLK_SEL = 0;		/* 1=32M, 0=16M */
	
	UART_1_CTRL->BAUDSEL = UART_BAUD_115200;
	UART_1_CTRL->FLOW_EN = UART_RTS_CTS_DISABLE;
	
	UART_1_CTRL->RX_INT_MASK = 0;	/* 1=MASK */
	UART_1_CTRL->TX_INT_MASK = 1;	/* 1=MASK */
	
	UART_1_CTRL->PAR_FAIL_INT_MASK = 1; /* 1=MASK */
	UART_1_CTRL->par_rx_even = 0;	/* 1=Even, 0=Odd */
	UART_1_CTRL->par_rx_en = 0;		/* 1=Rx Parity check enable */
	
	UART_1_CTRL->par_tx_even = 0;	/* 1=Even, 0=Odd */
	UART_1_CTRL->par_tx_en = 0;		/* 1=Tx Parity check enable */

	//clr Int Flag
	UART_1_CTRL->RI = 0;
	UART_1_CTRL->TI = 0;
	UART_1_CTRL->PAR_FAIL = 1;

	UART_1_CTRL->RX_FLUSH = 1;		/* clr rx fifo and set RX_FIFO_EMPTY */

	NVIC_EnableIRQ(UART1_IRQn);

	UART_1_CTRL->UART_EN = 1;
}

/****************************************************************************
UART1����д����
����: uint8_t data Ҫ���͵�����
ע��: 
	  ʹ�������������ʹ��Tx�ж�
****************************************************************************/
void uart_1_write(uint8_t data)
{
	UART_1_CTRL->TX_DATA = data;
	while(UART_1_CTRL->TI == 0);
	UART_1_CTRL->TI = 0;
}

/****************************************************************************
UART1���ڶ�Rx FIFO����
����:
	pcnt - ���ض������ֽ���
	pbuf - ָ�����ݴ�ŵ�buf
ע��: FIFO����Ϊ4��Ҫ�����buf��С����Ϊ4�ֽ�
****************************************************************************/
void uart_1_read(uint8_t *pcnt, uint8_t *pbuf)
{
	uint8_t i = 0;
	volatile uint8_t dly = 0;		//�����64Mʱ�ӣ�����Ҫ��delay��ʱ400ns����
	
	while(!UART_1_CTRL->RX_FIFO_EMPTY)
	{
		rx_buf[rx_len]=UART_1_CTRL->RX_DATA;
		rx_len++;
		if(rx_len>=RX_BUF_MAX)  rx_len=0;
		//*(pbuf+i) = UART_1_CTRL->RX_DATA;
		i++;
		//
		dly++;
		dly++;
		dly++;
	}

	*pcnt = i;
}
void uart_1_close(void)
{
	UART_1_CTRL->UART_EN = 0;
	NVIC_DisableIRQ(UART1_IRQn);
}

/*****************************************************************************
UART1�����жϺ���
ע��:
	  RX_FLUSHֻ����λRX_FIFO_EMPTY,�������RI
*****************************************************************************/
void UART1_IRQHandler(void)
{
	if ((UART_1_CTRL->RI) == 1)
	{
		//�����ݵ�buf[]��
		uint8_t len, buf[4];
		UART_1_CTRL->RI = 0;		//�����־���ٶ�����		
		uart_1_read(&len, buf);		
	}

#if 1	
	if ((UART_1_CTRL->TI) == 1)
	{
		UART_1_CTRL->TI = 0;
	}
#endif
	if((UART_1_CTRL->PAR_FAIL) == 1)
	{
		UART_1_CTRL->PAR_FAIL = 1;
	}
}

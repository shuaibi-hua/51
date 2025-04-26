/*
 * stc uart
*/

#include "common.h"
#include "uart.h"

/*Define UART parity mode*/
#define NONE_PARITY     0   //None parity
#define ODD_PARITY      1   //Odd parity
#define EVEN_PARITY     2   //Even parity
#define MARK_PARITY     3   //Mark parity
#define SPACE_PARITY    4   //Space parity

#define PARITYBIT NONE_PARITY   //Testing even parity


COMx_Define	COM1;
unsigned char	xdata TX1_Buffer[COM_TX1_Lenth];	//?¤?é?¨a?o3?
unsigned char xdata RX1_Buffer[COM_RX1_Lenth];	//?¨?¨o??o3?

void uart_int(void) interrupt 4 using 1
{
    if(RI)
    {
        RI = 0;
        if(COM1.B_RX_OK == 0)
        {
            if(COM1.RX_Cnt >= COM_RX1_Lenth)	COM1.RX_Cnt = 0;
            RX1_Buffer[COM1.RX_Cnt++] = SBUF;
            COM1.RX_TimeOut = TimeOutSet1;
        }
	
    }

    if(TI)
    {
        TI = 0;
	
        if(COM1.TX_read != COM1.TX_write)
        {
#if (PARITYBIT == ODD_PARITY)
            TB8 = 1;            //Set parity bit to 1
#elif (PARITYBIT == EVEN_PARITY)
            TB8 = 0;            //Set parity bit to 0
#endif
            SBUF = TX1_Buffer[COM1.TX_read];
            if(++COM1.TX_read >= COM_TX1_Lenth)		COM1.TX_read = 0;
        }
        else	COM1.B_TX_busy = 0;
	
    }
}

void uart_init(unsigned int baud)
{
#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;			//8-bit variable UART
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;			//9-bit variable UART, parity bit initial to 1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;			//9-bit variable UART, parity bit initial to 0
#endif

    TL2 = RCAP2L = (65536-(UART_FOSC/32/baud)); //Set auto-reload vaule
    TH2 = RCAP2H = (65536-(UART_FOSC/32/baud)) >> 8;
    T2CON = 0x34;			//Timer2 start run
    ES = 1; 				//Enable UART interrupt
    EA = 1; 				//Open master interrupt switch
}

void uart_write2buff(unsigned char dat)	//写入发送缓冲，指针+1
{
    TX1_Buffer[COM1.TX_write] = dat;	//装发送缓冲
    if(++COM1.TX_write >= COM_TX1_Lenth)	COM1.TX_write = 0;

    if(COM1.B_TX_busy == 0)		//空闲
    {
        COM1.B_TX_busy = 1;		//标志忙
        TI = 1;					//触发发送中断
    }
}

void uart_send(unsigned char *bufs,unsigned char len)
{
    if (len != SERIAL_AUTO_LEN)
    {
        while (len--)
        {
            uart_write2buff(*bufs);
            bufs ++;
        }
    }
    else
    {
        for (; *bufs != 0;	bufs++)  uart_write2buff(*bufs); 	//¨??|ì?¨a?ê?1?¤?0?¨￠¨o?
    }
}

unsigned char uart_recv(unsigned char *bufs, unsigned short timeout)
{
    unsigned char i;

    COM1.RX_TimeOut = timeout;
    
    do
    {
        delay_ms(1);
        if(COM1.RX_TimeOut > 0)		//超时计数
        {
            if(--COM1.RX_TimeOut == 0)
            {
                unsigned char ret;

                ret = COM1.RX_Cnt;
                if(COM1.RX_Cnt > 0)
                {
                    for(i=0; i<COM1.RX_Cnt; i++)
                    {
                        //uart_write2buff(RX1_Buffer[i]);	//收到的数据原样返回
                        *bufs = RX1_Buffer[i];
                        bufs ++;
                    }
                }
                COM1.RX_Cnt = 0;
                return ret;
            }
        }
    }
    while(1);

    return 0;
}


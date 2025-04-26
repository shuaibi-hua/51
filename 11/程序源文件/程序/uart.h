#ifndef __UART_H_
#define __UART_H_


#define	TimeOutSet1		50
#define	TimeOutSet2		50

#define	BRT_Timer1	1
#define	BRT_Timer2	2

typedef struct
{
    unsigned char	TX_read;		//发送读指针
    unsigned char	TX_write;		//发送写指针
    unsigned char	B_TX_busy;		//忙标志

    unsigned char 	RX_Cnt;			//接收字节计数
    unsigned short	RX_TimeOut;		//接收超时
    unsigned char	B_RX_OK;		//接收块完成
} COMx_Define;

#define	COM_TX1_Lenth	48
#define	COM_RX1_Lenth	48

#define SERIAL_AUTO_LEN 					0xFF

#define UART_FOSC MAIN_Fosc      //System frequency

#define UART_B115200						115200
#define UART_B57600							57600
#define UART_B38400							38400
#define UART_B9600							9600

void uart_init(unsigned int baud);
void uart_write2buff(unsigned char dat);	//写入发送缓冲，指针+1
void uart_send(unsigned char *bufs,unsigned char  len);
unsigned char uart_recv(unsigned char *bufs,unsigned short timeout);
#endif /*__UART_H_*/

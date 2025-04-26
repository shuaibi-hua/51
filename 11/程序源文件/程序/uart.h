#ifndef __UART_H_
#define __UART_H_


#define	TimeOutSet1		50
#define	TimeOutSet2		50

#define	BRT_Timer1	1
#define	BRT_Timer2	2

typedef struct
{
    unsigned char	TX_read;		//���Ͷ�ָ��
    unsigned char	TX_write;		//����дָ��
    unsigned char	B_TX_busy;		//æ��־

    unsigned char 	RX_Cnt;			//�����ֽڼ���
    unsigned short	RX_TimeOut;		//���ճ�ʱ
    unsigned char	B_RX_OK;		//���տ����
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
void uart_write2buff(unsigned char dat);	//д�뷢�ͻ��壬ָ��+1
void uart_send(unsigned char *bufs,unsigned char  len);
unsigned char uart_recv(unsigned char *bufs,unsigned short timeout);
#endif /*__UART_H_*/

#include <reg52.h>
#include <intrins.h>
#include "lcd1602.h"
#include "common.h" 

sbit RS = P2^5;//������������
sbit RW = P2^6;//��д����
sbit E = P2^7;//ʹ������

#define DataPort P0//1602����ͨ�ſ�

void lcd_clear(void)
{
     lcd_write_com(0x01);
	 delay_ms(5);
}

void lcd_write_com(unsigned char byte)
{
		RS = 0;
	  RW = 0;
	  E = 1;
	  DataPort = byte;
	  _nop_();
	  _nop_();
	  delay_ms(1);
	  E = 0;   
}

void lcd_write_data(unsigned char byte)
{
	  RS = 1;
	  RW = 0;
	  E = 1;
	  DataPort = byte;
	  _nop_();
	  _nop_();
	  delay_ms(1);
	  E = 0;
}

void lcd_init()
{
	  delay_ms(5);
	  lcd_write_com(0x38);//����16*2��ʾ��5*7����8λ���ݽӿ�
	  delay_ms(5);
	  lcd_write_com(0x38);//����16*2��ʾ��5*7����8λ���ݽӿ�
	  delay_ms(5);
	  lcd_write_com(0x38);//����16*2��ʾ��5*7����8λ���ݽӿ�
	  delay_ms(5);
	  lcd_write_com(0x08); //�ر���ʾ
	  delay_ms(5);
	  lcd_write_com(0x01);//����ָ��
	  delay_ms(5);
	  lcd_write_com(0x06);
	  delay_ms(5);
	  lcd_write_com(0x0c);
	  delay_ms(5);
	  
}
void lcd_write_char(unsigned char x,unsigned char y,unsigned char byte)//��ָ��λ����ʾ�ַ�
{
	  if(0 == y)
		{
			lcd_write_com(0x80 + x);//��һ��
		}
	  else if(1 == y)
		{
			lcd_write_com(0x80 + 0x40 + x);//�ڶ���
		}
		lcd_write_data(byte);
}

void lcd_write_str(unsigned char x, unsigned char y, unsigned char *s)//��ָ��λ����ʾ�ַ���
{
	 if(0 == y)
		{
			lcd_write_com(0x80 + x);//��һ��
		}
	  else
		{
			lcd_write_com(0x80 + 0x40 + x);//�ڶ���
		}
		
		while(*s)//�ж��Ƿ��⵽������ 
		{
			lcd_write_data(*s);//��ʾ����
			s++;//ָ���1 
		}
		
}

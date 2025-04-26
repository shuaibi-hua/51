#include <reg52.h>
#include <intrins.h>
#include "lcd1602.h"
#include "common.h" 

sbit RS = P2^5;//数据命令引脚
sbit RW = P2^6;//读写引脚
sbit E = P2^7;//使能引脚

#define DataPort P0//1602并行通信口

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
	  lcd_write_com(0x38);//设置16*2显示，5*7点阵，8位数据接口
	  delay_ms(5);
	  lcd_write_com(0x38);//设置16*2显示，5*7点阵，8位数据接口
	  delay_ms(5);
	  lcd_write_com(0x38);//设置16*2显示，5*7点阵，8位数据接口
	  delay_ms(5);
	  lcd_write_com(0x08); //关闭显示
	  delay_ms(5);
	  lcd_write_com(0x01);//清屏指令
	  delay_ms(5);
	  lcd_write_com(0x06);
	  delay_ms(5);
	  lcd_write_com(0x0c);
	  delay_ms(5);
	  
}
void lcd_write_char(unsigned char x,unsigned char y,unsigned char byte)//在指定位置显示字符
{
	  if(0 == y)
		{
			lcd_write_com(0x80 + x);//第一行
		}
	  else if(1 == y)
		{
			lcd_write_com(0x80 + 0x40 + x);//第二行
		}
		lcd_write_data(byte);
}

void lcd_write_str(unsigned char x, unsigned char y, unsigned char *s)//在指定位置显示字符串
{
	 if(0 == y)
		{
			lcd_write_com(0x80 + x);//第一行
		}
	  else
		{
			lcd_write_com(0x80 + 0x40 + x);//第二行
		}
		
		while(*s)//判断是否检测到结束符 
		{
			lcd_write_data(*s);//显示数据
			s++;//指针加1 
		}
		
}

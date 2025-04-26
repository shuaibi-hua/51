#include "common.h"
#include "DS18B20_One.h"
#include "lcd1602.h"
#include "eeprom52.h"
 
sbit key1 = P1^5;
sbit key2 = P1^4;
sbit key3 = P1^3;

sbit beep = P2^0;

sbit jdq = P3^7;

sbit led1 = P1^0;
sbit led2 = P1^1;

unsigned char temp_hight = 35,temp_low = 10,setn=0;
bit flag = 0;

char check = 111;//检验位

void write_eeprom()//eeprom存储数据，设置值
{
	  SectorErase(0x2000);//清除

		byte_write(0x2001, temp_low);
		byte_write(0x2002, temp_hight);
	  byte_write(0x2009, check);
}

void eeprom_init()//eeprom读取数据
{
	  check = byte_read(0x2009);
		if(check != 111)//单片机第一次使用时，check校验值便不是111，所以这里要把初始的数据先存一遍再读取
		{
				check = 111;
			  temp_hight = 35;
			  temp_low = 10;
			  write_eeprom();
		}
	  temp_low = byte_read(0x2001);//读取下限值
	  temp_hight = byte_read(0x2002);//读取上限值
}

void keyscan()
{
 	if(key1 == 0)//加键
	{
	   delay_ms(10);//消抖
	   if(key1 == 0)
	   {
	   	  while(key1 == 0);//等待按键松开
			  if(setn == 1)//设置温度下限
				{
						if(temp_hight - temp_low > 1)
						{
							 temp_low++;	
						}	
						lcd_write_char(4,1,temp_low/10+48);
						lcd_write_char(5,1,temp_low%10+48);
						
						lcd_write_com(0x80+0x40+5);
					  lcd_write_com(0x0f);
			  }
				if(setn == 2)//设置温度上限
				{
						if(temp_hight < 99)//最大到99
						{
							 temp_hight++;	
						}	
						lcd_write_char(13,1,temp_hight/10+48);
						lcd_write_char(14,1,temp_hight%10+48);
						
						lcd_write_com(0x80+0x40+14);
					  lcd_write_com(0x0f);
			  }
				write_eeprom();//存储
	   }	
	}
	if(key2 == 0)//减键
	{
	   delay_ms(10);
	   if(key2 == 0)
	   {
	   	  while(key2 == 0);
				if(setn == 1)
				{
						if(temp_low > 0)
						{
							 temp_low--;	
						}	
						lcd_write_char(4,1,temp_low/10+48);
						lcd_write_char(5,1,temp_low%10+48);
						
						lcd_write_com(0x80+0x40+5);
					  lcd_write_com(0x0f);
			  }
				if(setn == 2)
				{
						if(temp_hight - temp_low > 1)
						{
							 temp_hight--;	
						}	
						lcd_write_char(13,1,temp_hight/10+48);
						lcd_write_char(14,1,temp_hight%10+48);
						
						lcd_write_com(0x80+0x40+14);
					  lcd_write_com(0x0f);
			  }
				write_eeprom();
	   }	
	}
	if(key3 == 0)//设置键
	{
	   delay_ms(10);
	   if(key3 == 0)
	   {
	   	  while(key3 == 0);
			  beep = 1;
        setn++;
			  if(setn > 2)
				{
						setn = 0;
					  lcd_write_com(0x0c);//关闭光标
				}
				if(setn == 1)
				{
						lcd_write_com(0x80+0x40+5);
					  lcd_write_com(0x0f);
				}
				if(setn == 2)
				{
						lcd_write_com(0x80+0x40+14);
					  lcd_write_com(0x0f);
				}
	   }	
	}
}

void main()
{
    short temperature;//温度
    unsigned int count;

	  temperature = DS18B20_TF();
	  delay_ms(1000);
	  eeprom_init();
    lcd_init();//1602液晶初始化
		lcd_write_str(0,0,"Temperature:   C");
		lcd_write_str(0,1,"T-L:00C  T-H:00C");

	  lcd_write_char(4,1,temp_low/10+48);//显示十位 
    lcd_write_char(5,1,temp_low%10+48);//显示个位
	
    lcd_write_char(13,1,temp_hight/10+48);
    lcd_write_char(14,1,temp_hight%10+48);
	  delay_ms(1000);
		while(1)
		{
			 keyscan();
			 if(count++ > 200 && setn ==0)
			 {
					count = 0;

					temperature = DS18B20_TF();//读取温度
					lcd_write_char(13,0,temperature/10+48);
					lcd_write_char(14,0,temperature%10+48);
           
				  if(temperature <= temp_low){led1=0;jdq=0;}else{led1=1;}
				  if(temperature >= temp_hight){led2=0;jdq=1;}else{led2=1;}
				 
						if(temperature >= temp_hight || temperature <= temp_low)
						{
								beep = ~beep;//蜂鸣器报警
							  delay_ms(10);
						}
						else
						{
								beep = 1;//蜂鸣器关闭
						}
			 }
			 delay_ms(1);
		}
}
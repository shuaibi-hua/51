#include <reg51.h>
#include <intrins.h>
#include <math.h>     
#include <stdio.h>     
 
#define uchar unsigned char		// 以后unsigned char就可以用uchar代替
#define uint  unsigned int		// 以后unsigned int 就可以用uint 代替
 
 
//LCD定义 
#define LCD_DB P0 
sbit LCD_RS=P2^0;   
sbit LCD_RW=P2^1; //RS和RW相互自组合控制指令和数据的寄存 
sbit LCD_E=P2^2;  //E是使能脚，需要先高电平后低电平形成下降沿，送入数据和命令，类似锁存

sbit FMQ=P3^6;    //声光报警

sbit LED1=P3^7;   //升温指示灯

sbit K1=P1^5;   
sbit K2=P1^6;  
sbit K3=P1^7;  
sbit K4=P1^4;  
bit flag=1;    //按键松开

sbit JR=P2^3;  //加热
uint i=0;
sbit DQ = P1^1;	// 温度传感器的引脚 
uint Time50ms=0;
uint num0,num1;			
uint wendu=0;
uint H=35;
uint L=20;
/*-------------------------------------
			   函数申明
---------------------------------------*/
void LCD_init(void);                          //初始化函数 
void LCD_write_command(uchar command);        //写指令函数 
void LCD_write_data(uchar dat);               //写数据函数  
void LCD_disp_char(uchar x,uchar y,uchar dat);//在某个屏幕位置上显示一个字符,X（0-15),y(1-2)  
void LCD_disp_str(uchar x,uchar y,uchar *str); //LCD1602显示字符串函数 
void delay_n10us(uint n);                     //延时函数
/*-------------------------------------
			   LCD1602子程序
---------------------------------------*/
void LCD_init(void) //初始化函数
{  
	delay_n10us(10);  
	LCD_write_command(0x38);//0x38设置16*2显示，5*7点阵和8位数据接口
	delay_n10us(10);  LCD_write_command(0x0c);//0x0c开显示，不显示光标 
	delay_n10us(10);  LCD_write_command(0x06);//0x06写一个字符后地址指针加1 
	delay_n10us(10);  LCD_write_command(0x01);//0x01显示清0，数据指针清0
	delay_n10us(100);//延时清屏，延时函数，延时约n个10us 
}

void LCD_write_command(uchar dat) //写指令函数
{  
	delay_n10us(10);  
	LCD_RS=0;//指令 
	LCD_RW=0;//写入
	LCD_E=1;//允许 
	LCD_DB=dat;  delay_n10us(10);//实践证明，我的LCD1602上，用for循环1次就能完成普通写指令。 
	LCD_E=0;  delay_n10us(10);//实践证明，我的LCD1602上，用for循环1次就能完成普通写指令。 
}

void LCD_write_data(uchar dat) //写数据函数
{  
	delay_n10us(10);  
	LCD_RS=1;//数据 
	LCD_RW=0;//写入
	LCD_E=1;//允许 
	LCD_DB=dat; delay_n10us(10);
	 LCD_E=0;  delay_n10us(10); 
}

void LCD_disp_char(uchar x,uchar y,uchar dat) //在某个位置上显示一个字符0-15 1-2
{    
	uchar address;   
	if(y==1)           
	address=0x80+x;//0x80指LCD第一行的起始地址
	else           
	address=0xc0+x;    
	LCD_write_command(address);//指令函数
	LCD_write_data(dat);//数据函数
}

void LCD_disp_str(uchar x,uchar y,uchar *str) 
{    
	uchar address;   
	if(y==1)           
	address=0x80+x;   
	else           
	address=0xc0+x;    
	LCD_write_command(address);   
	while(*str!='\0')   
		{       
		LCD_write_data(*str);
		str++;
		}
}

void delay_n10us(uint n)//延时 
{                 
	uint i;                    
	for(i=n;i>0;i--)            
	 {         
	 nop_();_nop_();_nop_();_nop_();_nop_();_nop_();    
	 } 
}




/*********************************************************/
// 延时15微秒
/*********************************************************/
void Delay15us(void)
{
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	_nop_();
}


/*********************************************************/
// 复位DS18B20（初始化）
/*********************************************************/
void DS18B20_ReSet(void)
{
	uchar i;
	DQ=0;
	i=240;
	while(--i);
	DQ=1;
	i=30;
	while(--i);
	while(~DQ);
	i=4;
	while(--i);
}


/*********************************************************/
// 向DS18B20写入一个字节
/*********************************************************/
void DS18B20_WriteByte(uchar dat)
{
	uchar j;
	uchar btmp;
	
	for(j=0;j<8;j++)
	{
		btmp=0x01;
		btmp=btmp<<j;
		btmp=btmp&dat;
		
		if(btmp>0)		// 写1
		{
			DQ=0;
			Delay15us();
			DQ=1;
			Delay15us();
			Delay15us();
			Delay15us();
			Delay15us();
		}
		else			// 写0
		{
			DQ=0;
			Delay15us();
			Delay15us();
			Delay15us();
			Delay15us();
			DQ=1;
			Delay15us();
		}
	}
}


/*********************************************************/
// 读取温度值
/*********************************************************/
int DS18B20_ReadTemp(void)
{
	uchar j;
	int b,temp=0;	

	DS18B20_ReSet();							// 产生复位脉
	DS18B20_WriteByte(0xcc);			// 忽略ROM指令
	DS18B20_WriteByte(0x44);			// 启动温度转换指令

	DS18B20_ReSet();							// 产生复位脉
	DS18B20_WriteByte(0xcc);			// 忽略ROM指令
	DS18B20_WriteByte(0xbe);			// 读取温度指令

	for(j=0;j<16;j++)							// 读取温度数量
	{						
		DQ=0;
		_nop_();
		_nop_();
		DQ=1;	
		Delay15us();
		b=DQ;
		Delay15us();
		Delay15us();
		Delay15us();
		b=b<<j;
		temp=temp|b;
	}
	
	temp=temp*0.625;							// 合成温度值并放大10倍					
	
	return (temp);								// 返回检测到的温度值
}






void LCD()
{
	LCD_disp_char(8,1,wendu/100+'0'); //显示 
	LCD_disp_char(9,1,wendu/10%10+'0'); //显示 
	LCD_disp_char(10,1,wendu%10+'0'); //显示 

	
	LCD_disp_char(2,2,H/100+'0'); //显示 
	LCD_disp_char(3,2,H/10%10+'0'); //显示 
	LCD_disp_char(4,2,H%10+'0'); //显示 
	
	LCD_disp_char(12,2,L/100+'0'); //显示 
	LCD_disp_char(13,2,L/10%10+'0'); //显示 
	LCD_disp_char(14,2,L%10+'0'); //显示 
}



void delay_ms(unsigned int ms)// 延时子程序
{   
  unsigned int a,b;
  for(a=ms;a>0;a--)
  for(b=123;b>0;b--);
}

void key()
{
	if(K1==0)
	{
		if(flag==1)
		{
			flag=0;
			if(H<128)
			{
				H++;
			}
		}
	}
	else if(K2==0)
	{
		if(flag==1)
		{
			flag=0;
			if(H>L+1)
			{
				H--;
			}
		}
	}
	else if(K3==0)
	{
		if(flag==1)
		{
			flag=0;
			if(L<H-1)
			{
				L++;
			}
		}
	}
	else if(K4==0)
	{
		if(flag==1)
		{
			flag=0;
			if(L>0)
			{
				L--;
			}
		}
	}
	else
	{
		flag=1;
	}
}
/********************************************************
主函数
********************************************************/
void main(void)
{
	LCD_init();//LCD初始化              
	LCD_disp_str(0,1,"   Temp:000c    "); //显示 
	LCD_disp_str(0,2,"H:000c    L:000c"); //显示 
	
	while(1)
	{
		key();
		wendu=DS18B20_ReadTemp()/10;	// 读取温度值 
		
		LCD();
		
		
 
		
		if(wendu<L)
		{
			JR=0;
		}
		if(wendu>H)
		{
			JR=1; 
		}
		
		if(wendu<L||wendu>H)
			FMQ=0;  
		else
			FMQ=1;

		
		
		LED1=JR;
		
		delay_ms(200);//延时子程序
	}
}





#include <reg51.h>
#include <intrins.h>
#include <math.h>     
#include <stdio.h>     
 
#define uchar unsigned char		// �Ժ�unsigned char�Ϳ�����uchar����
#define uint  unsigned int		// �Ժ�unsigned int �Ϳ�����uint ����
 
 
//LCD���� 
#define LCD_DB P0 
sbit LCD_RS=P2^0;   
sbit LCD_RW=P2^1; //RS��RW�໥����Ͽ���ָ������ݵļĴ� 
sbit LCD_E=P2^2;  //E��ʹ�ܽţ���Ҫ�ȸߵ�ƽ��͵�ƽ�γ��½��أ��������ݺ������������

sbit FMQ=P3^6;    //���ⱨ��

sbit LED1=P3^7;   //����ָʾ��

sbit K1=P1^5;   
sbit K2=P1^6;  
sbit K3=P1^7;  
sbit K4=P1^4;  
bit flag=1;    //�����ɿ�

sbit JR=P2^3;  //����
uint i=0;
sbit DQ = P1^1;	// �¶ȴ����������� 
uint Time50ms=0;
uint num0,num1;			
uint wendu=0;
uint H=35;
uint L=20;
/*-------------------------------------
			   ��������
---------------------------------------*/
void LCD_init(void);                          //��ʼ������ 
void LCD_write_command(uchar command);        //дָ��� 
void LCD_write_data(uchar dat);               //д���ݺ���  
void LCD_disp_char(uchar x,uchar y,uchar dat);//��ĳ����Ļλ������ʾһ���ַ�,X��0-15),y(1-2)  
void LCD_disp_str(uchar x,uchar y,uchar *str); //LCD1602��ʾ�ַ������� 
void delay_n10us(uint n);                     //��ʱ����
/*-------------------------------------
			   LCD1602�ӳ���
---------------------------------------*/
void LCD_init(void) //��ʼ������
{  
	delay_n10us(10);  
	LCD_write_command(0x38);//0x38����16*2��ʾ��5*7�����8λ���ݽӿ�
	delay_n10us(10);  LCD_write_command(0x0c);//0x0c����ʾ������ʾ��� 
	delay_n10us(10);  LCD_write_command(0x06);//0x06дһ���ַ����ַָ���1 
	delay_n10us(10);  LCD_write_command(0x01);//0x01��ʾ��0������ָ����0
	delay_n10us(100);//��ʱ��������ʱ��������ʱԼn��10us 
}

void LCD_write_command(uchar dat) //дָ���
{  
	delay_n10us(10);  
	LCD_RS=0;//ָ�� 
	LCD_RW=0;//д��
	LCD_E=1;//���� 
	LCD_DB=dat;  delay_n10us(10);//ʵ��֤�����ҵ�LCD1602�ϣ���forѭ��1�ξ��������ͨдָ� 
	LCD_E=0;  delay_n10us(10);//ʵ��֤�����ҵ�LCD1602�ϣ���forѭ��1�ξ��������ͨдָ� 
}

void LCD_write_data(uchar dat) //д���ݺ���
{  
	delay_n10us(10);  
	LCD_RS=1;//���� 
	LCD_RW=0;//д��
	LCD_E=1;//���� 
	LCD_DB=dat; delay_n10us(10);
	 LCD_E=0;  delay_n10us(10); 
}

void LCD_disp_char(uchar x,uchar y,uchar dat) //��ĳ��λ������ʾһ���ַ�0-15 1-2
{    
	uchar address;   
	if(y==1)           
	address=0x80+x;//0x80ָLCD��һ�е���ʼ��ַ
	else           
	address=0xc0+x;    
	LCD_write_command(address);//ָ���
	LCD_write_data(dat);//���ݺ���
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

void delay_n10us(uint n)//��ʱ 
{                 
	uint i;                    
	for(i=n;i>0;i--)            
	 {         
	 nop_();_nop_();_nop_();_nop_();_nop_();_nop_();    
	 } 
}




/*********************************************************/
// ��ʱ15΢��
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
// ��λDS18B20����ʼ����
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
// ��DS18B20д��һ���ֽ�
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
		
		if(btmp>0)		// д1
		{
			DQ=0;
			Delay15us();
			DQ=1;
			Delay15us();
			Delay15us();
			Delay15us();
			Delay15us();
		}
		else			// д0
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
// ��ȡ�¶�ֵ
/*********************************************************/
int DS18B20_ReadTemp(void)
{
	uchar j;
	int b,temp=0;	

	DS18B20_ReSet();							// ������λ��
	DS18B20_WriteByte(0xcc);			// ����ROMָ��
	DS18B20_WriteByte(0x44);			// �����¶�ת��ָ��

	DS18B20_ReSet();							// ������λ��
	DS18B20_WriteByte(0xcc);			// ����ROMָ��
	DS18B20_WriteByte(0xbe);			// ��ȡ�¶�ָ��

	for(j=0;j<16;j++)							// ��ȡ�¶�����
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
	
	temp=temp*0.625;							// �ϳ��¶�ֵ���Ŵ�10��					
	
	return (temp);								// ���ؼ�⵽���¶�ֵ
}






void LCD()
{
	LCD_disp_char(8,1,wendu/100+'0'); //��ʾ 
	LCD_disp_char(9,1,wendu/10%10+'0'); //��ʾ 
	LCD_disp_char(10,1,wendu%10+'0'); //��ʾ 

	
	LCD_disp_char(2,2,H/100+'0'); //��ʾ 
	LCD_disp_char(3,2,H/10%10+'0'); //��ʾ 
	LCD_disp_char(4,2,H%10+'0'); //��ʾ 
	
	LCD_disp_char(12,2,L/100+'0'); //��ʾ 
	LCD_disp_char(13,2,L/10%10+'0'); //��ʾ 
	LCD_disp_char(14,2,L%10+'0'); //��ʾ 
}



void delay_ms(unsigned int ms)// ��ʱ�ӳ���
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
������
********************************************************/
void main(void)
{
	LCD_init();//LCD��ʼ��              
	LCD_disp_str(0,1,"   Temp:000c    "); //��ʾ 
	LCD_disp_str(0,2,"H:000c    L:000c"); //��ʾ 
	
	while(1)
	{
		key();
		wendu=DS18B20_ReadTemp()/10;	// ��ȡ�¶�ֵ 
		
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
		
		delay_ms(200);//��ʱ�ӳ���
	}
}





#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "usart2.h"
#include "timer.h"
#include "led.h" 		 	 
#include "lcd.h"  
#include "key.h"     
#include "usmart.h" 
#include "malloc.h"
#include "sdio_sdcard.h"  
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"   
#include "text.h"
#include "dht11.h"
#include "touch.h"
#include "24cxx.h"
#include "stmflash.h"
//控制主控终端参数1-8
u8 TED=8,ED=0,tempctr=0;
u8 EDtemp[]={0},temp[8]={0};
//设备确认请求相关
u8 CTR1=0,CTR2=0,UCTR=0,LINK=0,LINKtemp[8]={0};
//菜单功能相关
u8 menu=0,RST=0;
int chang1=240,chang2=470,kuan1=225,kuan2=575;
int shebei=110;

#define SIZE sizeof(EDtemp)		//数组长度
#define FLASH_SAVE_ADDR  0X08070000		//设置FLASH 保存地址(必须为偶数，且其值要大于本代码所占用FLASH的大小+0X08000000)

/************************************************
 ALIENTEK精英STM32开发板实验37
 汉字显示 实验 
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
void Init(void)
{
	u8 i;
	u8 str[17]="一二三四五六七八";  //显示设备号
	
	delay_init();	    	 //延时函数初始化	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	usart_init(115200);	 	//串口初始化为115200
	usart2_init(115200);	 	//串口初始化为115200
 	usmart_dev.init(72);		//初始化USMART		
 	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD   
	tp_dev.init();
	W25QXX_Init();				//初始化W25Q128
 	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//为fatfs相关变量申请内存  
 	f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	f_mount(fs[1],"1:",1); 		//挂载FLASH.
	font_init();
	SD_Init();
	
	TIM3_Int_Init(ED*10000-1,7199);//10Khz的计数频率，计数到5000为500ms 
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)EDtemp,SIZE);
	if(EDtemp[0]==0)
		ED=TED; 
	else
		ED=EDtemp[0];
	for(i=0;i<=TED;i++)
		temp[i]=0;
	Show_Str(132,40,250,24,"化工仪表网监控系统",24,0,2); 	
	for(i=0;i<ED;i++)
	{
		Show_Str(30,shebei+80*i,200,16,"设备",16,0,1);		
		Show_Str(62,shebei+80*i,20,16,&str[i*2],16,0,1);
    Show_Str(115,shebei+80*i,250,16,"WAITING",16,0,2);		
		Show_Str(30,shebei+20+80*i,200,16,"温度：",16,0,1);Show_Str(115,shebei+20+80*i,200,16,"C",16,0,2);
	  Show_Str(30,shebei+40+80*i,200,16,"湿度：",16,0,1);Show_Str(115,shebei+40+80*i,200,16,"%",16,0,2);
	}
	Show_Str(434,770,250,24,"RST",24,0,2); 
	Show_Str(10,770,250,24,"菜单",24,0,2); 
	delay_ms(100);
	u2_printf("%d\r\n", 2);//向串口2发送数据
}
  
void CAIDAN(u8 m)
{
	if(m==0)
	{
		LCD_Fill(10,770,60,800,WHITE); 
		Show_Str(10,770,250,24,"菜单",24,0,2); 
		LCD_Fill(chang1-5,kuan1-5,chang2+5,kuan2+5,WHITE);
	}
	if(m==1)
	{
		LCD_Fill(10,770,60,800,WHITE); 
		Show_Str(10,770,250,24,"返回",24,0,2); 
		LCD_Fill(chang1+5,kuan1+5,chang2-5,kuan2-5,WHITE);
		lcd_draw_bline(chang1,kuan1,chang1,kuan2,1,RED);
		lcd_draw_bline(chang2,kuan1,chang2,kuan2,1,RED);
		lcd_draw_bline(chang1,kuan1,chang2,kuan1,1,RED);
		lcd_draw_bline(chang1,kuan2,chang2,kuan2,1,RED);
		Show_Str(chang1+10,kuan1+10,250,24,"1.Zigbee重组网",24,0,1);
		Show_Str(chang1+10,kuan1+60,250,24,"2.获取终端连接信息",24,0,1);
		Show_Str(chang1+10,kuan1+110,250,24,"3.输入终端数量",24,0,1);
	}
	if(m==2)
	{
		LCD_Fill(chang1+5,kuan1+5,chang2-5,kuan2-5,WHITE);
		LCD_ShowNum(chang1+20,kuan1+20,1,2,24);	    //数字1
		LCD_ShowNum(chang1+20+70,kuan1+20,2,2,24);	//数字2
		LCD_ShowNum(chang1+20+140,kuan1+20,3,2,24);	//数字3
		LCD_ShowNum(chang1+20,kuan1+110,4,2,24);	  //数字4
		LCD_ShowNum(chang1+20+70,kuan1+110,5,2,24);	//数字5
		LCD_ShowNum(chang1+20+140,kuan1+110,6,2,24);//数字6	
		LCD_ShowNum(chang1+20+35,kuan1+200,7,2,24);	//数字7
		LCD_ShowNum(chang1+20+105,kuan1+200,8,2,24);//数字8	
		Show_Str(chang1+10,kuan1+250,250,24,"终端数(1-8)：",24,0,1);
		Show_Str(chang1+10,kuan1+280,250,16,"注:须大于等于实际连接数",16,0,1);
		Show_Str(chang1+10,kuan1+310,250,24,"确定",24,0,1);
	}
}
////////////////////////////////////////////////////////////////////////////////
 //5个触控点的颜色												 
const u16 POINT_COLOR_TBL[CT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED};  
//电容触摸屏测试函数
void EDs(u8 t)
{
	//数字1
	if(tp_dev.x[t]>(chang1)&&tp_dev.x[t]<(chang1+52)
	&&tp_dev.y[t]>(kuan1)&&tp_dev.y[t]<(kuan1+64)&&menu==2)  //输入数量
	{
		while(tp_dev.x[t]>(chang1)&&tp_dev.x[t]<(chang1+64)
		&&tp_dev.y[t]>(kuan1)&&tp_dev.y[t]<(kuan1+52))
		{
			tp_dev.scan(0);
		}
		EDtemp[0]=1;
		LCD_Fill(chang1+150,kuan1+250,chang1+150+13,kuan1+250+13,WHITE);
		LCD_ShowNum(chang1+150,kuan1+250,1,2,24);
	}
	//数字2
	if(tp_dev.x[t]>(chang1+20+50)&&tp_dev.x[t]<(chang1+20+90)
	&&tp_dev.y[t]>(kuan1)&&tp_dev.y[t]<(kuan1+52)&&menu==2)  //输入数量
	{
		while(tp_dev.x[t]>(chang1+20+50)&&tp_dev.x[t]<(chang1+20+90)
	  &&tp_dev.y[t]>(kuan1)&&tp_dev.y[t]<(kuan1+52))
		{
			tp_dev.scan(0);
		}
		EDtemp[0]=2;
		LCD_Fill(chang1+150,kuan1+250,chang1+150+13,kuan1+250+13,WHITE);
		LCD_ShowNum(chang1+150,kuan1+250,2,2,24);
	}
	//数字3
	if(tp_dev.x[t]>(chang1+20+100)&&tp_dev.x[t]<(chang1+20+180)
	&&tp_dev.y[t]>(kuan1)&&tp_dev.y[t]<(kuan1+52)&&menu==2)  //输入数量
	{
		while(tp_dev.x[t]>(chang1)&&tp_dev.x[t]<(chang1+40)
		&&tp_dev.y[t]>(kuan1)&&tp_dev.y[t]<(kuan1+40))
		{
			tp_dev.scan(0);
		}
		EDtemp[0]=3;
		LCD_Fill(chang1+150,kuan1+250,chang1+150+13,kuan1+250+13,WHITE);
		LCD_ShowNum(chang1+150,kuan1+250,3,2,24);
	}
	//数字4
	if(tp_dev.x[t]>(chang1)&&tp_dev.x[t]<(chang1+52)
	&&tp_dev.y[t]>(kuan1+90)&&tp_dev.y[t]<(kuan1+154)&&menu==2)  //输入数量
	{
		while(tp_dev.x[t]>(chang1)&&tp_dev.x[t]<(chang1+64)
		&&tp_dev.y[t]>(kuan1+90)&&tp_dev.y[t]<(kuan1+154))
		{
			tp_dev.scan(0);
		}
		EDtemp[0]=4;
		LCD_Fill(chang1+150,kuan1+250,chang1+150+13,kuan1+250+13,WHITE);
		LCD_ShowNum(chang1+150,kuan1+250,4,2,24);
	}
	//数字5
	if(tp_dev.x[t]>(chang1+20+50)&&tp_dev.x[t]<(chang1+20+90)
	&&tp_dev.y[t]>(kuan1+90)&&tp_dev.y[t]<(kuan1+154)&&menu==2)  //输入数量
	{
		while(tp_dev.x[t]>(chang1+20+50)&&tp_dev.x[t]<(chang1+20+90)
	  &&tp_dev.y[t]>(kuan1+90)&&tp_dev.y[t]<(kuan1+154))
		{
			tp_dev.scan(0);
		}
		EDtemp[0]=5;
		LCD_Fill(chang1+150,kuan1+250,chang1+150+13,kuan1+250+13,WHITE);
		LCD_ShowNum(chang1+150,kuan1+250,5,2,24);
	}
	//数字6
	if(tp_dev.x[t]>(chang1+20+100)&&tp_dev.x[t]<(chang1+20+180)
	&&tp_dev.y[t]>(kuan1+90)&&tp_dev.y[t]<(kuan1+154)&&menu==2)  //输入数量
	{
		while(tp_dev.x[t]>(chang1)&&tp_dev.x[t]<(chang1+40)
		&&tp_dev.y[t]>(kuan1)&&tp_dev.y[t]<(kuan1+40))
		{
			tp_dev.scan(0);
		}
		EDtemp[0]=6;
		LCD_Fill(chang1+150,kuan1+250,chang1+150+13,kuan1+250+13,WHITE);
		LCD_ShowNum(chang1+150,kuan1+250,6,2,24);
	}
	//数字7
	if(tp_dev.x[t]>(chang1+35)&&tp_dev.x[t]<(chang1+20+67)
	&&tp_dev.y[t]>(kuan1+180)&&tp_dev.y[t]<(kuan1+244)&&menu==2)  //输入数量
	{
		while(tp_dev.x[t]>(chang1+35)&&tp_dev.x[t]<(chang1+20+67)
		&&tp_dev.y[t]>(kuan1+180)&&tp_dev.y[t]<(kuan1+244))
		{
			tp_dev.scan(0);
		}
		EDtemp[0]=7;
		LCD_Fill(chang1+150,kuan1+250,chang1+150+13,kuan1+250+13,WHITE);
		LCD_ShowNum(chang1+150,kuan1+250,7,2,24);
	}
	//数字8
	if(tp_dev.x[t]>(chang1+105)&&tp_dev.x[t]<(chang1+20+137)
	&&tp_dev.y[t]>(kuan1+180)&&tp_dev.y[t]<(kuan1+244)&&menu==2)  //输入数量
	{
		while(tp_dev.x[t]>(chang1+105)&&tp_dev.x[t]<(chang1+20+137)
	  &&tp_dev.y[t]>(kuan1+180)&&tp_dev.y[t]<(kuan1+244))
		{
			tp_dev.scan(0);
		}
		EDtemp[0]=8;
		LCD_Fill(chang1+150,kuan1+250,chang1+150+13,kuan1+250+13,WHITE);
		LCD_ShowNum(chang1+150,kuan1+250,8,2,24);
	}
}
void ctp(void)
{
	u8 t=0;	
 	u16 lastpos[5][2];		//记录最后一次的数据 
	tp_dev.scan(0);
  for(t=0;t<CT_MAX_TOUCH;t++)
	{
		if((tp_dev.sta)&(1<<t))
		{
			if(tp_dev.x[t]<lcddev.width&&tp_dev.y[t]<lcddev.height&&tp_dev.sta&(1<<7))
			{
				if(lastpos[t][0]==0XFFFF)
				{
					lastpos[t][0] = tp_dev.x[t];
					lastpos[t][1] = tp_dev.y[t];
				}
				
				if(tp_dev.x[t]>(430)&&tp_dev.y[t]>765)  //主控RST
				{	
					Init();
					RST++;
					menu=0;
					delay_ms(50);
				}
				if(tp_dev.x[t]<(58)&&tp_dev.y[t]>770&&menu==0)  //菜单
				{
					while(tp_dev.x[t]<(58)&&tp_dev.y[t]>770)
					{
						tp_dev.scan(0);
					}
					CAIDAN(1);
					menu=1;
				}
				if(tp_dev.x[t]<(58)&&tp_dev.y[t]>770&&menu>=1)  //返回
				{
					while(tp_dev.x[t]<(58)&&tp_dev.y[t]>770)
					{
						tp_dev.scan(0);
					}
					menu--;
				  CAIDAN(menu);
					
				}
				if(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+168)
					&&tp_dev.y[t]>(kuan1+10)&&tp_dev.y[t]<(kuan1+34)&&menu==1)  //Zigbee初始化
				{
					while(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+168)
					&&tp_dev.y[t]>(kuan1+10)&&tp_dev.y[t]<(kuan1+34))
					{
						tp_dev.scan(0);
					}
					u2_printf("%d\r\n", 1);//向串口2发送数据
				}
				if(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+216)
					&&tp_dev.y[t]>(kuan1+10+50)&&tp_dev.y[t]<(kuan1+34+50)&&menu==1)  //获取终端信息
				{
					while(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+216)
					&&tp_dev.y[t]>(kuan1+10+50)&&tp_dev.y[t]<(kuan1+34+50))
					{
						tp_dev.scan(0);
					}
					u2_printf("%d\r\n", 2);//向串口2发送确认申请
					LINK=1;
			    CTR1=0;
				  CTR2=0;
				}
				if(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+168)
					&&tp_dev.y[t]>(kuan1+10+100)&&tp_dev.y[t]<(kuan1+34+100)&&menu==1)  //输入设备数量
				{
					while(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+168)
					&&tp_dev.y[t]>(kuan1+10+100)&&tp_dev.y[t]<(kuan1+34+100))
					{
						tp_dev.scan(0);
					}
					CAIDAN(2);
					menu=2;
				}
				
				//按键输入数字1-8
				EDs(t);
				
				if(tp_dev.x[t]>(chang1+5)&&tp_dev.x[t]<(chang1+60)
					&&tp_dev.y[t]>(kuan1+305)&&tp_dev.y[t]<(kuan1+340)&&menu==2)  //输入设备数量
				{
					while(tp_dev.x[t]>(chang1+5)&&tp_dev.x[t]<(chang1+60)
					&&tp_dev.y[t]>(kuan1+305)&&tp_dev.y[t]<(kuan1+340))
					{
						tp_dev.scan(0);
					}
					STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)EDtemp,SIZE);
					menu=0;
					Init();
					u2_printf("E%d\r\n", 3);//向串口2发送数据
					CAIDAN(1);
					menu=1;
				}
			}
		}
		else lastpos[t][0]=0XFFFF;
	}
	delay_ms(5);
}


void show()
{
	u8 i,j=0,k=0;
	u8 x=0,y=0,a=0,b=0;
	char *str_x="x",*str_y="y",*str_a="a",*str_b="b";
	
	x=atoi((char*)strtok(strstr((char*)USART2_RX_BUF,str_x),str_x));    //取出x后的终端号
	y=atoi((char*)strtok(strstr((char*)USART2_RX_BUF,str_y),str_y));    //取出y后的终端号
	a=atoi((char*)strtok(strstr((char*)USART2_RX_BUF,str_a),str_a));    //取出温度数值
	b=atoi((char*)strtok(strstr((char*)USART2_RX_BUF,str_b),str_b));    //取出湿度数值
	
  if(x!=0&&x<=ED)  //显示温湿度
  {
		if(LINK==0)
		{
		  LCD_Fill(115,shebei+80*(x-1),195,shebei+80*(x-1)+17,WHITE); 
		}
	  LCD_ShowNum(50+40,shebei+20+80*(x-1),a,2,16);  
		LCD_ShowNum(50+40,shebei+40+80*(x-1),b,2,16);	
		temp[x-1]=0;
	}	
	if(x>ED)
	{
		Show_Str(30,shebei-20,224,16,"警告:设备数与实际连接数不符!",16,0,1);
		temp[x-1]=x;
		for(i=0;i<=TED-1;i++)
		{
			if(temp[i]!=0)
				LCD_ShowNum(260+16*i,shebei-20,temp[i],2,16);	
		}
	}
	for(i=0;i<=TED;i++)
	{
		if(temp[i]==0)
			k++;
	}
	if(k==TED)
		LCD_Fill(30,shebei-21,400,shebei-21+17,WHITE); 
  if(y!=0&&y<=ED)   //传感器读取温度数据报错
	{
		LCD_Fill(115,shebei+80*(y-1),195,shebei+80*(y-1)+17,WHITE); 
		Show_Str(115,shebei+80*(y-1),250,16,"DATA ERROR",16,0,1);
		LCD_ShowNum(50+40,shebei+20+80*(y-1),0,2,16);
		LCD_ShowNum(50+40,shebei+40+80*(y-1),0,2,16);
	}
	
	if(USART2_RX_BUF[0]=='s')          //设备连接报错
	{
		for(i=1;i<=ED;i++)  //逐位判断设备是否断线
		{
			j=USART2_RX_BUF[i];
			if(j=='0')
			{
				LCD_Fill(115,shebei+80*(i-1),195,shebei+80*(i-1)+17,WHITE); 
	  		Show_Str(115,shebei+80*(i-1),250,16,"LINK ERROR",16,0,1);  
		    LCD_ShowNum(50+40,shebei+20+80*(i-1),0,2,16);
		    LCD_ShowNum(50+40,shebei+40+80*(i-1),0,2,16);
			}	
			else if(LINK==1)  //菜单功能2效果显示
			{
				LCD_Fill(115,shebei+80*(i-1),195,shebei+80*(i-1)+17,WHITE);
				Show_Str(115,shebei+80*(i-1),195,16,"LINK OK",16,0,2);  
				LINKtemp[i-1]=i;
				UCTR=0;
			}
		}
	}
}

int main(void)
{	
	u8 i=0;
	u8 len;
	Init();
	
	while(1)
	{	
    if(USART2_RX_STA&0x8000)  
		{
			len=USART2_RX_STA&0x3fff;//得到此次接收到的数据长度
		  USART2_RX_BUF[len]=0;	 	//加入结束?
			CTR1++;
			show();
			USART2_RX_STA=0;
		}
		if(CTR1==ED*2&&ED!=1)
		{
			u2_printf("%d\r\n", 0);//向串口2发送数据
			CTR1=0;
			CTR2=0;
		}		
		else if(CTR2==ED*2)  //发送设备判断申请
			{
				u2_printf("%d\r\n", 0);//向串口2发送数据
			  CTR1=0;
				CTR2=0;
		  }
		if(CTR1==4&&ED==1)
		{
			u2_printf("%d\r\n", 0);//向串口2发送数据
			CTR1=0;
			CTR2=0;
		}

		ctp();	//电容屏
		if(UCTR==2&&LINK==1)  //菜单功能2显示两秒
		{
			for(i=0;i<=ED-1;i++)
			{
				if(LINKtemp[i]!=0)
					LCD_Fill(115,shebei+80*i,195,shebei+80*i+17,WHITE);
			}
			LINK=0;
			UCTR=0;
		}
	}
}

















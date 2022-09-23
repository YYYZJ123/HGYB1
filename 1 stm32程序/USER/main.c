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
//¿ØÖÆÖ÷¿ØÖÕ¶Ë²ÎÊı1-8
u8 TED=8,ED=0,tempctr=0;
u8 EDtemp[]={0},temp[8]={0};
//Éè±¸È·ÈÏÇëÇóÏà¹Ø
u8 CTR1=0,CTR2=0,UCTR=0,LINK=0,LINKtemp[8]={0};
//²Ëµ¥¹¦ÄÜÏà¹Ø
u8 menu=0,RST=0;
int chang1=240,chang2=470,kuan1=225,kuan2=575;
int shebei=110;

#define SIZE sizeof(EDtemp)		//Êı×é³¤¶È
#define FLASH_SAVE_ADDR  0X08070000		//ÉèÖÃFLASH ±£´æµØÖ·(±ØĞëÎªÅ¼Êı£¬ÇÒÆäÖµÒª´óÓÚ±¾´úÂëËùÕ¼ÓÃFLASHµÄ´óĞ¡+0X08000000)

/************************************************
 ALIENTEK¾«Ó¢STM32¿ª·¢°åÊµÑé37
 ºº×ÖÏÔÊ¾ ÊµÑé 
 ¼¼ÊõÖ§³Ö£ºwww.openedv.com
 ÌÔ±¦µêÆÌ£ºhttp://eboard.taobao.com 
 ¹Ø×¢Î¢ĞÅ¹«ÖÚÆ½Ì¨Î¢ĞÅºÅ£º"ÕıµãÔ­×Ó"£¬Ãâ·Ñ»ñÈ¡STM32×ÊÁÏ¡£
 ¹ãÖİÊĞĞÇÒíµç×Ó¿Æ¼¼ÓĞÏŞ¹«Ë¾  
 ×÷Õß£ºÕıµãÔ­×Ó @ALIENTEK
************************************************/
void Init(void)
{
	u8 i;
	u8 str[17]="Ò»¶şÈıËÄÎåÁùÆß°Ë";  //ÏÔÊ¾Éè±¸ºÅ
	
	delay_init();	    	 //ÑÓÊ±º¯Êı³õÊ¼»¯	  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//ÉèÖÃÖĞ¶ÏÓÅÏÈ¼¶·Ö×éÎª×é2£º2Î»ÇÀÕ¼ÓÅÏÈ¼¶£¬2Î»ÏìÓ¦ÓÅÏÈ¼¶
	usart_init(115200);	 	//´®¿Ú³õÊ¼»¯Îª115200
	usart2_init(115200);	 	//´®¿Ú³õÊ¼»¯Îª115200
 	usmart_dev.init(72);		//³õÊ¼»¯USMART		
 	LED_Init();		  			//³õÊ¼»¯ÓëLEDÁ¬½ÓµÄÓ²¼ş½Ó¿Ú
	KEY_Init();					//³õÊ¼»¯°´¼ü
	LCD_Init();			   		//³õÊ¼»¯LCD   
	tp_dev.init();
	W25QXX_Init();				//³õÊ¼»¯W25Q128
 	my_mem_init(SRAMIN);		//³õÊ¼»¯ÄÚ²¿ÄÚ´æ³Ø
	exfuns_init();				//ÎªfatfsÏà¹Ø±äÁ¿ÉêÇëÄÚ´æ  
 	f_mount(fs[0],"0:",1); 		//¹ÒÔØSD¿¨ 
 	f_mount(fs[1],"1:",1); 		//¹ÒÔØFLASH.
	font_init();
	SD_Init();
	
	TIM3_Int_Init(ED*10000-1,7199);//10KhzµÄ¼ÆÊıÆµÂÊ£¬¼ÆÊıµ½5000Îª500ms 
	STMFLASH_Read(FLASH_SAVE_ADDR,(u16*)EDtemp,SIZE);
	if(EDtemp[0]==0)
		ED=TED; 
	else
		ED=EDtemp[0];
	for(i=0;i<=TED;i++)
		temp[i]=0;
	Show_Str(132,40,250,24,"»¯¹¤ÒÇ±íÍø¼à¿ØÏµÍ³",24,0,2); 	
	for(i=0;i<ED;i++)
	{
		Show_Str(30,shebei+80*i,200,16,"Éè±¸",16,0,1);		
		Show_Str(62,shebei+80*i,20,16,&str[i*2],16,0,1);
    Show_Str(115,shebei+80*i,250,16,"WAITING",16,0,2);		
		Show_Str(30,shebei+20+80*i,200,16,"ÎÂ¶È£º",16,0,1);Show_Str(115,shebei+20+80*i,200,16,"C",16,0,2);
	  Show_Str(30,shebei+40+80*i,200,16,"Êª¶È£º",16,0,1);Show_Str(115,shebei+40+80*i,200,16,"%",16,0,2);
	}
	Show_Str(434,770,250,24,"RST",24,0,2); 
	Show_Str(10,770,250,24,"²Ëµ¥",24,0,2); 
	delay_ms(100);
	u2_printf("%d\r\n", 2);//Ïò´®¿Ú2·¢ËÍÊı¾İ
}
  
void CAIDAN(u8 m)
{
	if(m==0)
	{
		LCD_Fill(10,770,60,800,WHITE); 
		Show_Str(10,770,250,24,"²Ëµ¥",24,0,2); 
		LCD_Fill(chang1-5,kuan1-5,chang2+5,kuan2+5,WHITE);
	}
	if(m==1)
	{
		LCD_Fill(10,770,60,800,WHITE); 
		Show_Str(10,770,250,24,"·µ»Ø",24,0,2); 
		LCD_Fill(chang1+5,kuan1+5,chang2-5,kuan2-5,WHITE);
		lcd_draw_bline(chang1,kuan1,chang1,kuan2,1,RED);
		lcd_draw_bline(chang2,kuan1,chang2,kuan2,1,RED);
		lcd_draw_bline(chang1,kuan1,chang2,kuan1,1,RED);
		lcd_draw_bline(chang1,kuan2,chang2,kuan2,1,RED);
		Show_Str(chang1+10,kuan1+10,250,24,"1.ZigbeeÖØ×éÍø",24,0,1);
		Show_Str(chang1+10,kuan1+60,250,24,"2.»ñÈ¡ÖÕ¶ËÁ¬½ÓĞÅÏ¢",24,0,1);
		Show_Str(chang1+10,kuan1+110,250,24,"3.ÊäÈëÖÕ¶ËÊıÁ¿",24,0,1);
	}
	if(m==2)
	{
		LCD_Fill(chang1+5,kuan1+5,chang2-5,kuan2-5,WHITE);
		LCD_ShowNum(chang1+20,kuan1+20,1,2,24);	    //Êı×Ö1
		LCD_ShowNum(chang1+20+70,kuan1+20,2,2,24);	//Êı×Ö2
		LCD_ShowNum(chang1+20+140,kuan1+20,3,2,24);	//Êı×Ö3
		LCD_ShowNum(chang1+20,kuan1+110,4,2,24);	  //Êı×Ö4
		LCD_ShowNum(chang1+20+70,kuan1+110,5,2,24);	//Êı×Ö5
		LCD_ShowNum(chang1+20+140,kuan1+110,6,2,24);//Êı×Ö6	
		LCD_ShowNum(chang1+20+35,kuan1+200,7,2,24);	//Êı×Ö7
		LCD_ShowNum(chang1+20+105,kuan1+200,8,2,24);//Êı×Ö8	
		Show_Str(chang1+10,kuan1+250,250,24,"ÖÕ¶ËÊı(1-8)£º",24,0,1);
		Show_Str(chang1+10,kuan1+280,250,16,"×¢:Ğë´óÓÚµÈÓÚÊµ¼ÊÁ¬½ÓÊı",16,0,1);
		Show_Str(chang1+10,kuan1+310,250,24,"È·¶¨",24,0,1);
	}
}
////////////////////////////////////////////////////////////////////////////////
 //5¸ö´¥¿ØµãµÄÑÕÉ«												 
const u16 POINT_COLOR_TBL[CT_MAX_TOUCH]={RED,GREEN,BLUE,BROWN,GRED};  
//µçÈİ´¥ÃşÆÁ²âÊÔº¯Êı
void EDs(u8 t)
{
	//Êı×Ö1
	if(tp_dev.x[t]>(chang1)&&tp_dev.x[t]<(chang1+52)
	&&tp_dev.y[t]>(kuan1)&&tp_dev.y[t]<(kuan1+64)&&menu==2)  //ÊäÈëÊıÁ¿
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
	//Êı×Ö2
	if(tp_dev.x[t]>(chang1+20+50)&&tp_dev.x[t]<(chang1+20+90)
	&&tp_dev.y[t]>(kuan1)&&tp_dev.y[t]<(kuan1+52)&&menu==2)  //ÊäÈëÊıÁ¿
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
	//Êı×Ö3
	if(tp_dev.x[t]>(chang1+20+100)&&tp_dev.x[t]<(chang1+20+180)
	&&tp_dev.y[t]>(kuan1)&&tp_dev.y[t]<(kuan1+52)&&menu==2)  //ÊäÈëÊıÁ¿
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
	//Êı×Ö4
	if(tp_dev.x[t]>(chang1)&&tp_dev.x[t]<(chang1+52)
	&&tp_dev.y[t]>(kuan1+90)&&tp_dev.y[t]<(kuan1+154)&&menu==2)  //ÊäÈëÊıÁ¿
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
	//Êı×Ö5
	if(tp_dev.x[t]>(chang1+20+50)&&tp_dev.x[t]<(chang1+20+90)
	&&tp_dev.y[t]>(kuan1+90)&&tp_dev.y[t]<(kuan1+154)&&menu==2)  //ÊäÈëÊıÁ¿
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
	//Êı×Ö6
	if(tp_dev.x[t]>(chang1+20+100)&&tp_dev.x[t]<(chang1+20+180)
	&&tp_dev.y[t]>(kuan1+90)&&tp_dev.y[t]<(kuan1+154)&&menu==2)  //ÊäÈëÊıÁ¿
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
	//Êı×Ö7
	if(tp_dev.x[t]>(chang1+35)&&tp_dev.x[t]<(chang1+20+67)
	&&tp_dev.y[t]>(kuan1+180)&&tp_dev.y[t]<(kuan1+244)&&menu==2)  //ÊäÈëÊıÁ¿
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
	//Êı×Ö8
	if(tp_dev.x[t]>(chang1+105)&&tp_dev.x[t]<(chang1+20+137)
	&&tp_dev.y[t]>(kuan1+180)&&tp_dev.y[t]<(kuan1+244)&&menu==2)  //ÊäÈëÊıÁ¿
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
 	u16 lastpos[5][2];		//¼ÇÂ¼×îºóÒ»´ÎµÄÊı¾İ 
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
				
				if(tp_dev.x[t]>(430)&&tp_dev.y[t]>765)  //Ö÷¿ØRST
				{	
					Init();
					RST++;
					menu=0;
					delay_ms(50);
				}
				if(tp_dev.x[t]<(58)&&tp_dev.y[t]>770&&menu==0)  //²Ëµ¥
				{
					while(tp_dev.x[t]<(58)&&tp_dev.y[t]>770)
					{
						tp_dev.scan(0);
					}
					CAIDAN(1);
					menu=1;
				}
				if(tp_dev.x[t]<(58)&&tp_dev.y[t]>770&&menu>=1)  //·µ»Ø
				{
					while(tp_dev.x[t]<(58)&&tp_dev.y[t]>770)
					{
						tp_dev.scan(0);
					}
					menu--;
				  CAIDAN(menu);
					
				}
				if(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+168)
					&&tp_dev.y[t]>(kuan1+10)&&tp_dev.y[t]<(kuan1+34)&&menu==1)  //Zigbee³õÊ¼»¯
				{
					while(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+168)
					&&tp_dev.y[t]>(kuan1+10)&&tp_dev.y[t]<(kuan1+34))
					{
						tp_dev.scan(0);
					}
					u2_printf("%d\r\n", 1);//Ïò´®¿Ú2·¢ËÍÊı¾İ
				}
				if(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+216)
					&&tp_dev.y[t]>(kuan1+10+50)&&tp_dev.y[t]<(kuan1+34+50)&&menu==1)  //»ñÈ¡ÖÕ¶ËĞÅÏ¢
				{
					while(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+216)
					&&tp_dev.y[t]>(kuan1+10+50)&&tp_dev.y[t]<(kuan1+34+50))
					{
						tp_dev.scan(0);
					}
					u2_printf("%d\r\n", 2);//Ïò´®¿Ú2·¢ËÍÈ·ÈÏÉêÇë
					LINK=1;
			    CTR1=0;
				  CTR2=0;
				}
				if(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+168)
					&&tp_dev.y[t]>(kuan1+10+100)&&tp_dev.y[t]<(kuan1+34+100)&&menu==1)  //ÊäÈëÉè±¸ÊıÁ¿
				{
					while(tp_dev.x[t]>(chang1+10)&&tp_dev.x[t]<(chang1+168)
					&&tp_dev.y[t]>(kuan1+10+100)&&tp_dev.y[t]<(kuan1+34+100))
					{
						tp_dev.scan(0);
					}
					CAIDAN(2);
					menu=2;
				}
				
				//°´¼üÊäÈëÊı×Ö1-8
				EDs(t);
				
				if(tp_dev.x[t]>(chang1+5)&&tp_dev.x[t]<(chang1+60)
					&&tp_dev.y[t]>(kuan1+305)&&tp_dev.y[t]<(kuan1+340)&&menu==2)  //ÊäÈëÉè±¸ÊıÁ¿
				{
					while(tp_dev.x[t]>(chang1+5)&&tp_dev.x[t]<(chang1+60)
					&&tp_dev.y[t]>(kuan1+305)&&tp_dev.y[t]<(kuan1+340))
					{
						tp_dev.scan(0);
					}
					STMFLASH_Write(FLASH_SAVE_ADDR,(u16*)EDtemp,SIZE);
					menu=0;
					Init();
					u2_printf("E%d\r\n", 3);//Ïò´®¿Ú2·¢ËÍÊı¾İ
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
	
	x=atoi((char*)strtok(strstr((char*)USART2_RX_BUF,str_x),str_x));    //È¡³öxºóµÄÖÕ¶ËºÅ
	y=atoi((char*)strtok(strstr((char*)USART2_RX_BUF,str_y),str_y));    //È¡³öyºóµÄÖÕ¶ËºÅ
	a=atoi((char*)strtok(strstr((char*)USART2_RX_BUF,str_a),str_a));    //È¡³öÎÂ¶ÈÊıÖµ
	b=atoi((char*)strtok(strstr((char*)USART2_RX_BUF,str_b),str_b));    //È¡³öÊª¶ÈÊıÖµ
	
  if(x!=0&&x<=ED)  //ÏÔÊ¾ÎÂÊª¶È
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
		Show_Str(30,shebei-20,224,16,"¾¯¸æ:Éè±¸ÊıÓëÊµ¼ÊÁ¬½ÓÊı²»·û!",16,0,1);
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
  if(y!=0&&y<=ED)   //´«¸ĞÆ÷¶ÁÈ¡ÎÂ¶ÈÊı¾İ±¨´í
	{
		LCD_Fill(115,shebei+80*(y-1),195,shebei+80*(y-1)+17,WHITE); 
		Show_Str(115,shebei+80*(y-1),250,16,"DATA ERROR",16,0,1);
		LCD_ShowNum(50+40,shebei+20+80*(y-1),0,2,16);
		LCD_ShowNum(50+40,shebei+40+80*(y-1),0,2,16);
	}
	
	if(USART2_RX_BUF[0]=='s')          //Éè±¸Á¬½Ó±¨´í
	{
		for(i=1;i<=ED;i++)  //ÖğÎ»ÅĞ¶ÏÉè±¸ÊÇ·ñ¶ÏÏß
		{
			j=USART2_RX_BUF[i];
			if(j=='0')
			{
				LCD_Fill(115,shebei+80*(i-1),195,shebei+80*(i-1)+17,WHITE); 
	  		Show_Str(115,shebei+80*(i-1),250,16,"LINK ERROR",16,0,1);  
		    LCD_ShowNum(50+40,shebei+20+80*(i-1),0,2,16);
		    LCD_ShowNum(50+40,shebei+40+80*(i-1),0,2,16);
			}	
			else if(LINK==1)  //²Ëµ¥¹¦ÄÜ2Ğ§¹ûÏÔÊ¾
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
			len=USART2_RX_STA&0x3fff;//µÃµ½´Ë´Î½ÓÊÕµ½µÄÊı¾İ³¤¶È
		  USART2_RX_BUF[len]=0;	 	//¼ÓÈë½áÊø·
			CTR1++;
			show();
			USART2_RX_STA=0;
		}
		if(CTR1==ED*2&&ED!=1)
		{
			u2_printf("%d\r\n", 0);//Ïò´®¿Ú2·¢ËÍÊı¾İ
			CTR1=0;
			CTR2=0;
		}		
		else if(CTR2==ED*2)  //·¢ËÍÉè±¸ÅĞ¶ÏÉêÇë
			{
				u2_printf("%d\r\n", 0);//Ïò´®¿Ú2·¢ËÍÊı¾İ
			  CTR1=0;
				CTR2=0;
		  }
		if(CTR1==4&&ED==1)
		{
			u2_printf("%d\r\n", 0);//Ïò´®¿Ú2·¢ËÍÊı¾İ
			CTR1=0;
			CTR2=0;
		}

		ctp();	//µçÈİÆÁ
		if(UCTR==2&&LINK==1)  //²Ëµ¥¹¦ÄÜ2ÏÔÊ¾Á½Ãë
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

















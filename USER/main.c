#include "led.h"
#include "delay.h"
#include "sys.h"
#include "includes.h" 
#include "24cxx.h"
#include "touch.h"
#include "key.h"
#include "lcd.h"
#include "myiic.h"
#include "usart.h"
#include "PWM.h"
#include "text.h"
#include "image2lcd.h"
#include "stdlib.h"

/////////////////////////UCOSII��������///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//�����ջ	
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);	
 			   
//LED0����
//�����������ȼ�
#define BUZZER_TASK_PRIO       			2 
//���������ջ��С
#define BUZZER_STK_SIZE  		    		64
//�����ջ	
OS_STK BUZZER_TASK_STK[BUZZER_STK_SIZE];
//������
void buzzer_task(void *pdata);


//LED1����
//�����������ȼ�
#define LED1_TASK_PRIO       			6 
//���������ջ��С
#define LED1_STK_SIZE  					64
//�����ջ
OS_STK LED1_TASK_STK[LED1_STK_SIZE];
//������
void led1_task(void *pdata);

//����������
//�����������ȼ�
#define TOUCH_TASK_PRIO       		 	6
//���������ջ��С
#define TOUCH_STK_SIZE  				64
//�����ջ	
OS_STK TOUCH_TASK_STK[TOUCH_STK_SIZE];
//������
void touch_task(void *pdata);

//������
//�����������ȼ�
#define MAIN_TASK_PRIO       			4 
//���������ջ��С
#define MAIN_STK_SIZE  					128
//�����ջ	
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//������
void main_task(void *pdata);

//����ɨ������
//�����������ȼ�
#define KEY_TASK_PRIO       			3 
//���������ջ��С
#define KEY_STK_SIZE  					64
//�����ջ	
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//������
void key_task(void *pdata);



OS_EVENT * msg_key;			//���������¼���ָ��
OS_EVENT * msg_fre;			//���������¼���ָ��
OS_EVENT * sem_led1;		//LED1�ź���ָ��	 


void ucos_load_main_ui(void)
{
	LCD_Clear(WHITE);	//����
	LCD_Display_Dir(1);
 	POINT_COLOR=BLACK;	//��������Ϊ��ɫ 
//	LCD_ShowString(30,10,200,16,16,"Mini STM32");	
//	LCD_ShowString(30,30,200,16,16,"UCOSII TEST2");	
//	LCD_ShowString(30,50,200,16,16,"ATOM@ALIENTEK");
//   	LCD_ShowString(30,75,200,16,16,"KEY0:LED1 KEY_UP:ADJUST");	
//   	LCD_ShowString(30,95,200,16,16,"KEY1:CLEAR"); 
//	LCD_ShowString(80,210,200,16,16,"Touch Area");
	//����	
	LCD_DrawLine(0,0,0,lcddev.height);
	LCD_DrawLine(40,0,40,lcddev.height);
	LCD_DrawLine(80,0,80,lcddev.height);
	LCD_DrawLine(120,0,120,lcddev.height);
	LCD_DrawLine(160,0,160,lcddev.height);
	LCD_DrawLine(200,0,200,lcddev.height);
	LCD_DrawLine(240,0,240,lcddev.height);
	LCD_DrawLine(280,0,280,lcddev.height);
	//����
	LCD_DrawLine(0,lcddev.height/2,280,lcddev.height/2);
	//mark
	LCD_ShowChar(10,10,'1',24,0);
	LCD_ShowChar(50,10,'2',24,0);
	LCD_ShowChar(90,10,'3',24,0);
	LCD_ShowChar(130,10,'4',24,0);
	LCD_ShowChar(170,10,'5',24,0);
	LCD_ShowChar(210,10,'6',24,0);
	LCD_ShowChar(250,10,'7',24,0);
	
	LCD_ShowChar(10, 130,'1',24,0);
	LCD_ShowChar(50, 130,'2',24,0);
	LCD_ShowChar(90, 130,'3',24,0);
	LCD_ShowChar(130,130,'4',24,0);
	LCD_ShowChar(170,130,'5',24,0);
	LCD_ShowChar(210,130,'6',24,0);
	LCD_ShowChar(250,130,'7',24,0);
	
	TP_Draw_Big_Point(15,127,BLACK);
	TP_Draw_Big_Point(55,127,BLACK);
	TP_Draw_Big_Point(95,127,BLACK);
	TP_Draw_Big_Point(135,127,BLACK);
	TP_Draw_Big_Point(175,127,BLACK);
	TP_Draw_Big_Point(215,127,BLACK);
	TP_Draw_Big_Point(255,127,BLACK);

 	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	
	OLED_Show_Font(288,12,3);				//Ƕ
	OLED_Show_Font(288,48,4);				//��
	OLED_Show_Font(288,84,5);				//ʽ 
	
	OLED_Show_Font(288,132,6);				//��
	OLED_Show_Font(288,168,7);				//��
	OLED_Show_Font(288,204,8);				//�� 
	
	
}	  

extern const u8 gImage_image1[];

int main(void)
{	
	HEADCOLOR *imginfo;
	delay_init();	    	 //��ʱ������ʼ��	
	uart_init(9600);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// �����ж����ȼ�����2
	LED_Init();		  	//��ʼ����LED���ӵ�Ӳ���ӿ�
	LCD_Init();			   	//��ʼ��LCD
	KEY_Init();				//������ʼ��
	tp_dev.init();			//��������ʼ��

	
	imginfo=(HEADCOLOR*)gImage_image1;	//�õ��ļ���Ϣ 
	srand(imginfo->h*imginfo->w);
	image_display(0,0,(u8*)gImage_image1);//��ָ����ַ��ʾͼƬ
	delay_ms(3000);
	
	ucos_load_main_ui();	//����������
	OSInit();   
	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();	
}

	  
//��ʼ����
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	msg_key=OSMboxCreate((void*)0);	//������Ϣ����
	msg_fre=OSMboxCreate((void*)1);	//������Ϣ����
	sem_led1=OSSemCreate(0);		//�����ź���		
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������
  	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 	OSTaskCreate(buzzer_task,(void *)0,(OS_STK*)&BUZZER_TASK_STK[BUZZER_STK_SIZE-1],BUZZER_TASK_PRIO);		//��������������
	OSTaskCreate(touch_task,(void *)0,(OS_STK*)&TOUCH_TASK_STK[TOUCH_STK_SIZE-1],TOUCH_TASK_PRIO);	 				   
 	//OSTaskCreate(led1_task,(void *)0,(OS_STK*)&LED1_TASK_STK[LED1_STK_SIZE-1],LED1_TASK_PRIO);	 				   
 	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);	 				   
 	OSTaskCreate(key_task,(void *)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO);	 				   
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}

//������
void main_task(void *pdata)
{							 
	u32 key=0,fre = 0;	
	u8 err;	
	u8 tcnt=0;
	while(1)
	{
		key=(u32)OSMboxPend(msg_key,10,&err);   
		switch(key)
		{ 
			case KEY0_PRES://�����ź���
				fre = 0;
				OSMboxPost(msg_fre,(void*)fre);
				break;
			case KEY1_PRES://���
				LCD_Fill(0,121,lcddev.width,lcddev.height,WHITE);
				break;
			case WKUP_PRES://У׼
				OSTaskSuspend(TOUCH_TASK_PRIO);	//������������		 
 				if((tp_dev.touchtype&0X80)==0)TP_Adjust();	   
 				OSTaskResume(TOUCH_TASK_PRIO);	//���
				ucos_load_main_ui();			//���¼���������		 
				break;
		}
//   		if(semmask||sem_led1->OSEventCnt)//��Ҫ��ʾsem		
//		{
//			POINT_COLOR=BLUE;
//			LCD_ShowxNum(192,50,sem_led1->OSEventCnt,3,16,0X80);//��ʾ�ź�����ֵ
//			if(sem_led1->OSEventCnt==0)semmask=0;	//ֹͣ����
//		} 
//		if(tcnt==50)//0.5�����һ��CPUʹ����
//		{
//			tcnt=0;
//			POINT_COLOR=BLUE;		  
//			LCD_ShowxNum(192,30,OSCPUUsage,3,16,0);	//��ʾCPUʹ����   
//		}
		tcnt++;
		delay_ms(10);
	}
}

//LED0����
void buzzer_task(void *pdata)
{	 
	static unsigned int music[18] = {10,1912,1704,1517,1433,1276,1136,1012,0,0,0,956,851,759,716,638,568,506};
	u8 err;
	u8 NowNote = 0,LastNote = 0;
	
	while(1)
	{
		NowNote = (u32)OSMboxPend(msg_fre,0,&err);
		//printf("\t���ݹ�����ֵ��%d\n",NowNote);
		if(NowNote != LastNote)
		{
			TIM1_PWM_Init((music[NowNote]-1),71);		//72000/((arr+1)*(71+1))=10KHz		7199->10k	14399->5k	71999->1k	35999->2k
			TIM_SetCompare1(TIM1,music[NowNote]/2);
			
		}
		delay_ms(10);
		LastNote = NowNote;
	};
}

//LED1����
void led1_task(void *pdata)
{	  
	while(1)
	{
		LED1=0;
		delay_us(100);
		LED1=1;
		delay_us(100);
	};
}

//����ɨ������
void key_task(void *pdata)
{	
	u8 key;		    						 
	while(1)
	{
		key=KEY_Scan(0);   
		if(key)OSMboxPost(msg_key,(void*)key);//������Ϣ
 		delay_ms(10);
	}
}

//����������
void touch_task(void *pdata)
{	
	u8 SKey = 0,SKeyLast = 0;
	while(1)
	{
		tp_dev.scan(0); 		 
		if(tp_dev.sta&TP_PRES_DOWN)		//������������
		{	
		 	if(tp_dev.x[0]>0 && tp_dev.x[0]<120	&& tp_dev.y[0]>280 && tp_dev.y[0]<320)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//��ͼ
				LCD_Fill(1, 40, 39, lcddev.height/2-1,RED);
				SKey = 1;	
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>0 && tp_dev.x[0]<120 && tp_dev.y[0]>240 && tp_dev.y[0]<280)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],BLUE);		//��ͼ
				LCD_Fill(41, 40, 79, lcddev.height/2-1,BLUE);
				SKey = 2;					
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>0 && tp_dev.x[0]<120 && tp_dev.y[0]>200 && tp_dev.y[0]<240)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],YELLOW);		//��ͼ
				LCD_Fill(81, 40, 119, lcddev.height/2-1,YELLOW);
				SKey = 3;					
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>0 && tp_dev.x[0]<120 && tp_dev.y[0]>160 && tp_dev.y[0]<200)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],DARKBLUE);		//��ͼ
				LCD_Fill(121, 40, 159, lcddev.height/2-1,DARKBLUE);
				SKey = 4;					
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>0 && tp_dev.x[0]<120 && tp_dev.y[0]>120 && tp_dev.y[0]<160)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],BLACK);		//��ͼ
				LCD_Fill(161, 40, 199, lcddev.height/2-1,BLACK);
				SKey = 5;					
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>0 && tp_dev.x[0]<120 && tp_dev.y[0]>80 && tp_dev.y[0]<120)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],GREEN);		//��ͼ
				LCD_Fill(201, 40, 239, lcddev.height/2-1,GREEN);
				SKey = 6;					
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>0 && tp_dev.x[0]<120 && tp_dev.y[0]>40 && tp_dev.y[0]<80)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],MAGENTA);		//��ͼ
				LCD_Fill(241, 40, 279, lcddev.height/2-1,MAGENTA);
				SKey = 7;					
				delay_ms(2); 			   
			}
			
			//����
			else if(tp_dev.x[0]>120 && tp_dev.x[0]<240 && tp_dev.y[0]>280 && tp_dev.y[0]<320)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],RED);		//��ͼ1
				LCD_Fill(1, lcddev.height/2+40, 39, lcddev.height,RED);
				SKey = 11;				
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>120 && tp_dev.x[0]<240 && tp_dev.y[0]>240 && tp_dev.y[0]<280)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],BLUE);		//��ͼ2
				LCD_Fill(41, lcddev.height/2+40, 79, lcddev.height,BLUE);
				SKey = 12;					
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>120 && tp_dev.x[0]<240 && tp_dev.y[0]>200 && tp_dev.y[0]<240)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],YELLOW);		//��ͼ3
				LCD_Fill(81, lcddev.height/2+40, 119, lcddev.height,YELLOW);
				SKey = 13;					
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>120 && tp_dev.x[0]<240 && tp_dev.y[0]>160 && tp_dev.y[0]<200)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],DARKBLUE);		//��ͼ4
				LCD_Fill(121, lcddev.height/2+40, 159, lcddev.height,DARKBLUE);
				SKey = 14;					
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>120 && tp_dev.x[0]<240 && tp_dev.y[0]>120 && tp_dev.y[0]<160)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],BLACK);		//��ͼ5
				LCD_Fill(161, lcddev.height/2+40, 199, lcddev.height,BLACK);
				SKey = 15;					
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>120 && tp_dev.x[0]<240 && tp_dev.y[0]>80 && tp_dev.y[0]<120)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],GREEN);		//��ͼ6
				LCD_Fill(201, lcddev.height/2+40, 239, lcddev.height,GREEN);
				SKey = 16;					
				delay_ms(2); 			   
			}
			else if(tp_dev.x[0]>120 && tp_dev.x[0]<240 && tp_dev.y[0]>40 && tp_dev.y[0]<80)
			{													   
				//TP_Draw_Big_Point(tp_dev.x[0],tp_dev.y[0],MAGENTA);		//��ͼ7
				LCD_Fill(241, lcddev.height/2+40, 279, lcddev.height,MAGENTA);
				SKey = 17;					
				delay_ms(2); 			   
			}

		}
		else
		{
			SKey = 0;
			delay_ms(10);	//û�а������µ�ʱ�� 
		}
		if(SKey != SKeyLast)
		{
			switch(SKeyLast)
			{
				case 1:  LCD_Fill(1,   40, 39,  lcddev.height/2-1,WHITE);break;
				case 2:  LCD_Fill(41,  40, 79,  lcddev.height/2-1,WHITE);break;
				case 3:  LCD_Fill(81,  40, 119, lcddev.height/2-1,WHITE);break;
				case 4:  LCD_Fill(121, 40, 159, lcddev.height/2-1,WHITE);break;
				case 5:  LCD_Fill(161, 40, 199, lcddev.height/2-1,WHITE);break;
				case 6:  LCD_Fill(201, 40, 239, lcddev.height/2-1,WHITE);break;
				case 7:  LCD_Fill(241, 40, 279, lcddev.height/2-1,WHITE);break;
				case 11: LCD_Fill(1,   lcddev.height/2+40, 39,  lcddev.height,WHITE);break;
				case 12: LCD_Fill(41,  lcddev.height/2+40, 79,  lcddev.height,WHITE);break;
				case 13: LCD_Fill(81,  lcddev.height/2+40, 119, lcddev.height,WHITE);break;
				case 14: LCD_Fill(121, lcddev.height/2+40, 159, lcddev.height,WHITE);break;
				case 15: LCD_Fill(161, lcddev.height/2+40, 199, lcddev.height,WHITE);break;
				case 16: LCD_Fill(201, lcddev.height/2+40, 239, lcddev.height,WHITE);break;
				case 17: LCD_Fill(241, lcddev.height/2+40, 279, lcddev.height,WHITE);break;
			}
		}
		SKeyLast = SKey;
		
		//printf("\t׼�����ݵ�ֵ��%d\n",SKey);
		OSMboxPost(msg_fre,(void*)SKey);
	}
}








#include <IAP15W4K61S4.h>
#include <stdio.h>

typedef unsigned char uChar8;
typedef unsigned int uInt16;

/**************************************¶¨ÒåÈ«¾Ö±äÁ¿********************************************************************/	
sbit LED = P5^5;                                                 //LEDµÆ
uInt16 count = 0;                                                //¼ÆÊýÆ÷
int time = 100;                                                  //µ¹¼ÆÊ±³õÊ¼Ê±¼ä
struct {                                                         //¶¨Ê±Æ÷³õÊ¼Öµ½á¹¹Ìå
    uChar8 TH;
    uChar8 TL;
}Timer1, Timer2;

/**************************************¶¨ÒåÍâ²¿º¯Êý½Ó¿Ú************************************************************************/
extern void LCD12864ShowString(uChar8 x, uChar8 y, uChar8* str); //µÚyÐÐµÚxÁÐ£¬¿ªÊ¼´òÓ¡str
extern void LCD12864EraseArea(uChar8 x, uChar8 y, uChar8 size);  //´ÓµÚyÐÐµÚxÁÐ¿ªÊ¼£¬É¾³ýsize×Ö·
extern void img_dele_underLine(void);                            //É¾³ýËùÓÐÏÂ»®Ïß
extern void keyScanner(void);                                    //°´¼üÉ¨Ãè
extern uChar8 interface;                                         //È«¾Ö±äÁ¿£¬±íÊ¾µ±Ç°Ëù´¦½çÃæ

/**************************************º¯ÊýÉùÃ÷************************************************************************/
void configureTimer0(void) reentrant;                            //ÏµÍ³Ê±ÖÓtime0ÅäÖÃ
void configureTimer1(uChar8 ms);                                 //¶¨Ê±Æ÷1ÅäÖÃ
void TimerCounter_1 (void);                                      //¶¨Ê±Æ÷1ÖÐ¶Ï£ºÉ¨Ãè°´¼ü
void configureTimer2(void);                                      //¶¨Ê±Æ÷2ÅäÖÃ
void TimerCounter_2 (void);                                      //¶¨Ê±Æ÷1ÖÐ¶Ï£ºµ¹¼ÆÊ±
void display(void);                                              //ÏÔÊ¾µ¹¼ÆÊ±


//ÏµÍ³Ê±ÖÓtime0ÅäÖÃ
void configureTimer0(void) reentrant
{
    TMOD &= 0xF0;
	TMOD |= 0x01;                                                //¶¨Ê±Æ÷0£ºÄ£Ê½1(16Î»¶¨Ê±Æ÷)£¬½öÊÜTR0¿ØÖÆ£»¶¨Ê±Æ÷1£º²¨ÌØÂÊ·¢ÉúÆ÷
//  TH0 = 0xDC;                                                  //¶¨ÒåTick=100´Î/Ãë(¼´0.01Ãë/´Î),TH,TLÖµÓëCPUµÄÆµÂÊÓÐ¹Ø(11.0592M)
//  TL0 = 0x00;  	                                             //OS_CPU_C.CÖÐ¶¨Ê±Æ÷ÖÐ¶ÏÏìÓ¦Ò²ÒªÉèÖÃ£¬OS_CFG.HÖÐOS_TICKS_PER_SECÒ²ÓÐ¹ØÏµ
    TR0  = 1;		                                             //±ØÐëÆô¶¯
}

//¶¨Ê±Æ÷1ÅäÖÃ
void configureTimer1(uChar8 ms)
{
    unsigned long timer = 11059200 / 12;
    timer = timer * ms / 1000;
    timer = 65536 - timer;
    TH1 = (uChar8)(timer >> 8);
    TL1 = (uChar8)timer;
    Timer1.TH = TH1;
    Timer1.TL = TL1;
    TMOD &= 0xF0;
    TMOD |= 0x01;
    ET1 = 1;
    TR1 = 1;
}

//¶¨Ê±Æ÷1ÖÐ¶Ï
void TimerCounter_1 (void) interrupt 3 
{ 
    TH1 = Timer1.TH;
    TL1 = Timer1.TL;
	keyScanner();
}


//¶¨Ê±Æ÷2ÅäÖÃ
void configureTimer2(void)
{
	AUXR &= 0xFB;
	T2H = 0xFC;
	T2L = 0X67;
	Timer2.TH = T2H;
    Timer2.TL = T2L;
	//IE2 |= 0x04;
	AUXR |= 0x10;
}

//¶¨Ê±Æ÷2ÖÐ¶Ï
void TimerCounter_2 (void) interrupt 12
{ 
	T2H = Timer2.TH;
	T2L = Timer2.TL;
	count++;
	if(count == 1000)
	{
		display();
		//LED = !LED;
		count = 0;
	}
}


//ÏÔÊ¾µ¹¼ÆÊ±
void display(void)
{
	uChar8 str[20];
	sprintf(str, "µ¹¼ÆÊ±: %03d", time--);
	LCD12864EraseArea(0, 2, 20);
	LCD12864ShowString(1, 2, str);
	if(time == -1)
	{
		img_dele_underLine();
		sprintf(str, "ÓÎÏ·½áÊø");
		LCD12864EraseArea(0, 1, 20);
		LCD12864ShowString(2, 1, str);
		IE2 = 0x00;                 //¹Ø±Õ¶¨Ê±Æ÷2
		interface = 2;
	}
}
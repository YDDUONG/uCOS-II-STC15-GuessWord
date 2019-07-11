#include <IAP15W4K61S4.h>

#define LCD12864_DB P0

typedef unsigned char uChar8;

/**************************************定义全局变量***********************************************************************/	
sbit LCD12864_RS = P3^5;
sbit LCD12864_RW = P3^6;
sbit LCD12864_E = P3^7;

uChar8 i, j;                                                //循环变量

/**************************************函数声明*******************************************************************************/
void LCD12864WaitReady(void);                               //检查12864是否繁忙
void LCD12864WriteCommand(uChar8 command);	                //写命令函数
void initLCD12864(void);	                                //初始化12864
void LCD12864WriteData(uChar8 dat);                     	//写数据函数
void LCD12864SetCursor(uChar8 x, uChar8 y) ;	            //从第y行第x列写数据
void LCD12864ShowString(uChar8 x, uChar8 y, uChar8* str) ;	//第y行第x列，开始打印str
void LCD12864EraseArea(uChar8 x, uChar8 y, uChar8 size);	//从第y行第x列开始，删除size字符
void LCD12864EraseAll(void);	                            //擦除整个显示屏
void IO_init(void);
void img_disp(uChar8 code *img); 	                        //画128 * 64的像素图片
void img_clear(void);                                       //清屏

void startUnderLine(uChar8 id);                             //管理开始界面下划线
void img_line_start1(void); 	                            //画一条下划线	
void img_dele_line_start1(void);                            //删除这条下划线
void img_line_start2(void);
void img_dele_line_start2(void);	
void img_line_start3(void);
void img_dele_line_start3(void);	

void underLine(uChar8 id);                                  //管理游戏界面下划线
void img_underLine(uChar8 k);                               //画线
void img_dele_underLine(void);                              //删除所有下划线

/**************************************LCD12864功能函数**************************************************************/	
//检查12864是否繁忙
void LCD12864WaitReady(void)
{
    uChar8 state;

    LCD12864_DB = 0xFF; //P0
    LCD12864_RS = 0;
    LCD12864_RW = 1;
    
    do {
        LCD12864_E = 1;
        state = LCD12864_DB;
        LCD12864_E = 0;
    } while(state & 0x80);
}

//写命令函数
void LCD12864WriteCommand(uChar8 command)
{
    LCD12864WaitReady();
    LCD12864_RS = 0;
    LCD12864_RW = 0;
    LCD12864_DB = command;
    LCD12864_E = 1;
    LCD12864_E = 0;
}

//初始化12864
void initLCD12864(void)
{
    LCD12864WriteCommand(0x38);
    LCD12864WriteCommand(0x0C);
    LCD12864WriteCommand(0x06);
    LCD12864WriteCommand(0x01);
}

//写数据函数
void LCD12864WriteData(uChar8 dat)
{
    LCD12864WaitReady();
    LCD12864_RS = 1;
    LCD12864_RW = 0;
    LCD12864_DB = dat;
    LCD12864_E = 1;
    LCD12864_E = 0;
}

//从第y行第x列写数据
void LCD12864SetCursor(uChar8 x, uChar8 y) 
{
    uChar8 address;
    if (0 == y) 
	{
        address = 0x80 + x;
    } else if (1 == y) 
	{
        address = 0x90 + x;
    } else if (2 == y) 
	{
    	address = 0x88 + x;
	} else if (3 == y)
	{
		address = 0x98 + x;
	}
    LCD12864WriteCommand(address);
}

//第y行第x列，开始打印str
void LCD12864ShowString(uChar8 x, uChar8 y, uChar8* str) 
{
    LCD12864SetCursor(x, y);
    while (*str != '\0') {
        LCD12864WriteData(*str++);
    }
}

//从第y行第x列开始，删除size字符
void LCD12864EraseArea(uChar8 x, uChar8 y, uChar8 size)
{
    LCD12864SetCursor(x, y); //定位
    while(size--) {
         LCD12864WriteData(' '); //覆盖原数据
    }
}

//擦除整个显示屏
void LCD12864EraseAll(void)
{
    LCD12864WriteCommand(0x01);
}

//针对IAP15W4K61S4和STC15W4K56S4系列 IO口初始化
void IO_init(void)
{
	P0M0 = 0X00;
	P0M1 = 0X00;

	P1M0 = 0X00;
	P1M1 = 0X00;

	P2M0 = 0X00;
	P2M1 = 0X00;

	P3M0 = 0X00;
	P3M1 = 0X00;

	P4M0 = 0X00;
	P4M1 = 0X00;  
}

/************************************LCD12864画图操作**************************************************************/	
//画128 * 64的像素图片
void img_disp(uChar8 code *img) 
{
	for(j = 0; j < 32; j++)     //上半屏
	{
		for(i = 0; i < 8; i++)
		{
			LCD12864WriteCommand(0x34);      
			LCD12864WriteCommand(0x80 + j);   
			LCD12864WriteCommand(0x80 + i);    
			LCD12864WriteCommand(0x30);      
			LCD12864WriteData(img[j * 16 + i * 2]); 
			LCD12864WriteData(img[j * 16 + i * 2 + 1]);  
		}
	}
	for(j = 32; j < 64; j++)    //下半屏   
	{
		for(i = 0; i<8; i++)
		{
			LCD12864WriteCommand(0x34);        
			LCD12864WriteCommand(0x80 + j - 32);
			LCD12864WriteCommand(0x88 + i);    
			LCD12864WriteCommand(0x30);       
			LCD12864WriteData(img[j * 16 + i * 2]);    
			LCD12864WriteData(img[j * 16 + i * 2 + 1]);    
		} 
	}
	LCD12864WriteCommand(0x36); 
	LCD12864WriteCommand(0x30);	
}

//清屏
void img_clear(void)
{
	for(j = 0; j < 32; j++)
	{
		for(i = 0; i < 8; i++)
		{
			LCD12864WriteCommand(0x34);      
			LCD12864WriteCommand(0x80 + j);   
			LCD12864WriteCommand(0x80 + i);    
			LCD12864WriteCommand(0x30);      
			LCD12864WriteData(0x00); 
			LCD12864WriteData(0x00);  
		}
	}
	for(j = 32; j < 64; j++)      
	{
		for(i = 0; i < 8; i++)
		{
			LCD12864WriteCommand(0x34);        
			LCD12864WriteCommand(0x80 + j - 32);
			LCD12864WriteCommand(0x88 + i);    
			LCD12864WriteCommand(0x30);       
			LCD12864WriteData(0x00); 
			LCD12864WriteData(0x00); 
		} 
	}
	LCD12864WriteCommand(0x36);
	LCD12864WriteCommand(0x30);	
}

/***************************startInterface******************************************************************/
void startUnderLine(uChar8 id)     //管理游戏界面下划线
{
	img_dele_line_start1();
	img_dele_line_start2();
	img_dele_line_start3();
	switch(id)
	{
		case 0: 
			img_line_start1();
			break;
		case 1: 
			img_line_start2();
			break;
		case 2: 
			img_line_start3();
			break;
		default: break;
	}
}

//画一条下划线
void img_line_start1(void)
{
	j = 20;
	for(i = 5; i<7; i++)
	{
		LCD12864WriteCommand(0x34);      
		LCD12864WriteCommand(0x80 + j);   
		LCD12864WriteCommand(0x80 + i);    
		LCD12864WriteCommand(0x30);      
		LCD12864WriteData(0xFF);  
		LCD12864WriteData(0xFF);
	}
	LCD12864WriteCommand(0x36); 
	LCD12864WriteCommand(0x30);	
}

//删除这条下划线
void img_dele_line_start1(void)
{
	j = 20;
	for(i = 5; i < 7; i++)
	{
		LCD12864WriteCommand(0x34);      
		LCD12864WriteCommand(0x80 + j);   
		LCD12864WriteCommand(0x80 + i);    
		LCD12864WriteCommand(0x30);      
		LCD12864WriteData(0x00);  
		LCD12864WriteData(0x00);
	}
	LCD12864WriteCommand(0x36); 
	LCD12864WriteCommand(0x30);	
}

void img_line_start2(void)
{
	j = 37;
	for(i = 5; i < 7; i++)
	{
		LCD12864WriteCommand(0x34);        
		LCD12864WriteCommand(0x80 + j - 32);
		LCD12864WriteCommand(0x88 + i);    
		LCD12864WriteCommand(0x30);       
		LCD12864WriteData(0xFF); 
		LCD12864WriteData(0xFF); 
	} 
	LCD12864WriteCommand(0x36); 
	LCD12864WriteCommand(0x30);	
}

void img_dele_line_start2(void)
{
	j = 37;
	for(i = 5; i < 7; i++)
	{
		LCD12864WriteCommand(0x34);        
		LCD12864WriteCommand(0x80 + j - 32);
		LCD12864WriteCommand(0x88 + i);    
		LCD12864WriteCommand(0x30);       
		LCD12864WriteData(0x00); 
		LCD12864WriteData(0x00); 
	} 
	LCD12864WriteCommand(0x36); 
	LCD12864WriteCommand(0x30);	
}


void img_line_start3(void)
{
	j = 53;
	for(i = 5; i < 7; i++)
	{
		LCD12864WriteCommand(0x34);        
		LCD12864WriteCommand(0x80 + j - 32);
		LCD12864WriteCommand(0x88 + i);    
		LCD12864WriteCommand(0x30);       
		LCD12864WriteData(0xFF); 
		LCD12864WriteData(0xFF); 
	} 
	LCD12864WriteCommand(0x36); 
	LCD12864WriteCommand(0x30);	
}

void img_dele_line_start3(void)
{
	j = 53;
	for(i = 5; i < 7; i++)
	{
		LCD12864WriteCommand(0x34);        
		LCD12864WriteCommand(0x80 + j - 32);
		LCD12864WriteCommand(0x88 + i);    
		LCD12864WriteCommand(0x30);       
		LCD12864WriteData(0x00); 
		LCD12864WriteData(0x00); 
	} 
	LCD12864WriteCommand(0x36); 
	LCD12864WriteCommand(0x30);	
}


/***************************gameInterface******************************************************************/
void underLine(uChar8 id)     //管理游戏界面下划线
{
	img_dele_underLine();
	switch(id)
	{
		case 0: img_underLine(2); break;
		case 1: img_underLine(3); break;
		case 2: img_underLine(4); break;
		case 3: img_underLine(5); break;
		case 4: img_underLine(6); break;
		default: break;
	}
}

void img_underLine(uChar8 k)
{
	j = 29;
	LCD12864WriteCommand(0x34);      
	LCD12864WriteCommand(0x80 + j);   
	LCD12864WriteCommand(0x80 + k);    
	LCD12864WriteCommand(0x30);      
	LCD12864WriteData(0xFF);  
	LCD12864WriteData(0x80);
	LCD12864WriteCommand(0x36); 
	LCD12864WriteCommand(0x30);	
}

void img_dele_underLine(void)
{
	j = 29;
	for(i = 0; i < 8; i++)
	{
		LCD12864WriteCommand(0x34);      
		LCD12864WriteCommand(0x80 + j);   
		LCD12864WriteCommand(0x80 + i);    
		LCD12864WriteCommand(0x30);      
		LCD12864WriteData(0x00);  
		LCD12864WriteData(0x00);
	}
	LCD12864WriteCommand(0x36); 
	LCD12864WriteCommand(0x30);	
}

/*
*********************************************************************************************************
*                                               uC/OS-II
*                                               实时内核
*
*                        (c) Copyright 1992-1998, Jean J. Labrosse, Plantation, FL
*                                               版权所有
*
*                                            MCU-51 专用代码
*                                           KEIL C51大模式编译
*
* 文件名 : OS_CPU_C.C
* 作者   : Jean J. Labrosse
*********************************************************************************************************
*/

#define  OS_CPU_GLOBALS
#include "source\includes.h"

/*
*********************************************************************************************************
*                                        初始化任务堆栈
*
* 描述       : 这个函数被OSTaskCreate()或OSTaskCreateExt()调用，以便初始化新创建任务的堆栈结构。本函数
*              与处理器高度相关。
*
* 参数       : task          指向任务代码的指针
*
*              pdata         当任务第一次执行时将要传入任务的用户数据结构指针
*
*              ptos          栈顶指针。ptos指针被默认为用户堆栈入口指针。如果OS_STK_GROWTH被置1，那么，
*                            ptos指向用户堆栈的最高有效地址。同样地，如果OS_STK_GROWTH清0，ptos将指向
*                            用户堆栈的最低有效地址。
*
*              opt           指定可以改变OSTaskStkInit()行为的选项。(见uCOS_II.H for OS_TASK_OPT_???)。
*
* 返回值     : 我修改了原来的程序，使函数总是返回用户堆栈空间的最低有效地址。这样修改提高了TCB换入换出
*              的效率。
*
* 注意       : 任务堆栈结构:
*
*                                    ---------- -
*                 用户栈最高地址---->|        | |
*                                    ---------- |
*                                    |   ...  | 仿真堆栈空间
*----------                          ---------- | 每任务一个
*|OSTCBCur|               ?C_XBP---->|        | | KEIL自动处理
*----------                          ---------- -
*    |                               |空闲间隔|
*    |     -----------------------   ----------                           ----------
*    \---->|OSTCBCur->OSTCBStkPtr|   |?C_XBP低|                    SP---->|        |
*          -----------------------   ----------                           ----------
*                     |              |?C_XBP高|                           |        |
*                     |              ---------- -                         ----------
*                     |              |        | |                         |   .    |
*                     |              ---------- |                         |   .    |
*                     |              |        | |                         |   .    |
*                     |              ---------- |                         ----------
*                     |              |   .    |长度                       |        | +1
*                     |              |   .    | |                         ----------
*                     |              |   .    | |             OSStack---->|        | 0
*                     |              ---------- |                         ----------
*                     |              |        | |          OSStkStart---->| 不关心 | -1  低地址
*                     |              ---------- -                         ----------
*                     \------------->|  长度  | 低地址                   系统硬件堆栈
*                                    ----------
*                                     用户堆栈                       长度=SP-OSStkStart
*********************************************************************************************************
*/

OS_STK *OSTaskStkInit (void (*task)(void *pd) reentrant, void *ppdata, OS_STK *ptos, INT16U opt) reentrant
{    
    OS_STK *stk;

    ppdata = ppdata;
    opt    = opt;                               //opt没被用到，保留此语句防止告警产生    
    stk    = ptos;                              //用户堆栈最低有效地址
    *stk++ = 15;                                //用户堆栈长度  15 
    *stk++ = (INT16U)task & 0xFF;               //任务地址低8位	(1)
    *stk++ = (INT16U)task >> 8;                 //任务地址高8位 (2)   
    *stk++ = 0x0A;                              //ACC			(3)
    *stk++ = 0x0B;                              //B				(4)
    *stk++ = 0x00;                              //DPH			(5)
    *stk++ = 0x00;                              //DPL			(6)
    *stk++ = 0x00;                              //PSW			(7)
    *stk++ = 0x00;                              //R0			(8)
    
	//R3、R2、R1用于传递任务参数ppdata，其中R3代表存储器类型，R2为高字节偏移，R1为低字节位移。
	//通过分析KEIL汇编，了解到任务的void *ppdata参数恰好是用R3、R2、R1传递，不是通过虚拟堆栈。
    *stk++ = (INT16U)ppdata & 0xFF;             //R1			(9)
    *stk++ = (INT16U)ppdata >> 8;               //R2			(10)
    *stk++ = 0x01;                              //R3  (11) XDATA，所以存储器类型固定为1，见C51.PDF第178页说明。

    *stk++ = 0x04;                              //R4			(12)
    *stk++ = 0x05;                              //R5			(13)
    *stk++ = 0x06;                              //R6			(14)
    *stk++ = 0x07;                              //R7			(15)
                                                //不用保存SP，任务切换时根据用户堆栈长度计算得出。    
    *stk++ = (INT16U) (ptos+MaxStkSize) >> 8;   //?C_XBP 仿真堆栈指针高8位
    *stk++ = (INT16U) (ptos+MaxStkSize) & 0xFF; //?C_XBP 仿真堆栈指针低8位
        
    return ((void *)ptos);
}

#if OS_CPU_HOOKS_EN

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                            (BEGINNING)
*
* Description: This function is called by OSInit() at the beginning of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/
#if OS_VERSION > 203
void OSInitHookBegin (void) reentrant
{

}
#endif

/*
*********************************************************************************************************
*                                       OS INITIALIZATION HOOK
*                                               (END)
*
* Description: This function is called by OSInit() at the end of OSInit().
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts should be disabled during this call.
*********************************************************************************************************
*/
#if OS_VERSION > 203
void OSInitHookEnd (void) reentrant
{
}
#endif

/*
*********************************************************************************************************
*                                          任务创建钩挂函数
*
* 描述       : 任务创建时调用
*
* 参数       : ptcb是指向将被创建任务的任务控制块的指针。
*
* 注意       : 1) 调用期间中断被禁止
*********************************************************************************************************
*/
void OSTaskCreateHook (OS_TCB *ptcb) reentrant
{
    ptcb = ptcb;                       /* Prevent compiler warning                                     */
}


/*
*********************************************************************************************************
*                                          任务删除钩挂函数
*
* 描述       : 任务删除时调用
*
* 参数       : ptcb是指向将被删除任务的任务控制块的指针。
*
* 注意       : 1) 调用期间中断被禁止
*********************************************************************************************************
*/
#if OS_TASK_DEL_EN > 0
void OSTaskDelHook (OS_TCB *ptcb) reentrant
{
    ptcb = ptcb;                       /* Prevent compiler warning                                     */
	
}
#endif

/*
*********************************************************************************************************
*                                          任务切换钩挂函数
*
* 描述       : 执行任务切换时调用。这允许你在上下文切换期间执行其它操作。
*
* 参数       : 无
*
* 注意       : 1) 调用期间中断被禁止
*              2) 假定全局指针'OSTCBHighRdy'已经指向了将要被换入的任务控制块(即:最高优先级任务)，并且
*                 'OSTCBCur'指向了将被换出的任务(即:当前任务)。
*********************************************************************************************************
*/
void OSTaskSwHook (void) reentrant
{
}

/*
*********************************************************************************************************
*                                          统计任务钩挂函数
*
* 描述       : 这个函数每秒钟被uC/OS-II统计任务调用。这么做使你的应用程序可以增加统计任务的功能。
*
* 注意       : 无
*********************************************************************************************************
*/
#if OS_TASK_STAT_EN > 0
void OSTaskStatHook (void) reentrant
{
}
#endif

/*
*********************************************************************************************************
*                                           OSTCBInit() HOOK
*
* Description: This function is called by OSTCBInit() after setting up most of the TCB.
*
* Arguments  : ptcb    is a pointer to the TCB of the task being created.
*
* Note(s)    : 1) Interrupts may or may not be ENABLED during this call.
*********************************************************************************************************
*/
#if OS_VERSION > 203
void OSTCBInitHook (OS_TCB *ptcb) reentrant
{
    ptcb = ptcb;                                           /* Prevent Compiler warning                 */
}
#endif

/*
*********************************************************************************************************
*                                          定时钩挂函数
*
* 描述       : 本函数每一滴答被调用一次。
*
* 参数       : 无
*
* 注意       : 1) 在本调用期间中断可以或不可以使能。
*********************************************************************************************************
*/
void OSTimeTickHook (void) reentrant
{
}

/*
*********************************************************************************************************
*                                             IDLE TASK HOOK
*
* Description: This function is called by the idle task.  This hook has been added to allow you to do  
*              such things as STOP the CPU to conserve power.
*
* Arguments  : none
*
* Note(s)    : 1) Interrupts are enabled during this call.
*********************************************************************************************************
*/
#if OS_VERSION >= 251
void OSTaskIdleHook (void) reentrant
{
}
#endif

#endif

/*
   使用C语言的中断处理函数有助与提高程序的移植性。建议中断程序不要太长，如果长则使用信号量来与任务同步，
   在外部任务中实现大量的处理。
   中断处理例程都放在下面。
*/

/*
void UserTickTimer(void)
{
    TH0=0xB8;    		//普通51定时器方式1，必须在发生中断时，重新赋值并再次启动计时
    TL0=0;            	//Tick=50次/秒(即0.02秒/次)，晶振11.0592M
    TR0=1;
}
 
//uCOS-II系统时钟中断处理程序
void OSTickISR(void) interrupt 1
{
    OSIntEnter();					// Must be called first at every hardware interrupt entry point 
    UserTickTimer();				// User functions can be called here.
   	OSTimeTick();					// Must be called during tick isr 
   	OSIntExit();					// Must be called finally at every hardware interupt exit point 
}
*/
/*
  设置硬件寄存器的初始值。
  初始化定时器0,作为ucOS-II的系统时钟。
  还有其他的与硬件相关的初始化也可以放在这里。
*/

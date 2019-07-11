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
* 文件名 : OS_CPU_A.ASM
* 作者   : Jean J. Labrosse
*********************************************************************************************************
*/

;伪指令详细用法请查A51.PDF文件
;程序结构详见《uC/OS-II》193-198页

;不用此语句！！！ $CASE    ;标号和变量名区分大小写

$NOMOD51
EA	BIT	0A8H.7
SP	DATA	081H
B	DATA	0F0H
ACC	DATA	0E0H
DPH	DATA	083H
DPL	DATA	082H
PSW	DATA	0D0H
TR0	BIT	088H.4
TH0	DATA	08CH
TL0	DATA	08AH

        NAME OS_CPU_A    ;模块名
        
;定义重定位段
?PR?OSStartHighRdy?OS_CPU_A    SEGMENT CODE
?PR?OSCtxSw?OS_CPU_A           SEGMENT CODE
?PR?OSIntCtxSw?OS_CPU_A        SEGMENT CODE
?PR?OSTickISR?OS_CPU_A         SEGMENT CODE
?PR?_?serial?OS_CPU_A          SEGMENT CODE
        
;声明引用全局变量和外部子程序
        EXTRN DATA  (?C_XBP)           ;仿真堆栈指针用于重入局部变量保存,为V2.51能被C使用定义在本模块中

        EXTRN IDATA (OSTCBCur)		   ;指向正在运行任务控制块的指针
        EXTRN IDATA (OSTCBHighRdy)	   ;指向最高优先级就绪任务块的指针
        EXTRN IDATA (OSRunning)		   ;系统是否运行标志
        EXTRN IDATA (OSPrioCur)		   ;正字运行任务的优先级
        EXTRN IDATA (OSPrioHighRdy)	   ;具有最高优先级别的就绪任务的优先级
  
        EXTRN CODE  (_?OSTaskSwHook)   ;系统任务切换钩子
        EXTRN CODE  (_?OSIntEnter)	   ;进入中断函数
        EXTRN CODE  (_?OSIntExit)	   ;退出中断函数
        EXTRN CODE  (_?OSTimeTick)     ;内核系统节拍函数      
        EXTRN CODE  (_?serial)		   ;串口中断函数
            
;对外声明4个不可重入函数   外部可使用
        PUBLIC OSStartHighRdy	  ;段名
        PUBLIC OSCtxSw			  ;段名
        PUBLIC OSIntCtxSw		  ;段名
        PUBLIC OSTickISR		  ;段名
        PUBLIC SerialISR          ;段名


;分配堆栈空间。只关心大小，堆栈起点由keil决定，通过标号可以获得keil分配的SP起点。
?STACK SEGMENT IDATA
        RSEG ?STACK
OSStack:
        DS 30H
OSStkStart IDATA OSStack-1

;定义压栈宏  子程序调用
PUSHALL     MACRO	
        	PUSH ACC
        	PUSH B
        	PUSH DPH
        	PUSH DPL
        	PUSH PSW
        	MOV  A,R0   ;R0-R7入栈
        	PUSH ACC
        	MOV  A,R1
       	 	PUSH ACC
        	MOV  A,R2
        	PUSH ACC
        	MOV  A,R3
        	PUSH ACC
        	MOV  A,R4
        	PUSH ACC
        	MOV  A,R5
        	PUSH ACC
        	MOV  A,R6
        	PUSH ACC
        	MOV  A,R7
        	PUSH ACC
        	;PUSH SP    ;不必保存SP，任务切换时由相应程序调整
        	ENDM
;定义出栈宏  子程序调用   
POPALL  	MACRO			 ;不必保存SP，任务切换时由相应程序调整	 R0-R7出栈   
        	POP  ACC    
        	MOV  R7,A
        	POP  ACC
        	MOV  R6,A
        	POP  ACC
        	MOV  R5,A
        	POP  ACC
        	MOV  R4,A
        	POP  ACC
        	MOV  R3,A
      	 	POP  ACC
       	 	MOV  R2,A
        	POP  ACC
        	MOV  R1,A
        	POP  ACC
        	MOV  R0,A
        	POP  PSW
        	POP  DPL
        	POP  DPH
        	POP  B
        	POP  ACC
        	ENDM
    
;子程序
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;OSStart()中调用，永远只执行一次
        RSEG ?PR?OSStartHighRdy?OS_CPU_A
OSStartHighRdy:
        USING 0    ; (1) 上电后51自动关中断，此处不必用CLR EA指令，因为到此处还未开中断，本程序退出后，开中断; 使用寄存器组0
        LCALL _?OSTaskSwHook    ; (2) 调用任务钩子，可以是空的

OSCtxSw_in:		   ;子程序模拟软中断
    
        ;OSTCBCur -> DPTR  获得当前TCB指针，详见C51.PDF第178页
        MOV  R0,#LOW (OSTCBCur) ; (3) 获得OSTCBCur指针低地址，指针占3字节。+0类型+1高8位数据+2低8位数据
        INC  R0					; 使用OSSTartHighRdy和OSTCBCur效果一样
        MOV  DPH,@R0            ; 获取需要恢复的最高优先级任务的堆栈指针
        INC  R0
        MOV  DPL,@R0			;DPTR 16数据指针 特殊功能寄存器  = DPH + DPL
    	
        ;OSTCBCur -> OSTCBStkPtr -> DPTR  获得最高优先级任务的堆栈指针	指针占3字节。+0类型+1高8位数据+2低8位数据
        INC  DPTR        		; (5)
        MOVX A,@DPTR            ; OSTCBStkPtr是void指针
        MOV  R0,A
        INC  DPTR
        MOVX A,@DPTR
        MOV  R1,A
        MOV  DPH,R0				 
        MOV  DPL,R1				 ;取出OSTCB中的用户堆栈指针地址到DPTR
    
        ;*UserStkPtr -> R5  用户堆栈起始地址内容(即用户堆栈长度放在此处)  
        MOVX A,@DPTR             ;(6) 用户堆栈中是unsigned char类型数据	 
        MOV  R5,A                ;R5 = 用户堆栈长度
    
        ;恢复现场堆栈内容
        MOV  R0,#OSStkStart		 ;(7)  系统堆栈起始地址	 R0 = OSStkStart
        
restore_stack: 					 ;将用户堆栈中的寄存器值推入系统栈
        INC  DPTR				 ;移动2字节长度
        INC  R0
        MOVX A,@DPTR
        MOV  @R0,A
        DJNZ R5,restore_stack	 ;R5 = 用户堆栈长度
    
        ;恢复堆栈指针SP			 指向系统栈栈顶
        MOV  SP,R0				 ;系统堆栈空间大小 = 任务堆栈空间大小 = SP - OSStkStart
    
        ;恢复仿真堆栈指针?C_XBP        
        INC  DPTR
        MOVX A,@DPTR
        MOV  ?C_XBP,A    ;?C_XBP 仿真堆栈指针高8位
        INC  DPTR
        MOVX A,@DPTR
        MOV  ?C_XBP+1,A  ;?C_XBP 仿真堆栈指针低8位
    
        ;系统运行标志 OSRunning = TRUE
        MOV  R0,#LOW (OSRunning)	;全局变量
        MOV  @R0,#01
    
        POPALL	   ;执行出栈宏，将系统栈中存储的内容恢复到寄存器，就好像刚发生过中断一样
        SETB EA    ;开中断
        RETI
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;函数实现任务级任务切换,1,手动保存CPU在中断来临时自动保存的寄存器(PC)；2，额外的寄存器
        RSEG ?PR?OSCtxSw?OS_CPU_A
OSCtxSw:    
        USING 0            ;默认使用R0~R7
		PUSHALL			   ;PC值已经压入系统堆栈，将剩余的寄存器寄压入系统栈
    
OSIntCtxSw_in:
    
        ;获得堆栈长度和起址
        MOV  A,SP
        CLR  C
        SUBB A,#OSStkStart
        MOV  R5,A     	   ;获得堆栈长度   R5 = SP - OSStkStart   
    
        ;OSTCBCur -> DPTR  获得当前TCB指针
        MOV  R0,#LOW (OSTCBCur) ;获得OSTCBCur指针低地址，指针占3字节。+0类型+1高8位数据+2低8位数据 u_COS_II.H中声明
        INC  R0
        MOV  DPH,@R0       ;全局变量OSTCBCur在IDATA中
        INC  R0
        MOV  DPL,@R0
    
        ;OSTCBCur->OSTCBStkPtr -> DPTR  获得用户（任务）堆栈指针
        INC  DPTR        ;指针占3字节。+0类型+1高8位数据+2低8位数据
        MOVX A,@DPTR     ;.OSTCBStkPtr是void指针
        MOV  R0,A
        INC  DPTR
        MOVX A,@DPTR
        MOV  R1,A
        MOV  DPH,R0
        MOV  DPL,R1
        
        ;保存堆栈长度
        MOV  A,R5
        MOVX @DPTR,A
    
        MOV  R0,#OSStkStart  ;获得堆栈起址
save_stack:					 ;保存当前系统堆栈内容到用户堆栈 
        INC  DPTR
        INC  R0
        MOV  A,@R0
        MOVX @DPTR,A
        DJNZ R5,save_stack
        
        ;保存仿真堆栈指针?C_XBP
        INC  DPTR					   ;(9)
        MOV  A,?C_XBP    ;?C_XBP 仿真堆栈指针高8位
        MOVX @DPTR,A
        INC  DPTR
        MOV  A,?C_XBP+1  ;?C_XBP 仿真堆栈指针低8位
        MOVX @DPTR,A        
    
        LCALL _?OSTaskSwHook			;(10)  任务交换钩子
        
        ;OSTCBCur = OSTCBHighRdy  将当前任务的指针指向要恢复运行的任务(优先级最高的任务)
        MOV  R0,#OSTCBCur				;(11)
		MOV  R1,#OSTCBHighRdy
		MOV  A,@R1
        MOV  @R0,A
        INC  R0
		INC  R1
		MOV  A,@R1
        MOV  @R0,A
        INC  R0
		INC  R1
		MOV  A,@R1
        MOV  @R0,A					   ;指针3个字节 操作3次
                
        ;OSPrioCur = OSPrioHighRdy 
        MOV  R0,#OSPrioCur			   ;(12)
		MOV  R1,#OSPrioHighRdy
		MOV  A,@R1
        MOV  @R0,A
        
        LJMP OSCtxSw_in					;模拟中断完成
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        RSEG ?PR?OSIntCtxSw?OS_CPU_A
        
OSIntCtxSw:

        ;调整SP指针去掉在调用OSIntExit(),OSIntCtxSw()过程中压入堆栈的多余内容
        ;SP=SP-4

        MOV  A,SP
        CLR  C
        SUBB A,#4
        MOV  SP,A
        
        LJMP OSIntCtxSw_in			    ;模拟中断完成
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;系统节拍处理
        CSEG AT 000BH    ;OSTickISR
        LJMP OSTickISR   ;使用定时器0
        RSEG ?PR?OSTickISR?OS_CPU_A

OSTickISR:        
        USING 0        
        PUSHALL
        
        CLR  TR0		 ;停止定时器0
        MOV  TH0,#0DCH   ;初始化定时器，Tick=100次/秒
        MOV  TL0,#00H    ;
        SETB TR0		 ;启动定时器0
        
        LCALL _?OSIntEnter   ;中断函数正在执行
        LCALL _?OSTimeTick	 ;调用
        LCALL _?OSIntExit	 ;中断函数已经退出

        POPALL        
        RETI
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;串口中断处理
        CSEG AT 0023H    
        LJMP SerialISR   ;串口系统功能部分
        RSEG ?PR?_?serial?OS_CPU_A
        
SerialISR:
        
        USING 0   
		CLR  EA        ;先关中断，以防中断嵌套
        PUSHALL	       ;保存所有寄存器
   		LCALL _?OSIntEnter
        LCALL _?serial   ;串口中断处理   
		LCALL _?OSIntExit
        POPALL        ;恢复所有寄存器
		SETB EA
        RETI
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
        END


#include  <app_cfg.h>
#include  <lib_mem.h>

#include  <cpu.h>
#include  <cpu_core.h>

#include  <os.h>
#include  <os_cfg_app.h>

#include "stm32f4xx.h"
#include "stdio.h"
#include "Queue_List.h"

#include "unity_fixture.h"
/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

OS_TCB        AppTaskStartTCB;
CPU_STK       AppTaskStartStk[2048u];



/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  AppTaskStart  (void *p_arg);
static  void RunAllTests(void);
static  int TestMain(int argc, const char* argv[]);
/*
*********************************************************************************************************
*                                               main()
*
* Description : Entry point for C code.
*
* Arguments   : none.
*
* Returns     : none.
*********************************************************************************************************
*/


int main(int argc, const char* argv[])
{
    OS_ERR os_err;
    
    CPU_Init();

    Mem_Init();

    OSInit(&os_err);

    OSTaskCreate((OS_TCB     *)&AppTaskStartTCB,                /* Create the start task                                    */
                 (CPU_CHAR   *)"App Task Start",
                 (OS_TASK_PTR ) AppTaskStart,
                 (void       *) 0,
                 (OS_PRIO     ) 5,
                 (CPU_STK    *)&AppTaskStartStk[0],
                 (CPU_STK     ) 0u,
                 (CPU_STK_SIZE) 1024u,
                 (OS_MSG_QTY  ) 0,
                 (OS_TICK     ) 0,
                 (void       *) 0,
                 (OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&os_err);



    OSStart(&os_err);

    return 0;
}


/*
*********************************************************************************************************
*                                           App_TaskStart()
*
* Description : Startup task example code.
*
* Arguments   : p_arg       Argument passed by 'OSTaskCreate()'.
*
* Returns     : none.
*
* Created by  : main().
*
* Notes       : (1) The ticker MUST be initialised AFTER multitasking has started.
*********************************************************************************************************
*/

int argc; 
const char* argv[10];
static  void  AppTaskStart (void *p_arg)
{
    OS_ERR os_err;
    CPU_INT32U cpu_clk_freq;
    CPU_INT32U cnts;
    
    SystemCoreClockUpdate();
	
    cpu_clk_freq = SystemCoreClock;
    cnts = cpu_clk_freq / OS_CFG_TICK_RATE_HZ;
    OS_CPU_SysTickInit(cnts);

    //UnityBegin("List queue test£º\n");
    TestMain(argc, argv);
    
#if 1	
    while(DEF_TRUE) {

        OSTimeDlyHMSM((CPU_INT16U) 0,
                      (CPU_INT16U) 0,
                      (CPU_INT16U) 1,
                      (CPU_INT32U) 0,
                      (OS_OPT    ) OS_OPT_TIME_HMSM_STRICT,
                      (OS_ERR   *)&os_err);

    }
#endif
}

static void RunAllTests(void)
{
    RUN_TEST_GROUP(ListQueueTest);
}

int TestMain(int argc, const char* argv[])
{
    return UnityMain(argc, argv, RunAllTests);
}

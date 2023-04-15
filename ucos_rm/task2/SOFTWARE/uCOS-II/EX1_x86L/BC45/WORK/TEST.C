/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/

#include "includes.h"
#include "stdio.h"
//#include "stdlib.h"
/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/

OS_STK        Task1_Stk[TASK_STK_SIZE]; 
OS_STK        Task2_Stk[TASK_STK_SIZE];
/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void  Task1();                       /* Function prototypes of tasks                  */
        void  Task2();
static  void  TaskStartCreateTasks(void);
        //void  Print(void);

		Node* ListPrintNode(Node *head);

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void  main (void)
{

	head = NULL;
    task_log_index = 0;
    printf("[Time]  [Event]   [From]  [To]\r\n");
    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */

    TaskStartCreateTasks();

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(OS_TICKS_PER_SEC);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();
    OSTimeSet(0);
    OSStart();                                             /* Start multitasking                       */
}


/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
/*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
        OSTaskCreate(Task1, (void *)0, &Task1_Stk[TASK_STK_SIZE], 1);
        OSTaskCreate(Task2, (void *)0, &Task2_Stk[TASK_STK_SIZE], 2);
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

void  Task1()
{
    int C = 1;
    int P = 3;
    int start;      /*The Start time*/
    int end;
    int toDelay;    
    OSTCBCur->compTime = C;
    OSTCBCur->period = P;
    start = 0;
    for (;;) {
           while(OSTCBCur->compTime > 0);
           end = OSTimeGet();
           toDelay = (OSTCBCur->period)-(end-start);
           start = start+(OSTCBCur->period);
           OSTCBCur->compTime=C;      
           OSTimeDly(toDelay);                               
    }
}

void  Task2()
{
  int C = 3;
  int P = 6;
  int start;
  int end;
  int toDelay;
  OSTCBCur->compTime = C;
  OSTCBCur->period = P;
  start = 0;
  for(;;){
      head = ListPrintNode(head); // Print out scheduling state & free list
      while(OSTCBCur->compTime > 0);
      end = OSTimeGet();
      toDelay = (OSTCBCur->period)-(end-start);
      start = start+(OSTCBCur->period);
      OSTCBCur->compTime = C;
      OSTimeDly(toDelay);
    }
}

Node* ListPrintNode(Node *head){
	Node *cur = head;
	Node *temp;
	int idx;
	if(head == NULL){
		return head;
	}
	while(cur != NULL){
	 	idx = cur->state;
	 	switch(idx)
	 	{
	 	 	case 1:
 	 	 	 	printf("%-10d%-16s%3d%9d\n", cur->ticks,"Preempt", cur->prio_cur, cur->prio_next);
	 	 	 	break;
 	 	 	case 2:
          	 	printf("%-10d%-16s%3d%9d\n", cur->ticks,"Complete", cur->prio_cur, cur->prio_next);
	 	 	 	break;
 	 	}
	 	cur = cur->next;
 	 	temp = head;
 	 	head = head->next;
	 	free(temp);
	}
	return head;
}


# 1092-Real-Time-embedded-systems
## Environment: Dosbox
Real Time Operation System: uCOS-II
Use command line：
```sh
~MOUNT c ~/ucos #(mount to your local file)
~C:
```
### Imglement project:
* Rate Monotonic Scheduling
* Earliest Deadline First Scheduling Algo
* Ceiling Priority Protocol(Resource)

### Rate Monotonic Scheduling (RM)
Purpose : To log the preempte and complete context switch.
1. Modify uCOS_II.H file 
2. Create list struct and global head node(baed on linklist to log data)
3. Add `OS_EXT *Node head`;
4. Modify OS_CORE.C file 
5. Add `OSTCBCur->compTime--` in `OS_TimeTick()` to consume time
6. Store complete and preempt data in `OSIntExit(void)` and `OS_Sched(void)` function call.

### Simulate 2 Task parameter setting:
1. Task 1: Prio = 1,compTime = 1,Period = 3.
2. Task 2: Prio = 2,compTime = 3,Period = 6.


![1](/image/ListInser.jpg)
![2](/image/ListPrint.jpg)

Simulation result:
![3](/image/rm_task2.jpg)

### Eaeliest Deadline First(EDF)
1. Modify original TCB data to add deadline data type.
2. Modify uCOS_II.H file protype `OS_TCBInit(…, INT32U compTime, INT32U deadline);`

In OS_TASK.C to add compTime and deadline
1. `INT8U OSTaskCreate(…, INT32 compTime, INT32U deadline)`
2. In `OSTaskCreateExt(…)` modify `OS_TCBInit(…,10,10)` to set default deadline & compTime.
3. `OSTaskCreate()` will use `OS_TCBInit()` function to setup TASK TCB.

In OS_CORE.C file
1. Modify `INT8U OS_TCBInit(..., INT32U compTime, INT32U deadline)`;
2. Add new TCB data compTime & deadline.
    `ptcb->compTime = compTime;`
    `ptcb->deadline = deadline;`
3. Modify `static void OS_InitTaskStat(void)` to disable `OSTaskCreate` function because it will bug.

In OS_CORE.C file
1. Modify `OSIntExit()` and `OSStart()` function to use `OS_SchededEDF()` function call; 
```c
static void OS_SchededEDF(void)
{
    INT8U prio = OS_LOWEST_PRIO;	// default prio 63 idle
    INT32U deadline_r = 1000000;	// deadline max 
    OS_TCB* cur = OSTCBList;		// pointer to OSTCBList
    while(cur->OSTCBNext){			
        if(cur->OSTCBDly == 0 && cur->deadline != NULL && cur->deadline < deadline_r) {
            prio = cur->OSTCBPrio;
            deadline_r = cur->deadline; // update search early deadline to compare
        }
        cur = cur->OSTCBNext;
    }
    OSPrioHighRdy = prio;
}
```
### Simulate 2&3 Task parameter setting:
1. Task1 compTime = 1, Period = 3  Task2 compTime = 3 Period = 5
2. Task1 compTime = 1, Period = 4  Task2 compTime = 2 Period = 5, Task3 compTime = 2 Period = 10

Simulation result:
![4](/image/edf_task2.jpg)
![5](/image/edf_task3.jpg)

### Ceiling Priority Protocol(CPP) resource
Purpose : Original Mutex algorithm is PIP (Priority Inheritance Priority) to change the CPP algorithm.

1. Modify uCOS_II.H file to add new Linklist Node 
2. Add new node struct data int resource index.
    OS_EXT        Mutex_store[2];
    OS_EXT 	*Node 		head2;
    Create ```Node* ListInsertResource(Node *head, int state, int ticks, int prio_cur, int prio_next)``` protype function.

In OS_MUTEX.C file
1. Modify ```OSMutexPend(OS_EVENT *pevent, INT16U timeout, INT8U *err)``` function call.
2. Modify ```OSMutexPost(OS_EVENT *pevent, INT16U timeout, INT8U *err)``` function call.
3. Because the 2 Mutex lock will lost original mutex priority so need to restore ```Mutex_store``` array. 

Simulate result:
![6](/image/cpp_task2.jpg)
![7](/image/cpp_task3.jpg)

























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

##### Rate Monotonic Scheduling (RM)
1. Modify uCOS_II.H file 
2. Create list struct and global head node(baed on linklist to log data)
3. Add `OS_EXT *Node head`;
4. Modify OS_CORE.C file 
5. Add `OSTCBCur->compTime--` in `OS_TimeTick()` to consume time
6. Store complete and preempt data in `OSIntExit(void)` and `OS_Sched(void)` function call.

Simulate 2 Task parameter setting:
Task 1: Prio = 1 compTime = 1 Period = 3
Task 2: Prio = 2 compTime = 3 Period = 6

Purpose : To log the preempte and complete context switch.
![1](/image/ListInser.jpg)
![2](/image/ListPrint.jpg)

Simulation result:
![3](/image/rm_task2.jpg)

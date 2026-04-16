#ifndef WDT_FUNCS_H
#define WDT_FUNCS_H

// Идентификаторы задач
enum
{
	WDT_TASK_MAIN = 0,
	WDT_TASK_RESERV = 1,
	WDT_TASK_UDP = 2,
	WDT_TASK_MODBUS_RTU_SLAVE = 3,
	WDT_TASK_TERMINAL = 4,
	WDT_TASK_LOCAL_IO = 5,
	WDT_TASK_DIAG = 6,
	WDT_TASK_HOT_RESTART = 7,
	WDT_TASK_ARCHIVE = 8,
	WDT_TASK_MODBUS_TCP_SLAVE = 9,
	WDT_TASK_FCGI = 10,
	WDT_TASK_UDPSEND = 11,
	WDT_TASK_UDPSEND2 = 12,
	WDT_TASK_FIRST_DYNAMIC = 13
	//    WDT_TASK_USER=WDT_TASK_HOT_RESTART + 1,					// QuanUserTasks штук
	//    WDT_TASK_SERIAL=WDT_TASK_USER+QuanUserTasks	// QuanSerialTasks штук
	//    WDT_TASK_OTHER_CONTROLLER=WDT_TASK_SERIAL+QuanSerialTasks,	//CountOtherControllerTasks штук
};

#define WDT_GROUP_USER_TASK	0
#define WDT_GROUP_SERIAL_TASK	1
#define WDT_GROUP_OTHER_CONTR_TASK	2

int WDT_Task_Init(void);
EXTERN_C void WDT_Register_Task(BYTE id);
EXTERN_C void WDT_Alive(BYTE id);
EXTERN_C void WDT_Register_Task_Group(BYTE group, BYTE id);
EXTERN_C void WDT_Alive_Group(BYTE group, BYTE id);
EXTERN_C int WDT_GetTaskIndex(BYTE group, BYTE id);

#endif

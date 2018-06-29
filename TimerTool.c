#include "MyTimer.h"
//#include <Windows.h>

const  int    gSleepTime = 3000; 
const  int    gMaxTaskNum = 10;
static int    gTimerThreadFlag = 0;
static struct DailyTaskST gMyTask[gMaxTaskNum];

DWORD WINAPI TimerThread(void* data);
time_t GetNextTime(int exeTime, eTimerCycle cycle);


void ResetTimerMain(){
	gTimerThreadFlag = 0;
	for(int i = 0; i < gMaxTaskNum; ++i){
		gMyTask[i].isWork = 0;
		gMyTask[i].hhmmss = 0;
		gMyTask[i].nextOffset = 0;
		gMyTask[i].cycle = eTimerCycle_Daily;
		gMyTask[i].handle = NULL;
		gMyTask[i].data = NULL;
	}
}

int SetTimerTask(int exeTime, eTimerCycle cycle, void(*fun)(void*), void* data){
	for(int i = 0; i < gMaxTaskNum; ++i){
		if(gMyTask[i].isWork == 0){
			gMyTask[i].isWork = 1;
			gMyTask[i].hhmmss = exeTime;
			gMyTask[i].cycle = cycle;
			gMyTask[i].nextOffset = GetNextTime(exeTime, cycle);
			gMyTask[i].handle = fun;
			gMyTask[i].data = data;
			return i;
		}
	}
	return -1;
}

int CloseTimerTask(int taskNum){
	if(taskNum < 0 || taskNum >= gMaxTaskNum){
		return -1;
	}
	if(gMyTask[taskNum].isWork == 1){
		gMyTask[taskNum].isWork = 0;
		return 0;
	}
	return -2;
}

int TimerMain(){
	if(gTimerThreadFlag == 0){
		CreateThread(NULL, 0, TimerThread, NULL, 0, NULL);
		return 0;
	}
	return -1;
}

DWORD WINAPI TimerThread(void* data){
	time_t current;
	pDailyTaskST task;

	//重新初始化一次時間
	for(int i = 0; i < gMaxTaskNum; ++i){
		task = &gMyTask[i];
		if(task->isWork == 1){
			task->nextOffset = GetNextTime(task->hhmmss, task->cycle);
		}		
	}

	//開啟thread
	gTimerThreadFlag = 1;
	while(gTimerThreadFlag){
		time(&current);
		for(int i = 0; i < gMaxTaskNum; ++i){
			task = &gMyTask[i];
			if(task->isWork == 1 && current >task->nextOffset){
				task->handle(task->data);
				task->nextOffset = GetNextTime(task->hhmmss, task->cycle);
			}
		}
		Sleep(gSleepTime);
	}
	
	return 0;
}



time_t GetNextTime(int exeTime, eTimerCycle cycle){
	int totalSec = 0;
	SYSTEMTIME stLocal;
	GetLocalTime(&stLocal);

	if(cycle == eTimerCycle_Daily){
		totalSec = 60 * 60 * ((exeTime/10000)%100 - stLocal.wHour) + 60 * ((exeTime/100)%100 - stLocal.wMinute) +  (exeTime % 100 - stLocal.wSecond);
		if(totalSec < 0){
			totalSec += (24 * 60 * 60);
		}
	}else if(cycle == eTimerCycle_Hourly){
		totalSec = 60 * ((exeTime/100)%100 - stLocal.wMinute) +  (exeTime % 100 - stLocal.wSecond);
		if(totalSec < 0){
			totalSec += (60 * 60);
		}
	}else{
		totalSec = (exeTime % 100 - stLocal.wSecond);
		if(totalSec < 0){
			totalSec += (60);
		}	
	}
	return time(NULL) + totalSec;	
}

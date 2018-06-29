#pragma once
#include <Windows.h>
#include <time.h>

enum eTimerCycle{eTimerCycle_Daily, eTimerCycle_Hourly, eTimerCycle_PerMinute};

typedef struct DailyTaskST{
	int isWork;
	int hhmmss; //ex:1750
	eTimerCycle cycle;
	time_t nextOffset;
	void(*handle)(void*);	
	void* data;
}*pDailyTaskST;


void ResetTimerMain();
int  SetTimerTask(int time, eTimerCycle cycle, void(*fun)(void*), void* data);
int  CloseTimerTask(int taskNum);
int  TimerMain();

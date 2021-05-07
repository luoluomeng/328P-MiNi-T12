#ifndef _SLEEP_TIMER_H_
#define _SLEEP_TIMER_H_

void checkSleep();

void processSleep();

void processStopHeat();
bool isMoved();
//********** 系统休眠判断决策 **********
void processSystemSleep();
void cancelSleep();

void triggerSleep();

void stopHeat();
#endif
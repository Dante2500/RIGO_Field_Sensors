#include "timer_functions.h"

//TIMER VARIABLES
hw_timer_t *timer = NULL;
volatile bool flag = false;
volatile bool toggle = false;
//const uint16_t interval = 4000; //en milisegundos

void InitializeTimers(uint16_t interval){
  timer = timerBegin(0,80,true);
  timerAttachInterrupt(timer, &Finished_Interval, true);
  timerAlarmWrite(timer, 1000*interval, true);
  timerAlarmEnable(timer);
}


void IRAM_ATTR Finished_Interval() {
  flag = true;
  toggle ^=true;
}
#ifndef TIMER_FUNCTIONS_H
#define TIMER_FUNCTIONS_H

#include <Arduino.h>


void InitializeTimers(uint16_t interval);
void IRAM_ATTR Finished_Interval();


#endif 
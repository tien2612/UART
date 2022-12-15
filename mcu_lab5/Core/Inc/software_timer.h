/*
 * software_timer.h
 *
 *  Created on: Sep 25, 2022
 *      Author: nguye
 */

#ifndef INC_SOFTWARE_TIMER_H_
#define INC_SOFTWARE_TIMER_H_

#define	DURATION_1S					1000
#define DURATION_HALF_OF_SECOND 	500
extern int timer1_flag;
extern int timer2_flag;
extern int timer3_flag;
extern int timer4_flag;
extern int timer5_flag;
extern int timer6_flag;
extern int TIMER_CYCLE;

extern int timer0_mode_debounce_flag;
extern int timer0_mode_debounce_counter;

void set_Timer_mode_debounce(int duration);
void setTimer1(int duration);
void setTimer2(int duration);
void setTimer3(int duration);
void setTimer4(int duration);
void setTimer5(int duration);
void setTimer6(int duration);
void timerRun();
void clearTimer1();
#endif /* INC_SOFTWARE_TIMER_H_ */

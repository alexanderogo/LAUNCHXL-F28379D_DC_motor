/*
 * _timer.h
 *
 *  Created on: 20 ���. 2018 �.
 *      Author: Alexander
 */

#ifndef TIMER_H_
#define TIMER_H_

__interrupt void cpu_timer0_isr(void);

void initTIMER0(void);

#endif /* TIMER_H_ */

/*
 * _current_loop.h
 *
 *  Created on: 17 ���. 2018 �.
 *      Author: Alexander
 */

#ifndef CURRENT_LOOP_H_
#define CURRENT_LOOP_H_

#define FIR_ORDER_CURR           (SIZE_CURR_BURST - 1)
#define FPU_FIR_ORDER_CURR       (SIZE_CURR_BURST - 1)
void init_current_filters(void);

#endif /* CURRENT_LOOP_H_ */

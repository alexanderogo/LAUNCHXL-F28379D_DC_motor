/*
 * _dac.h
 *
 *  Created on: 20 но€б. 2018 г.
 *      Author: Alexander
 */

#ifndef DAC_H_
#define DAC_H_

#include "F28x_Project.h"

void InitDAC(void);
void InitDACx(volatile struct DAC_REGS *DacxRegs);

#endif /* DAC_H_ */

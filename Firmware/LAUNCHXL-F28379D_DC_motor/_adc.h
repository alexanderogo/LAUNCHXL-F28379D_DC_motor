/*
 * _adc.h
 *
 *  Created on: 7 џэт. 2019 у.
 *      Author: Alexander
 */

#ifndef ADC_H_
#define ADC_H_

#include "F28x_Project.h"

__interrupt void dmach1_isr(void);
__interrupt void dmach2_isr(void);
__interrupt void dmach3_isr(void);

void InitADCforCurrents(void);
void InitADCforForces(void);

#endif /* ADC_H_ */

/*
 * _adc.h
 *
 *  Created on: 7 џэт. 2019 у.
 *      Author: Alexander
 */

#ifndef ADC_H_
#define ADC_H_

#include "F28x_Project.h"

//SOCx Trigger Source Select (TRIGSEL) bit

#define ADC_TRIGSEL_SOFTWARE        0
#define ADC_TRIGSEL_CPU1_TINT0      1
#define ADC_TRIGSEL_CPU1_TINT1      2
#define ADC_TRIGSEL_CPU1_TINT2      3
#define ADC_TRIGSEL_GPIO_ADCEXTSOC  4
#define ADC_TRIGSEL_EPWM1_ADCSOCA   5
#define ADC_TRIGSEL_EPWM1_ADCSOCB   6
#define ADC_TRIGSEL_EPWM2_ADCSOCA   7
#define ADC_TRIGSEL_EPWM2_ADCSOCB   8
#define ADC_TRIGSEL_EPWM3_ADCSOCA   9
#define ADC_TRIGSEL_EPWM3_ADCSOCB   10
#define ADC_TRIGSEL_EPWM4_ADCSOCA   11
#define ADC_TRIGSEL_EPWM4_ADCSOCB   12
#define ADC_TRIGSEL_EPWM5_ADCSOCA   13
#define ADC_TRIGSEL_EPWM5_ADCSOCB   14
#define ADC_TRIGSEL_EPWM6_ADCSOCA   15
#define ADC_TRIGSEL_EPWM6_ADCSOCB   16
#define ADC_TRIGSEL_EPWM7_ADCSOCA   17
#define ADC_TRIGSEL_EPWM7_ADCSOCB   18
#define ADC_TRIGSEL_EPWM8_ADCSOCA   19
#define ADC_TRIGSEL_EPWM8_ADCSOCB   20
#define ADC_TRIGSEL_EPWM9_ADCSOCA   21
#define ADC_TRIGSEL_EPWM9_ADCSOCB   22
#define ADC_TRIGSEL_EPWM10_ADCSOCA  23
#define ADC_TRIGSEL_EPWM10_ADCSOCB  24
#define ADC_TRIGSEL_EPWM11_ADCSOCA  25
#define ADC_TRIGSEL_EPWM11_ADCSOCB  26
#define ADC_TRIGSEL_EPWM12_ADCSOCA  27
#define ADC_TRIGSEL_EPWM12_ADCSOCB  28
#define ADC_TRIGSEL_CPU2_TINT0      29
#define ADC_TRIGSEL_CPU2_TINT1      30
#define ADC_TRIGSEL_CPU2_TINT2      31

__interrupt void dmach1_isr(void);
__interrupt void dmach2_isr(void);
__interrupt void dmach3_isr(void);
__interrupt void dmach4_isr(void);

void InitADCforCurrents(void);
void InitADCforForces(void);
void InitADCforVoltages(void);

#endif /* ADC_H_ */

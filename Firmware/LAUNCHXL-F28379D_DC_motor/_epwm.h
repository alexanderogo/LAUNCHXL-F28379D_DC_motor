/*
 * _epwm.h
 *
 *  Created on: 10 но€б. 2018 г.
 *      Author: Alexander
 */

#include "F28x_Project.h"
#include <stdint.h>

#ifndef EPWM_H_
#define EPWM_H_

#define EPWM_TBPRD                  500U         // PWM period 50 sempl div 4 and 25

__interrupt void epwm1_tzint_isr(void);
__interrupt void epwm3_int_isr(void);

void InitGpioTZ(void);
void InitTZ(void);

void InitGpioGS(void);

void initEPWM(void);
void initEPWM1(void); //example
void InitGpioEPWM1(void);
void InitGpioEPWM2(void);
void InitGpioEPWM3(void);
void InitGpioEPWM4(void);
void InitGpioEPWM5(void);
void InitGpioEPWM6(void);
void InitGpioEPWM8(void);
void initEPWMxMODE(volatile struct EPWM_REGS *EPwmxRegs, uint16_t epwm_period);
void initEPWMxAB(volatile struct EPWM_REGS *EPwmxRegs);
void initEPWMxA(volatile struct EPWM_REGS *EPwmxRegs);
void initEPWMxB(volatile struct EPWM_REGS *EPwmxRegs);
void initEPWMxInt(volatile struct EPWM_REGS *EPwmxRegs);
void initEPWMxSyncIn(volatile struct EPWM_REGS *EPwmxRegs, uint16_t tbphs);
void initEPWMxSyncOut(volatile struct EPWM_REGS *EPwmxRegs);
void initEPWMxASyncSOC(volatile struct EPWM_REGS *EPwmxRegs);
void initEPWMxBSyncSOC(volatile struct EPWM_REGS *EPwmxRegs);

//inline void EPWMxA1Ph1PinOut(volatile struct EPWM_REGS *EPwmxRegs, int32_t out);
//inline void fEPWMxA1Ph1PinOut(volatile struct EPWM_REGS *EPwmxRegs, float out);
//inline void EPWMxB1Ph1PinOut(volatile struct EPWM_REGS *EPwmxRegs, int32_t out);
//inline void fEPWMxB1Ph1PinOut(volatile struct EPWM_REGS *EPwmxRegs, float out);
//inline void EPWMx1Ph2PinOut(volatile struct EPWM_REGS *EPwmxRegs, int32_t out);
//inline void fEPWMx1Ph2PinOut(volatile struct EPWM_REGS *EPwmxRegs, float out);
//inline void EPWMx2Ph2PinOut(volatile struct EPWM_REGS *EPwmxRegs, int32_t out);
//inline void fEPWMx2Ph2PinOut(volatile struct EPWM_REGS *EPwmxRegs, float out);
//inline void EPWMx2Ph2PinOutInv(volatile struct EPWM_REGS *EPwmxRegs, int32_t out);
//inline void fEPWMx2Ph2PinOutInv(volatile struct EPWM_REGS *EPwmxRegs, float out);
//inline void EPWMx2Ph4PinOut(volatile struct EPWM_REGS *EPwmxRegs, volatile struct EPWM_REGS *EPwmyRegs, int32_t out);
//inline void fEPWMx2Ph4PinOut(volatile struct EPWM_REGS *EPwmxRegs, volatile struct EPWM_REGS *EPwmyRegs, float out);
//inline void EPWMx2Ph4PinOutInv(volatile struct EPWM_REGS *EPwmxRegs, volatile struct EPWM_REGS *EPwmyRegs, int32_t out);
//inline void fEPWMx2Ph4PinOutInv(volatile struct EPWM_REGS *EPwmxRegs, volatile struct EPWM_REGS *EPwmyRegs, float out);

inline float fsgn(float x)
{
    return (x < 0) ? -1 : (x > 0);
}

inline int16_t sgn(int16_t x)
{
    return (x < 0) ? -1 : (x > 0);
}

inline int32_t lsgn(int32_t x)
{
    return (x < 0) ? -1 : (x > 0);
}

inline void EPWMxA1Ph1PinOut(volatile struct EPWM_REGS *EPwmxRegs, int32_t out)
{
    out = (out <= UINT16_MAX) ? out : UINT16_MAX;
    EPwmxRegs->CMPA.bit.CMPA = (out >= 0) ? out : 0;
}

inline void fEPWMxA1Ph1PinOut(volatile struct EPWM_REGS *EPwmxRegs, float out)
{
    out = (out <= 1.0f) ? out : 1.0f;
    out = (out >= 0.0f) ? out : 0.0f;
    EPwmxRegs->CMPA.bit.CMPA = out*EPWM_TBPRD;
}

inline void EPWMxB1Ph1PinOut(volatile struct EPWM_REGS *EPwmxRegs, int32_t out)
{
    out = (out <= UINT16_MAX) ? out : UINT16_MAX;
    EPwmxRegs->CMPB.bit.CMPB = (out >= 0) ? out : 0;
}

inline void fEPWMxB1Ph1PinOut(volatile struct EPWM_REGS *EPwmxRegs, float out)
{
    out = (out <= 1.0f) ? out : 1.0f;
    out = (out >= 0.0f) ? out : 0.0f;
    EPwmxRegs->CMPB.bit.CMPB = out*EPWM_TBPRD;
}

inline void EPWMx1Ph2PinOut(volatile struct EPWM_REGS *EPwmxRegs, int32_t out)
{
    out = (out <= UINT16_MAX) ? out : UINT16_MAX;
    EPwmxRegs->CMPA.bit.CMPA = (out >= 0) ? out : 0;
}

inline void fEPWMx1Ph2PinOut(volatile struct EPWM_REGS *EPwmxRegs, float out)
{
    out = (out <= 1.0f) ? out : 1.0f;
    out = (out >= 0.0f) ? out : 0.0f;
    EPwmxRegs->CMPA.bit.CMPA = out*EPWM_TBPRD;
}

inline void EPWMx2Ph2PinOut(volatile struct EPWM_REGS *EPwmxRegs, int32_t out)
{
    out = (out <= UINT16_MAX) ? out : UINT16_MAX;
    out = (out > - UINT16_MAX) ? out : - UINT16_MAX;
    if (out >= 0) {
        EPwmxRegs->CMPB.bit.CMPB = 0;
        EPwmxRegs->CMPA.bit.CMPA = out;
    } else {
        EPwmxRegs->CMPA.bit.CMPA = 0;
        EPwmxRegs->CMPB.bit.CMPB = - out;
    }
//    (out > 0) ? (EPwmxRegs->CMPB.bit.CMPB = 0, EPwmxRegs->CMPA.bit.CMPA = out) : (EPwmxRegs->CMPA.bit.CMPA = 0, EPwmxRegs->CMPB.bit.CMPB = - out);
}

inline void fEPWMx2Ph2PinOut(volatile struct EPWM_REGS *EPwmxRegs, float out)
{
    out = (out <= 1.0f) ? out : 1.0f;
    out = (out >= - 1.0f) ? out : - 1.0f;
    if (out >= 0) {
        EPwmxRegs->CMPB.bit.CMPB = 0;
        EPwmxRegs->CMPA.bit.CMPA = out*EPWM_TBPRD;
    } else {
        EPwmxRegs->CMPA.bit.CMPA = 0;
        EPwmxRegs->CMPB.bit.CMPB = - out*EPWM_TBPRD;
    }

}

inline void EPWMx2Ph2PinOutInv(volatile struct EPWM_REGS *EPwmxRegs, int32_t out)
{
    out = (out <= UINT16_MAX) ? out : UINT16_MAX;
    out = (out > - UINT16_MAX) ? out : - UINT16_MAX;
    if (out >= 0) {
        EPwmxRegs->CMPA.bit.CMPA = EPWM_TBPRD;
        EPwmxRegs->CMPB.bit.CMPB = EPWM_TBPRD - out;
    } else {
        EPwmxRegs->CMPB.bit.CMPB = EPWM_TBPRD;
        EPwmxRegs->CMPA.bit.CMPA = EPWM_TBPRD + out;
    }
}

inline void fEPWMx2Ph2PinOutInv(volatile struct EPWM_REGS *EPwmxRegs, float out)
{
//    out = (out <= 1.0f) ? out : 1.0f;
//    out = (out >= - 1.0f) ? out : - 1.0f;
    __fsat(out, 1.0f, -1.0f);
//    if (out >= 0) {
//        EPwmxRegs->CMPA.bit.CMPA = EPWM_TBPRD;
//        EPwmxRegs->CMPB.bit.CMPB = (1.0f - out)*EPWM_TBPRD;
//    } else {
//        EPwmxRegs->CMPB.bit.CMPB = EPWM_TBPRD;
//        EPwmxRegs->CMPA.bit.CMPA = (1.0f + out)*EPWM_TBPRD;
//    }
    uint16_t epwm_tbprd = EPwmxRegs->TBPRD;
    if (out >= 0) {
        EPwmxRegs->CMPA.bit.CMPA = epwm_tbprd;
        EPwmxRegs->CMPB.bit.CMPB = (1.0f - out)*epwm_tbprd;
    } else {
        EPwmxRegs->CMPB.bit.CMPB = epwm_tbprd;
        EPwmxRegs->CMPA.bit.CMPA = (1.0f + out)*epwm_tbprd;
    }
}

inline void EPWMx2Ph4PinOut(volatile struct EPWM_REGS *EPwmxRegs, volatile struct EPWM_REGS *EPwmyRegs, int32_t out)
{
    out = (out <= UINT16_MAX) ? out : UINT16_MAX;
    out = (out > - UINT16_MAX) ? out : - UINT16_MAX;
    if (out >= 0) {
        EPwmyRegs->CMPA.bit.CMPA = 0;
        EPwmxRegs->CMPA.bit.CMPA = out;
    } else {
        EPwmxRegs->CMPA.bit.CMPA = 0;
        EPwmyRegs->CMPA.bit.CMPA = - out;
    }
}

inline void fEPWMx2Ph4PinOut(volatile struct EPWM_REGS *EPwmxRegs, volatile struct EPWM_REGS *EPwmyRegs, float out)
{
    out = (out <= 1.0f) ? out : 1.0f;
    out = (out >= - 1.0f) ? out : - 1.0f;
    if (out >= 0) {
        EPwmyRegs->CMPA.bit.CMPA = 0;
        EPwmxRegs->CMPA.bit.CMPA = out*EPWM_TBPRD;
    } else {
        EPwmxRegs->CMPA.bit.CMPA = 0;
        EPwmyRegs->CMPA.bit.CMPA = - out*EPWM_TBPRD;
    }
}

inline void EPWMx2Ph4PinOutInv(volatile struct EPWM_REGS *EPwmxRegs, volatile struct EPWM_REGS *EPwmyRegs, int32_t out)
{
    out = (out <= UINT16_MAX) ? out : UINT16_MAX;
    out = (out > - UINT16_MAX) ? out : - UINT16_MAX;
    if (out >= 0) {
        EPwmyRegs->CMPA.bit.CMPA = EPWM_TBPRD;
        EPwmxRegs->CMPA.bit.CMPA = EPWM_TBPRD - out;
    } else {
        EPwmxRegs->CMPA.bit.CMPA = EPWM_TBPRD;
        EPwmyRegs->CMPA.bit.CMPA = EPWM_TBPRD +- out;
    }
}

inline void fEPWMx2Ph4PinOutInv(volatile struct EPWM_REGS *EPwmxRegs, volatile struct EPWM_REGS *EPwmyRegs, float out)
{
    out = (out <= 1.0f) ? out : 1.0f;
    out = (out >= - 1.0f) ? out : - 1.0f;
    if (out >= 0) {
        EPwmyRegs->CMPA.bit.CMPA = EPWM_TBPRD;
        EPwmxRegs->CMPA.bit.CMPA = (1.0f - out)*EPWM_TBPRD;
    } else {
        EPwmxRegs->CMPA.bit.CMPA = EPWM_TBPRD;
        EPwmyRegs->CMPA.bit.CMPA = (1.0f + out)*EPWM_TBPRD;
    }
}





#endif /* EPWM_H_ */

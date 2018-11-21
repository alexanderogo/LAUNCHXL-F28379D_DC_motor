//#############################################################################
//
// FILE:   empty_bitfield_driverlib_main.c
//
// TITLE:  Empty Example
//
// Empty Bit-Field & Driverlib Example
//
// This example is an empty project setup for Bit-Field and Driverlib 
// development.
//
//#############################################################################
// $TI Release: F2837xD Support Library v3.05.00.00 $
// $Release Date: Thu Oct 18 15:48:42 CDT 2018 $
// $Copyright:
// Copyright (C) 2013-2018 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//#############################################################################

//
// Included Files
//
#include "F28x_Project.h"
#include "driverlib.h"
#include "device.h"
#include "_led.h"
#include "_epwm.h"
#include "_dac.h"
#include <stdint.h>

//
// Main
//

//#define EPWM_TBPRD                  500U         // PWM period 50 sempl div 4 and 25

//
// External Function Prototypes
//



//
// Function Prototypes
//

void initEPWM(void);
void initEPWM1(void); //example
void InitGpioEPWM1(void);
void InitGpioEPWM2(void);
void InitGpioEPWM3(void);
void InitGpioEPWM4(void);
void InitGpioEPWM5(void);
void InitGpioEPWM6(void);
void InitGpioEPWM8(void);
void initEPWMxMODE(volatile struct EPWM_REGS *EPwmxRegs);
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

void InitTZ(void);
void InitGpioTZ(void);
void InitGpioLED(void);
void InitGpioUsed(void);

//
// Interrupt Function Prototypes
//

//
// Globals
//

int64_t i = 0;
float freq_cpu_res = 0;
uint16_t gpio8 = 1;
uint16_t gpio9 = 1;

volatile float pwm1 = 0.1;
volatile float pwm2 = 0.2;
volatile float pwm3 = 0.3;
volatile float pwm4 = 0.4;

void main(void)
{
    InitSysCtrl();
    InitGpio();
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();
    //
    // Stop the ePWM clock
    //
    EALLOW;
    PieVectTable.EPWM1_TZ_INT = &epwm1_tzint_isr;
//    PieVectTable.EPWM1_TZ_INT = &EPWM1_TZ_ISR;
    EDIS;
    //
    // Stop the ePWM clock
    //
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;
    EDIS;
    //
    // Call the set up function for ePWM 2
    //
    InitTZ();
    initEPWM();
    InitGpioLED();
    InitDAC();
    InitGpioUsed();
    //
    // Start the ePWM clock
    //
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
    //
    // Enable CPU INT which is connected to chosen INT:
    //
    IER |= M_INT2;
    //
    // Enable chosen interrupts
    //
    PieCtrlRegs.PIEIER2.bit.INTx1 = 1;
    PieCtrlRegs.PIEIER2.bit.INTx2 = 1;
    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;
    //
    // IDLE loop. Just sit and loop forever (optional):
    //

    if (ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV == 0) {
        freq_cpu_res = 10*(ClkCfgRegs.SYSPLLMULT.bit.IMULT + 0.25*ClkCfgRegs.SYSPLLMULT.bit.FMULT) / (2*ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV + 1);
    }
    else {
        freq_cpu_res = 10*(ClkCfgRegs.SYSPLLMULT.bit.IMULT + 0.25*ClkCfgRegs.SYSPLLMULT.bit.FMULT) / (2*ClkCfgRegs.SYSCLKDIVSEL.bit.PLLSYSCLKDIV);
    }

    for (;;) {
        i++;
        DELAY_US(1e+3);
        fEPWMx2Ph2PinOutInv(&EPwm1Regs, pwm1);
        fEPWMx2Ph2PinOutInv(&EPwm2Regs, pwm2);
        fEPWMx2Ph2PinOutInv(&EPwm3Regs, pwm3);
//        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
//        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
//        GpioDataRegs.GPASET.bit.GPIO14 = 1;
//        GpioDataRegs.GPASET.bit.GPIO15 = 1;
        GpioDataRegs.GPATOGGLE.bit.GPIO14 = 1;
        GpioDataRegs.GPATOGGLE.bit.GPIO15 = 1;
//        GpioDataRegs.GPACLEAR.bit.GPIO14 = 1;
//        GpioDataRegs.GPACLEAR.bit.GPIO15 = 1;
        DacaRegs.DACVALS.all = ~(DacaRegs.DACVALS.all);
        DacbRegs.DACVALS.all = ~(DacbRegs.DACVALS.all);
        LED_BLUE_toggle();

    }
}

void InitGpioUsed(void)
{
    EALLOW;
    //
    // Initialization GPIOs as outputs
    //
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPASET.bit.GPIO14 = 1;     // Load output latch
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO14 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPASET.bit.GPIO15 = 1;     // Load output latch
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO15 = 1;     // GPIOXX is output

    EDIS;
}



//
// End of File
//

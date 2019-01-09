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
#include "_globals.h"
#include "_timer.h"
#include "_current_loop.h"
#include "_spi.h"
#include "_adc.h"
/*
extern volatile uint16_t DMA_Buf[2*CURR_TRFER_SZ*SIZE_CURR_BURST];
extern volatile uint16_t *currents;
extern volatile uint16_t *current1;
extern volatile uint16_t *current2;

extern volatile uint16_t *DMADest;
extern volatile uint16_t *DMASource;
*/
//#define SIZE_CURR_BURST       0x19U   //0x19U == 25U

//
// Main
//

//#define EPWM_TBPRD                  500U         // PWM period 50 sempl div 4 and 25

//#pragma DATA_SECTION(DMA_Buf, "ramgs0");    // map the TX data to memory
//volatile Uint16 DMA_Buf[4*SIZE_CURR_BURST];
//volatile Uint16 *currents = DMA_Buf;
//
//volatile Uint16 *DMADest;
//volatile Uint16 *DMASource;

// t_asc = 67ns -> ACQPS+1 > 13.4 -> ACQPS >= 13
// t_lat = 43 (pre = 6) -> ACQPS = 36 and 4000/(36+1+43) = 50 sample per pwm cycle (50kHz)
// t_lat = 59 (pre = 9) -> ACQPS = 20 and 4000/(20+1+59) = 50 sample per pwm cycle (50kHz)

//
// External Function Prototypes
//



//
// Function Prototypes
//

void InitGpioUsed(void);

//__interrupt void dmach1_isr(void);
//__interrupt void dmach2_isr(void);
//
//void InitADC(volatile struct ADC_REGS *AdcxRegs, uint16_t channel1, uint16_t channel2);
//void InitDMAforADCa(volatile struct ADC_RESULT_REGS *AdcxResultRegs);
//void InitDMAforADCb(volatile struct ADC_RESULT_REGS *AdcxResultRegs);


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
//uint32_t cntdma = 0;

volatile float pwm1 = 0.1;
volatile float pwm2 = 0.2;
volatile float pwm3 = 0.3;
volatile float pwm4 = 0.4;

volatile int16_t DacaVal = 1024;
volatile int16_t DacbVal = 2048;

extern uint16_t angl_get_arr[4];

void main(void)
{
    InitSysCtrl();
    InitGpio();
    DINT;
    InitPieCtrl();
    IER = 0x0000;
    IFR = 0x0000;
    InitPieVectTable();

    init_current_filters();
    init_current_PID();

    //
    // Stop the ePWM clock
    //
    EALLOW;
//    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    PieVectTable.EPWM1_TZ_INT = &epwm1_tzint_isr;
//    PieVectTable.EPWM1_TZ_INT = &EPWM1_TZ_ISR;
    PieVectTable.EPWM3_INT = &epwm3_int_isr;
//    PieVectTable.DMA_CH2_INT= &dmach2_isr;
//    PieVectTable.DMA_CH3_INT= &dmach3_isr;
    PieVectTable.SPIA_RX_INT = &spia_rx_int_isr;
    PieVectTable.SPIA_TX_INT = &spia_tx_int_isr;
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
    InitGpioUsed();
    GpioDataRegs.GPASET.bit.GPIO14 = 1;
    GpioDataRegs.GPASET.bit.GPIO15 = 1;
    InitTZ();
    initEPWM();
    InitGpioGS();
    InitGpioLED();
    InitDAC();
    DacaRegs.DACVALS.all = DacaVal;
    DacbRegs.DACVALS.all = DacbVal;
    DELAY_US(1e+3);
    DMAInitialize();
    InitADCforCurrents();
    InitADCforForces();
//    InitADC(&AdcbRegs, 2, 3);
//    DMAInitialize();
//    InitDMAforADCb(&AdcbResultRegs);
//    initTIMER0();
    InitSPIA();

    //
    // Start the ePWM clock
    //
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
    //
    // Enable CPU INT which is connected to chosen INT:
    //
    IER |= (M_INT1 | M_INT2 | M_INT3 | M_INT6 | M_INT7);
    //
    // Enable chosen interrupts
    //
//    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;  // Enable TINT0 in the PIE: Group 1 __interrupt 7
    PieCtrlRegs.PIEIER2.bit.INTx1 = 1;  // 2.1 - ePWM1 Trip Zone Interrupt
    PieCtrlRegs.PIEIER2.bit.INTx2 = 1;  // 2.2 - ePWM2 Trip Zone Interrupt
    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;  // 3.3 - ePWM3 Interrupt
    PieCtrlRegs.PIEIER6.bit.INTx1 = 1;  // 6.1 - SPIA Receive Interrupt
    PieCtrlRegs.PIEIER6.bit.INTx2 = 1;  // 6.2 - SPIA Transmit Interrupt
//    PieCtrlRegs.PIEIER7.bit.INTx2 = 1;  // DMA2
//    PieCtrlRegs.PIEIER7.bit.INTx3 = 1;  // DMA2


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
    DacaRegs.DACVALS.all = 1024;
    DacbRegs.DACVALS.all = 2048;
    for (;;) {
        i++;
        DELAY_US(1e+3);
        fEPWMx2Ph2PinOutInv(&EPwm1Regs, pwm1);
        fEPWMx2Ph2PinOutInv(&EPwm2Regs, pwm2);
        fEPWMx2Ph2PinOutInv(&EPwm3Regs, pwm3);
//        DacaVal = (DacaVal + 1)%4096;
//        DacbVal = (DacbVal + 1)%4096;
        DacaRegs.DACVALS.all = DacaVal;
        DacbRegs.DACVALS.all = DacbVal;
        LED_BLUE_toggle();

//        CS1_SPIA_on();
//        SpiaRegs.SPITXBUF = 0x7FFE;
//        DELAY_US(1e+3);
//        CS1_SPIA_off();
//        angl_get_arr[0] = (SpiaRegs.SPIRXBUF & 0x3FFF);
//
//        CS2_SPIA_on();
//        SpiaRegs.SPITXBUF = 0x7FFE;
//        DELAY_US(1e+3);
//        CS2_SPIA_off();
//        angl_get_arr[1] = (SpiaRegs.SPIRXBUF & 0x3FFF);
//
//        CS3_SPIA_on();
//        SpiaRegs.SPITXBUF = 0x7FFE;
//        DELAY_US(1e+3);
//        CS3_SPIA_off();
//        angl_get_arr[2] = (SpiaRegs.SPIRXBUF & 0x3FFF);
//
//        CS4_SPIA_on();
//        SpiaRegs.SPITXBUF = 0x7FFE;
//        DELAY_US(1e+3);
//        CS4_SPIA_off();
//        angl_get_arr[3] = (SpiaRegs.SPIRXBUF & 0x3FFF);


//        GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
//        GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;
//        GpioDataRegs.GPASET.bit.GPIO14 = 1;
//        GpioDataRegs.GPASET.bit.GPIO15 = 1;
//        GpioDataRegs.GPATOGGLE.bit.GPIO14 = 1;
//        GpioDataRegs.GPATOGGLE.bit.GPIO15 = 1;
//        GpioDataRegs.GPACLEAR.bit.GPIO14 = 1;
//        GpioDataRegs.GPACLEAR.bit.GPIO15 = 1;
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

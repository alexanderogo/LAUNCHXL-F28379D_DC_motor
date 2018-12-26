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

extern volatile uint16_t DMA_Buf[2*CURR_TRFER_SZ*SIZE_CURR_BURST];
extern volatile uint16_t *currents;
extern volatile uint16_t *current1;
extern volatile uint16_t *current2;

extern volatile uint16_t *DMADest;
extern volatile uint16_t *DMASource;

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
void InitADC(volatile struct ADC_REGS *AdcxRegs, uint16_t channel1, uint16_t channel2);
void InitDMAforADCa(volatile struct ADC_RESULT_REGS *AdcxResultRegs);
void InitDMAforADCb(volatile struct ADC_RESULT_REGS *AdcxResultRegs);
__interrupt void dmach1_isr(void);
__interrupt void dmach2_isr(void);


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
uint32_t cntdma = 0;

volatile float pwm1 = 0.1;
volatile float pwm2 = 0.2;
volatile float pwm3 = 0.3;
volatile float pwm4 = 0.4;

volatile int16_t DacaVal = 1024;
volatile int16_t DacbVal = 2048;

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

    //
    // Stop the ePWM clock
    //
    EALLOW;
//    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    PieVectTable.EPWM1_TZ_INT = &epwm1_tzint_isr;
//    PieVectTable.EPWM1_TZ_INT = &EPWM1_TZ_ISR;
    PieVectTable.EPWM3_INT = &epwm3_int_isr;
    PieVectTable.DMA_CH1_INT= &dmach1_isr;
    PieVectTable.DMA_CH2_INT= &dmach2_isr;
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
    InitGpioLED();
    InitDAC();
    DacaRegs.DACVALS.all = DacaVal;
    DacbRegs.DACVALS.all = DacbVal;
    DELAY_US(1e+3);
    InitADC(&AdcbRegs, 2, 3);
    DMAInitialize();
    InitDMAforADCb(&AdcbResultRegs);
    initTIMER0();

    //
    // Start the ePWM clock
    //
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
    //
    // Enable CPU INT which is connected to chosen INT:
    //
    IER |= (M_INT1 | M_INT2 | M_INT3 | M_INT7);
    //
    // Enable chosen interrupts
    //
//    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;  // Enable TINT0 in the PIE: Group 1 __interrupt 7
    PieCtrlRegs.PIEIER2.bit.INTx1 = 1;  // 2.1 - ePWM1 Trip Zone Interrupt
    PieCtrlRegs.PIEIER2.bit.INTx2 = 1;  // 2.2 - ePWM2 Trip Zone Interrupt
    PieCtrlRegs.PIEIER3.bit.INTx3 = 1;  // 3.3 - ePWM3 Interrupt
    PieCtrlRegs.PIEIER7.bit.INTx1 = 1;   // DMA1
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
//        DacaRegs.DACVALS.all = DacaVal;
//        DacbRegs.DACVALS.all = DacbVal;
        LED_BLUE_toggle();

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

__interrupt void dmach1_isr(void)
{
    static uint32_t dmach1_int_count = 0;
    dmach1_int_count++;
    //
    // Acknowledge this interrupt to receive more interrupts from group 7
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;
}

__interrupt void dmach2_isr(void)
{
    static uint32_t dmach2_int_count = 0;
    dmach2_int_count++;
    cntdma++;
    //
    // Acknowledge this interrupt to receive more interrupts from group 7
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;
}

void InitADC(volatile struct ADC_REGS *AdcxRegs, uint16_t channel1, uint16_t channel2)
{
    //Uint16 channel = 1;

    /*Uint16 acqps;

    //determine minimum acquisition window (in SYSCLKS) based on resolution
    if(ADC_RESOLUTION_12BIT == AdcxRegs->ADCCTL2.bit.RESOLUTION){
        acqps = 14; //75ns
    }
    else { //resolution is 16-bit
        acqps = 63; //320ns
    }*/

    //Select the channels to convert and end of conversion flag
    EALLOW;
    //AdcxRegs->ADCBURSTCTL.bit.BURSTEN = 1; //Enable ADC burst mode
    //AdcxRegs->ADCBURSTCTL.bit.BURSTTRIGSEL = 5; //CPU1 Timer 2 will trigger burst of conversions
    //AdcxRegs->ADCBURSTCTL.bit.BURSTSIZE = 16*SIZE_ARR_CURR*SIZE_ARR_CURR - 1;

    AdcxRegs->ADCSOC0CTL.bit.CHSEL = channel1;  //SOC0 will convert pin A0
    AdcxRegs->ADCSOC0CTL.bit.ACQPS = 36;//(Uint16)(PWM_PERIOD / SIZE_ARR_CURR); //sample window is 100 SYSCLK cycles
    AdcxRegs->ADCSOC0CTL.bit.TRIGSEL = 15; //5; //trigger on ePWM1 SOCA/C

    AdcxRegs->ADCSOC1CTL.bit.CHSEL = channel2;  //SOC0 will convert pin A0
    AdcxRegs->ADCSOC1CTL.bit.ACQPS = 36;//(Uint16)(PWM_PERIOD / SIZE_ARR_CURR); //sample window is 100 SYSCLK cycles
    AdcxRegs->ADCSOC1CTL.bit.TRIGSEL = 15; //5; //trigger on ePWM1 SOCA/C
//    AdcxRegs->ADCINTSOCSEL1.bit.SOC0 = 1;
//    AdcxRegs->ADCINTSOCSEL1.bit.SOC1 = 1;

    AdcxRegs->ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC0 will set INT1 flag
    AdcxRegs->ADCINTSEL1N2.bit.INT1CONT = 1; //end of SOC0 will set INT1 flag
    AdcxRegs->ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcxRegs->ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

    /*AdcxRegs->ADCINTSEL1N2.bit.INT2SEL = 1; //end of SOC0 will set INT1 flag
    AdcxRegs->ADCINTSEL1N2.bit.INT2CONT = 1; //end of SOC0 will set INT1 flag
    AdcxRegs->ADCINTSEL1N2.bit.INT2E = 1;   //enable INT1 flag
    AdcxRegs->ADCINTFLGCLR.bit.ADCINT2 = 1; //make sure INT1 flag is cleared
*/

    //write configurations
    AdcxRegs->ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcxRegs->ADCCTL2.bit.RESOLUTION = 0; //12bit
    AdcxRegs->ADCCTL2.bit.SIGNALMODE = 0; //single-end
    //AdcxRegs->ADCCTL2.bit.PRESCALE = 14; //set ADCCLK divider to /8
   // AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    //Set pulse positions to late
    AdcxRegs->ADCCTL1.bit.INTPULSEPOS = 1;

    //power up the ADC
    AdcxRegs->ADCCTL1.bit.ADCPWDNZ = 1;

    //delay for 1ms to allow ADC time to power up
    DELAY_US(1000);

    EDIS;
}

void InitDMAforADCa(volatile struct ADC_RESULT_REGS *AdcxResultRegs)
{
//    DMAInitialize();

// Configure DMA Channel
    DMADest   = &currents[0];              //Point DMA destination to the beginning of the array
    DMASource = &AdcaResultRegs.ADCRESULT0;    //Point DMA source to ADC result register base
    DMACH1AddrConfig(DMADest,DMASource);
    DMACH1BurstConfig(1,1,SIZE_CURR_BURST);
    DMACH1TransferConfig((SIZE_CURR_BURST)-1,0,0);
    DMACH1WrapConfig(0,0,0,1);
    DMACH1ModeConfig(DMA_ADCAINT1,PERINT_ENABLE,ONESHOT_DISABLE,CONT_ENABLE,SYNC_DISABLE,SYNC_SRC,
                     OVRFLOW_DISABLE,SIXTEEN_BIT,CHINT_END,CHINT_ENABLE);
    StartDMACH1();
}

void InitDMAforADCb(volatile struct ADC_RESULT_REGS *AdcxResultRegs)
{
//    DMAInitialize();

// Configure DMA Channel
    DMADest   = &currents[0];              //Point DMA destination to the beginning of the array
    DMASource = &AdcbResultRegs.ADCRESULT0;    //Point DMA source to ADC result register base
//    DMACH2AddrConfig(DMADest,DMASource);
//    DMACH2BurstConfig(1,1,SIZE_CURR_BURST);
//    DMACH2TransferConfig((SIZE_CURR_BURST)-1,0,0);
//    DMACH2WrapConfig(0,0,0,1);
//    DMACH2ModeConfig(DMA_ADCBINT1,PERINT_ENABLE,ONESHOT_DISABLE,CONT_ENABLE,SYNC_DISABLE,SYNC_SRC,
//                     OVRFLOW_DISABLE,SIXTEEN_BIT,CHINT_END,CHINT_ENABLE);
//    StartDMACH2();
    DMADest   = &currents[0];              //Point DMA destination to the beginning of the array
    DMASource = &AdcbResultRegs.ADCRESULT0;    //Point DMA source to ADC result register base
    DMACH2AddrConfig(DMADest,DMASource);
    EALLOW;
    // Set up SOURCE address:
    DmaRegs.CH2.SRC_BEG_ADDR_SHADOW = (Uint32)DMASource;   // Point to beginning of source buffer.
    DmaRegs.CH2.SRC_ADDR_SHADOW =     (Uint32)DMASource;
    // Set up DESTINATION address:
    DmaRegs.CH2.DST_BEG_ADDR_SHADOW = (Uint32)DMADest;  // Point to beginning of destination buffer.
    DmaRegs.CH2.DST_ADDR_SHADOW =     (Uint32)DMADest;
    // Set up BURST registers:
    DmaRegs.CH2.BURST_SIZE.all = 1;     // Number of words(X-1) x-ferred in a burst.
    DmaRegs.CH2.SRC_BURST_STEP = 1;  // Increment source addr between each word x-ferred.
    DmaRegs.CH2.DST_BURST_STEP = CURR_TRFER_SZ*SIZE_CURR_BURST;  // Increment dest addr between each word x-ferred.
    // Set up TRANSFER registers:
    DmaRegs.CH2.TRANSFER_SIZE = (CURR_TRFER_SZ*SIZE_CURR_BURST)-1;        // Number of bursts per transfer, DMA interrupt will occur after completed transfer.
    DmaRegs.CH2.SRC_TRANSFER_STEP = 0; // TRANSFER_STEP is ignored when WRAP occurs.
    DmaRegs.CH2.DST_TRANSFER_STEP = 0; // TRANSFER_STEP is ignored when WRAP occurs.
    // Set up WRAP registers:
    DmaRegs.CH2.SRC_WRAP_SIZE = 0; // Wrap source address after N bursts
    DmaRegs.CH2.SRC_WRAP_STEP = 0; // Step for source wrap
    DmaRegs.CH2.DST_WRAP_SIZE = 0; // Wrap destination address after N bursts.
    DmaRegs.CH2.DST_WRAP_STEP = 1; // Step for destination wrap
    // Set up MODE Register:
    DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH2 = DMA_ADCBINT1;   // persel - Source select
    DmaRegs.CH2.MODE.bit.PERINTSEL = 2;                     // PERINTSEL - Should be hard coded to channel, above now selects source
    DmaRegs.CH2.MODE.bit.PERINTE = PERINT_ENABLE;           // PERINTE - Peripheral interrupt enable
    DmaRegs.CH2.MODE.bit.ONESHOT = ONESHOT_DISABLE;         // ONESHOT - Oneshot enable
//    DmaRegs.CH2.MODE.bit.CONTINUOUS = CONT_ENABLE;          // CONTINUOUS - Continuous enable
    DmaRegs.CH2.MODE.bit.CONTINUOUS = CONT_DISABLE;          // CONTINUOUS - Continuous enable
    DmaRegs.CH2.MODE.bit.OVRINTE = OVRFLOW_DISABLE;         // OVRINTE - Enable/disable the overflow interrupt
    DmaRegs.CH2.MODE.bit.DATASIZE = SIXTEEN_BIT;            // DATASIZE - 16-bit/32-bit data size transfers
    DmaRegs.CH2.MODE.bit.CHINTMODE = CHINT_END;             // CHINTMODE - Generate interrupt to CPU at beginning/end of transfer
    DmaRegs.CH2.MODE.bit.CHINTE = CHINT_ENABLE;             // CHINTE - Channel Interrupt to  CPU enable
    // Clear any spurious flags: Interrupt flags and sync error flags
    DmaRegs.CH2.CONTROL.bit.PERINTCLR = 1;
    DmaRegs.CH2.CONTROL.bit.ERRCLR = 1;
    // Initialize PIE vector for CPU interrupt
    PieCtrlRegs.PIEIER7.bit.INTx2 = 1;                      // Enable DMA CH2 interrupt in PIE
    // Starts DMA Channel 2
//    DmaRegs.CH2.CONTROL.bit.RUN = 1;
    EDIS;
//    DMACH2ModeConfig(DMA_ADCBINT1,PERINT_ENABLE,ONESHOT_DISABLE,CONT_ENABLE,SYNC_DISABLE,SYNC_SRC,
//                     OVRFLOW_DISABLE,SIXTEEN_BIT,CHINT_END,CHINT_ENABLE);
//    StartDMACH2();
}


//
// End of File
//

/*
 * _epwm.c
 *
 *  Created on: 10 ����. 2018 �.
 *      Author: Alexander
 */

#include "F28x_Project.h"
#include <stdint.h>
#include "_epwm.h"
#include "_led.h"
#include "_globals.h"
#include <filter.h>
#include <math.h>
#include "_current_loop.h"
#include "fpu_filter.h"
#include "DCLF32.h"
#include "_spi.h"
#include "_angle_loop.h"


extern volatile uint16_t *current1;
extern volatile uint16_t *current2;
extern volatile float current1f;
extern volatile float current2f;

extern FIR32 firFXc1;
extern FIR32 firFXc2;
extern FIR_FP firFPc1;
extern FIR_FP firFPc2;

//
// Globals
//
int32_t in_test = 123;
int32_t out_test = 0;
float32 out_test_float = 0;
int32_t divider = 0x3FF7;
uint32_t cntpwm = 0;
float32 curr1_test[50];
float32 curr2_test[50];
float32 c1fp = 0;
float32 c2fp = 0;
float32 c1fx = 0;
float32 c2fx = 0;

// global  variables
long IdleLoopCount = 0;
long IsrCount = 0;
float rk = 0.0f;           // reference
float yk = 0.0f;           // feedback
float lk = 1.0f;           // saturation
float uk = 0.0f;           // output
uint16_t byte_spi1 = 0x7FFE;
uint16_t byte_spi2 = 0x7FFE;
int16_t byte_cnt = 0;
uint32_t ang_per_div = 10;
uint16_t angl_adr_msg = 0x7FFE; // dynamic comp. 0xFFFF
uint16_t angl_get_arr[4] = {0};

uint32_t glob_cnt1 = 0;

extern DCL_PID pid1;

inline float mean_curr(const uint16_t *it);

__interrupt void epwm3_int_isr(void)
{
    static uint32_t EPwm3IntCount = 0;
    EPwm3IntCount++;
    int16_t d_count = (EPwm3IntCount-1)%2;
    if (EPwm3IntCount == 1) {
        EALLOW;
        DmaRegs.CH2.CONTROL.bit.RUN = 1;
        DmaRegs.CH3.CONTROL.bit.RUN = 1;
        DmaRegs.CH4.CONTROL.bit.RUN = 1;
        EDIS;
    }
    VECTOR2F target_ang = {0, 0};
    angle_loop_calc(target_ang, 8);
//    get_angle(8);

//    #pragma MUST_ITERATE(25, 25)
//    for (i = 0; i < 25; i++) {
//        acc += *it++;
//    }
    glob_cnt1++;

    current1f = mean_curr(current1 + SIZE_CURR_BURST*((d_count+CURR_TRFER_SZ - 1)%CURR_TRFER_SZ)); // conversation (uint16_t*)
    current2f = mean_curr(current2 + SIZE_CURR_BURST*((d_count+CURR_TRFER_SZ - 1)%CURR_TRFER_SZ)); // conversation (uint16_t*)
    firFPc1.input = current1f;
    firFPc1.calc(&firFPc1);
    curr1_test[EPwm3IntCount%50] = firFPc1.output;
    c1fp = firFPc1.output;
    firFPc2.input = current2f;
    firFPc2.calc(&firFPc2);
    curr2_test[EPwm3IntCount%50] = firFPc2.output;
    c2fp = firFPc2.output;

    firFXc1.input = current1f*divider + (FIR_ORDER_CURR + 1)/2;
    firFXc1.calc(&firFXc1);
    c1fx = (float)firFXc1.output/(float)divider;
    firFXc2.input = current2f*divider + (FIR_ORDER_CURR + 1)/2;
    firFXc2.calc(&firFXc2);
    c2fx = (float)firFXc2.output/(float)divider;

//    firFXc1.input = in_test*divider + (FIR_ORDER_CURR+1)/2;             //Input data
//    firFXc1.calc(&firFXc1);             //FIR convolution operation
//    out_test = firFXc1.output;
//    out_test_float = (float)firFXc1.output/(float)divider;
    yk = uk;
    if (EPwm3IntCount%1000 == 0) {
        uk = DCL_runPID_C2(&pid1, rk, yk, lk);
    }
    int16_t data_count = 0;
    int32_t diff_addr = 0;
    diff_addr = DmaRegs.CH2.DST_ADDR_ACTIVE - DmaRegs.CH2.DST_BEG_ADDR_SHADOW;
    data_count = diff_addr/25;
    cntpwm++;
//    EALLOW;
//    SysCtrlRegs.WDKEY = 0xAA;                   // service WD #2
//    EDIS;
//    asm ("      ESTOP0");
//    for(;;);

//    if (EPwm3IntCount%10 == 0) {
//        if (EPwm3IntCount%20 == 0) {
//            SpiaRegs.SPITXBUF = byte_spi1;//0xAAAA;
//        } else {
//            SpiaRegs.SPITXBUF = byte_spi2;
//        }
//    }
//    if (EPwm3IntCount%2 == 0) {
//        CS2_SPIA_off();
//    } else {
//        CS2_SPIA_on();
//    }
//    CS1_SPIA_off();
//    if (EPwm3IntCount%10 == 0) {
//        CS1_SPIA_on();
//        SpiaRegs.SPITXBUF = byte_spi1;
////        static int16_t cnt_msg = 0;
////        if (cnt_msg < 4) {
////            SpiaRegs.SPITXBUF = byte_spi1;
////            cnt_msg++;
////        }
//    }
    EPwm3Regs.ETCLR.bit.INT = 0x1;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

inline float mean_curr(const uint16_t *it)
{
    uint32_t acc = 0;

    acc += *it++;
    acc += *it++;
    acc += *it++;
    acc += *it++;
    acc += *it++;

    acc += *it++;
    acc += *it++;
    acc += *it++;
    acc += *it++;
    acc += *it++;

    acc += *it++;
    acc += *it++;
    acc += *it++;
    acc += *it++;
    acc += *it++;

    acc += *it++;
    acc += *it++;
    acc += *it++;
    acc += *it++;
    acc += *it++;

    acc += *it++;
    acc += *it++;
    acc += *it++;
    acc += *it++;
    acc += *it++;

    return (float)acc/25.0f;
}

__interrupt void epwm1_tzint_isr(void)
{
    static uint32_t EPwm1TZIntCount = 0;
    EPwm1TZIntCount++;
    //    GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
    LED_RED_on();
    GpioDataRegs.GPASET.bit.GPIO14 = 1;
    GpioDataRegs.GPASET.bit.GPIO15 = 1;
    DELAY_US(1e+3);
    if (EPwm1TZIntCount < 1e+5) {
        LED_RED_off();
    }
    else {
        asm ("      ESTOP0");
        for(;;);
    }
    //
    // Clear the flags - we will continue to take
    // this interrupt until the TZ pin goes high
    //
    EALLOW;
    EPwm1Regs.TZCLR.bit.CBC = 1;
    EPwm1Regs.TZCLR.bit.INT = 1;
    EDIS;
    //
    // Acknowledge this interrupt to receive more interrupts from group 2
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;
}

void InitGpioTZ(void)
{
    //
    // Initialization GPIOs as inputs
    //

//    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 0;      // Enable pullup on GPIOXX
//    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 0;     // GPIOXX is GPIO
//    GpioCtrlRegs.GPAQSEL1.bit.GPIO6 = 1;    // set Qualification (3 samples)
//    GpioCtrlRegs.GPADIR.bit.GPIO6 = 0;      // GPIOXX is input
//
//    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 0;      // Enable pullup on GPIOXX
//    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;     // GPIOXX is GPIO
//    GpioCtrlRegs.GPAQSEL1.bit.GPIO7 = 1;    // set Qualification (3 samples)
//    GpioCtrlRegs.GPADIR.bit.GPIO7 = 0;      // GPIOXX is input

    GpioCtrlRegs.GPDPUD.bit.GPIO105 = 0;      // Enable pullup on GPIOXX
    GpioCtrlRegs.GPDMUX1.bit.GPIO105 = 0;     // GPIOXX is GPIO
    GpioCtrlRegs.GPDQSEL1.bit.GPIO105 = 1;    // set Qualification (3 samples)
    GpioCtrlRegs.GPDDIR.bit.GPIO105 = 0;      // GPIOXX is input

    GpioCtrlRegs.GPDPUD.bit.GPIO104 = 0;      // Enable pullup on GPIOXX
    GpioCtrlRegs.GPDMUX1.bit.GPIO104 = 0;     // GPIOXX is GPIO
    GpioCtrlRegs.GPDQSEL1.bit.GPIO104 = 1;    // set Qualification (3 samples)
    GpioCtrlRegs.GPDDIR.bit.GPIO104 = 0;      // GPIOXX is input
}

void InitTZ(void)
{
    //
    // For External Trigger, GPIO12 as the trigger for TripZone
    //

    InitGpioTZ();

    EALLOW;
    InputXbarRegs.INPUT1SELECT = 105;
    InputXbarRegs.INPUT2SELECT = 104;
    EDIS;

    EALLOW;

    // initialization Trip-Zone (TZ) Submodule
    //
    // Enable TZ1 as one shot trip sources
    //
    EPwm1Regs.TZSEL.bit.CBC1 = 1;        // Enable TZ1 as a cycle-by-cycle trip source for this ePWM module
    EPwm1Regs.TZSEL.bit.CBC2 = 1;        // Enable TZ2 as a cycle-by-cycle trip source for this ePWM module
    EPwm2Regs.TZSEL.bit.CBC1 = 1;        // Enable TZ1 as a cycle-by-cycle trip source for this ePWM module
    EPwm2Regs.TZSEL.bit.CBC2 = 1;        // Enable TZ2 as a cycle-by-cycle trip source for this ePWM module
    EPwm3Regs.TZSEL.bit.CBC1 = 1;        // Enable TZ1 as a cycle-by-cycle trip source for this ePWM module
    EPwm3Regs.TZSEL.bit.CBC2 = 1;        // Enable TZ2 as a cycle-by-cycle trip source for this ePWM module

    //
    // Set TZA
    //
    EPwm1Regs.TZCTL.bit.TZA = 2;              // TZ1 to TZ6, DCAEVT1/2, DCBEVT1/2 Trip Action On EPWMxA - Force EPWMxA to a low state
    EPwm1Regs.TZCTL.bit.TZB = 2;              // TZ1 to TZ6, DCAEVT1/2, DCBEVT1/2 Trip Action On EPWMxB - Force EPWMxB to a low state
    EPwm2Regs.TZCTL.bit.TZA = 2;              // TZ1 to TZ6, DCAEVT1/2, DCBEVT1/2 Trip Action On EPWMxA - Force EPWMxA to a low state
    EPwm2Regs.TZCTL.bit.TZB = 2;              // TZ1 to TZ6, DCAEVT1/2, DCBEVT1/2 Trip Action On EPWMxB - Force EPWMxB to a low state
    EPwm3Regs.TZCTL.bit.TZA = 2;              // TZ1 to TZ6, DCAEVT1/2, DCBEVT1/2 Trip Action On EPWMxA - Force EPWMxA to a low state
    EPwm3Regs.TZCTL.bit.TZB = 2;              // TZ1 to TZ6, DCAEVT1/2, DCBEVT1/2 Trip Action On EPWMxB - Force EPWMxB to a low state

    //
    // Enable TZ interrupt
    //
    EPwm1Regs.TZEINT.bit.CBC = 1;

    EDIS;

//    EALLOW;
//    InputXbarRegs.INPUT1SELECT = 6;
//    InputXbarRegs.INPUT2SELECT = 7;
//    EDIS;
}

void InitGpioGS(void)
{
    EALLOW;
    GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;     // Enable pullup on GPIOXX
//    GpioDataRegs.GPBSET.bit.GPIO32 = 1;     // Load output latch
    GpioDataRegs.GPBCLEAR.bit.GPIO32 = 1;     // Load output latch
    GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO32 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPCPUD.bit.GPIO67 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPCSET.bit.GPIO67 = 1;     // Load output latch
    GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPDPUD.bit.GPIO111 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPDSET.bit.GPIO111 = 1;     // Load output latch
    GpioCtrlRegs.GPDMUX1.bit.GPIO111 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPDDIR.bit.GPIO111 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPAPUD.bit.GPIO22 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPASET.bit.GPIO22 = 1;     // Load output latch
    GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO22 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPCPUD.bit.GPIO95 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPCSET.bit.GPIO95 = 1;     // Load output latch
    GpioCtrlRegs.GPCMUX2.bit.GPIO95 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPCDIR.bit.GPIO95 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPDPUD.bit.GPIO97 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPDSET.bit.GPIO97 = 1;     // Load output latch
    GpioCtrlRegs.GPDMUX1.bit.GPIO97 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPDDIR.bit.GPIO97 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPCPUD.bit.GPIO94 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPCSET.bit.GPIO94 = 1;     // Load output latch
    GpioCtrlRegs.GPCMUX2.bit.GPIO94 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPCDIR.bit.GPIO94 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPBPUD.bit.GPIO52 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPBSET.bit.GPIO52 = 1;     // Load output latch
    GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO52 = 1;     // GPIOXX is output
    EDIS;
}

void InitGpioEPWM1(void)
{
    //
    // Enable EPWM on GPIO
    //
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;   // Disable pullup
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;   // Disable pullup
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;  // GPIO = PWMxA
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;  // GPIO = PWMxB
    EDIS;
}

void InitGpioEPWM2(void)
{
    //
    // Enable EPWM on GPIO
    //
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;   // Disable pullup
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;   // Disable pullup
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;  // GPIO = PWMxA
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;  // GPIO = PWMxB
    EDIS;
}

void InitGpioEPWM3(void)
{
    //
    // Enable EPWM on GPIO
    //
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;   // Disable pullup
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;   // Disable pullup
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;  // GPIO = PWMxA
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;  // GPIO = PWMxB
    EDIS;
}

void InitGpioEPWM4(void)
{
    //
    // Enable EPWM on GPIO
    //
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;   // Disable pullup
    GpioCtrlRegs.GPAPUD.bit.GPIO7 = 1;   // Disable pullup
    GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;  // GPIO = PWMxA
    GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 1;  // GPIO = PWMxB
    EDIS;
}

void InitGpioEPWM5(void)
{
    //
    // Enable EPWM on GPIO
    //
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;   // Disable pullup
    GpioCtrlRegs.GPAPUD.bit.GPIO9 = 1;   // Disable pullup
    GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;  // GPIO = PWMxA
    GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 1;  // GPIO = PWMxB
    EDIS;
}

void InitGpioEPWM6(void)
{
    //
    // Enable EPWM on GPIO
    //
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;   // Disable pullup
    GpioCtrlRegs.GPAPUD.bit.GPIO11 = 1;   // Disable pullup
    GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;  // GPIO = PWMxA
    GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;  // GPIO = PWMxB
    EDIS;
}

void InitGpioEPWM8(void)
{
    //
    // Enable EPWM8 on GPIO
    //
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO14 = 1;   // Disable pullup
    GpioCtrlRegs.GPAPUD.bit.GPIO15 = 1;   // Disable pullup
//    GpioCtrlRegs.GPEPUD.bit.GPIO159 = 0;   // Disable pullup
//    GpioCtrlRegs.GPFPUD.bit.GPIO160 = 0;   // Disable pullup
    GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 1;  // GPIO14 = PWM8A
    GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 1;  // GPIO15 = PWM8B
//    GpioCtrlRegs.GPEMUX2.bit.GPIO159 = 1;  // GPIO159 = PWM8A
//    GpioCtrlRegs.GPFMUX1.bit.GPIO160 = 1;  // GPIO160 = PWM8B
    EDIS;
}

void initEPWM1(void)
{
    // Enable EPWM on GPIO
    InitGpioEPWM1();
    // initialization EPWM
    // initialization Trip-Zone (TZ) Submodule
//    EALLOW;
    //
    // Enable TZ1 as one shot trip sources
    //
//    EPwm1Regs.TZSEL.bit.OSHT1 = 1;        // Enable TZ1 as a one-shot trip source for this ePWM module

    //
    // Set TZA
    //
//    EPwm1Regs.TZCTL.bit.TZA = 2;              // TZ1 to TZ6, DCAEVT1/2, DCBEVT1/2 Trip Action On EPWMxA - Force EPWMxA to a low state
//    EPwm1Regs.TZCTL.bit.TZB = 2;              // TZ1 to TZ6, DCAEVT1/2, DCBEVT1/2 Trip Action On EPWMxB - Force EPWMxB to a low state

    //
    // Enable TZ interrupt
    //
//    EPwm1Regs.TZEINT.bit.OST = 1;


//    EPwm1Regs.TZSEL.bit.DCAEVT2 = 1;        // Enable DCAEVT2 as a CBC (Cycle-by-Cycle (CBC)) trip source for this ePWM module
//    EPwm1Regs.TZSEL.bit.DCBEVT2 = 1;        // Enable DCBEVT2 as a CBC (Cycle-by-Cycle (CBC)) trip source for this ePWM module
//    EPwm1Regs.TZDCSEL.bit.DCAEVT2 = 1;      // Generate event when High Digital Compare Output A (DCAH) = low
//    EPwm1Regs.TZDCSEL.bit.DCBEVT2 = 1;      // Generate event when High Digital Compare Output B (DCBH) = low
//    EPwm1Regs.TZCTL.bit.DCAEVT2 = 2;            // Digital Compare Output A Event 2 Action On EPWMxA - Force EPWMxA to a low state
//    EPwm1Regs.TZCTL.bit.DCBEVT2 = 2;            // Digital Compare Output B Event 2 Action On EPWMxB - Force EPWMxB to a low state
//    EPwm1Regs.DCTRIPSEL.bit.DCAHCOMPSEL = 0;    // Digital Compare A High Input Select TRIPIN1 and (TZ1 input)
//    EPwm1Regs.DCTRIPSEL.bit.DCBHCOMPSEL = 0;    // Digital Compare B High Input Select TRIPIN1 and (TZ1 input)
    // Optionally:
//    EPwm1Regs.DCACTL.bit.EVT2SRCSEL = 0;        // DCAEVT2 Source Signal Select - Source Is DCAEVT2 Signal
//    EPwm1Regs.DCACTL.bit.EVT2FRCSYNCSEL = 1;    // DCAEVT2 Force Synchronization Signal Select - Source Is Asynchronous Signal
//    EPwm1Regs.DCBCTL.bit.EVT2SRCSEL = 0;        // DCBEVT2 Source Signal Select - Source Is DCBEVT2 Signal
//    EPwm1Regs.DCBCTL.bit.EVT2FRCSYNCSEL = 1;    // DCBEVT2 Force Synchronization Signal Select - Source Is Asynchronous Signal
//    EDIS;

    EPwm1Regs.TBCTL.bit.CLKDIV = 0x0;          // CLKDIV = 0 (2 for 1kHz)
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x1;        // HSPCLKDIV = 1 (5 for 1kHz)
    //
    // Settings for Up-down count mode
    //
    EPwm1Regs.TBCTL.bit.CTRMODE = 0x2;          // Up-down count mode
    //
    // Settings for Up-count mode
    //
//    EPwm1Regs.TBCTL.bit.CTRMODE = 0x0;          // Up-count mode
    //
    // Settings for Down-count mode
    //
//    EPwm1Regs.TBCTL.bit.CTRMODE = 0x1;          // Down-count mode
    EPwm1Regs.TBPRD = EPWM_TBPRD;               // f = 40 kHz - PWM signal for Up-down count mode (f = 20 kHz - PWM signal for Up-count and Down-count modes)
    EPwm1Regs.CMPA.bit.CMPA = EPWM_TBPRD*2/3;                // Compare level
    //
    // Settings for Up-down count mode
    //
    EPwm1Regs.AQCTLA.bit.CAU = 0x1;             // Action When TBCTR = CMPA on Up Count - Clear: force EPWMxA output low
    EPwm1Regs.AQCTLA.bit.CAD = 0x2;             // Action When TBCTR = CMPA on Down Count - Set: force EPWMxA output high
    EPwm1Regs.AQCTLB.bit.CAU = 0x2;             // Action When TBCTR = CMPA on Up Count - Set: force EPWMxB output high
    EPwm1Regs.AQCTLB.bit.CAD = 0x1;             // Action When TBCTR = CMPA on Down Count - Clear: force EPWMxB output low
    //
    // Settings for Up-count mode
    //
//    EPwm1Regs.AQCTLA.bit.CAU = 0x1;             // Action When TBCTR = CMPA on Up Count - Clear: force EPWMxA output low
//    EPwm1Regs.AQCTLA.bit.ZRO = 0x2;             // Action When TBCTR = 0 - Set: force EPWMxA output high
//    EPwm1Regs.AQCTLB.bit.CAU = 0x2;             // Action When TBCTR = CMPA on Down Count - Set: force EPWMxB output high
//    EPwm1Regs.AQCTLB.bit.ZRO = 0x1;             // Action When TBCTR = 0 - Clear: force EPWMxB output low
    //
    // Settings for Down-count mode
    //
//    EPwm1Regs.AQCTLA.bit.CAD = 0x2;             // Action When TBCTR = CMPA on Down Count - Set: force EPWMxA output high
//    EPwm1Regs.AQCTLA.bit.ZRO = 0x1;             // Action When TBCTR = 0 - Clear: force EPWMxA output low
//    EPwm1Regs.AQCTLB.bit.CAD = 0x1;             // Action When TBCTR = CMPA on Up Count - Clear: force EPWMxB output low
//    EPwm1Regs.AQCTLB.bit.ZRO = 0x2;             // Action When TBCTR = 0 - Set: force EPWMxB output high

    EPwm1Regs.DBCTL.bit.OUT_MODE = 0x3;             // Dead-Band is fully enabled (i.e. both Rising Edge Delay and Falling Edge Delay active)
    EPwm1Regs.DBCTL.bit.POLSEL = 0x2;               // Active high complementary (AHC). EPWMxB is inverted
    EPwm1Regs.DBCTL.bit.IN_MODE = 0x0;              // EPWMxA In (from the action-qualifier) is the source for both falling-edge and rising-edge delay
    EPwm1Regs.DBRED.bit.DBRED = 0x5;                // 0x1 -> 10ns rise delay
    EPwm1Regs.DBFED.bit.DBFED = 0x5;                // 0x1 -> 10ns fall delay

    EPwm1Regs.TBCTL.bit.SYNCOSEL = 1;               // generate a syncout if CTR = 0

//    EPwm1Regs.ETPS.bit.SOCACNT = 0x1;           // 1 event has occurred.
//    EPwm1Regs.ETPS.bit.SOCAPRD = 0x1;           // Generate the EPWMxSOCA pulse on the first event
//    EPwm1Regs.ETSEL.bit.SOCASEL = 0x1;          // Enable event time-base counter equal to zero. (TBCTR = 0x0000)
//    EPwm1Regs.ETSEL.bit.SOCAEN = 0x1;           // Enable EPWMxSOCA pulse
//
//    EPwm1Regs.ETPS.bit.INTCNT = 0x1;            // ePWM Interrupt Event (EPWMx_INT) Counter Register, 1 event has occurred.
//    EPwm1Regs.ETPS.bit.INTPRD = 0x1;            // ePWM Interrupt (EPWMx_INT) Period Select, Generate an interrupt on the first event INTCNT = 01 (first event)
//    EPwm1Regs.ETSEL.bit.INTSEL = 0x1;           // ePWM Interrupt (EPWMx_INT) Selection Options, Enable event time-base counter equal to zero. (TBCTR = 0x0000)
//    EPwm1Regs.ETSEL.bit.INTEN = 0x1;            // Enable ePWM Interrupt (EPWMx_INT) Generation, Enable EPWMx_INT generation
//
//    EPwm3Regs.ETPS.bit.INTCNT = 0x1;
//    EPwm3Regs.ETPS.bit.INTPRD = 0x1;
//    EPwm3Regs.ETSEL.bit.INTSEL = 0x1;
//    EPwm3Regs.ETSEL.bit.INTEN = 0x1;
}

void initEPWMxMODE(volatile struct EPWM_REGS *EPwmxRegs, uint16_t epwm_period)
{
    // initialization EPWM
    EPwmxRegs->TBCTL.bit.CLKDIV = 0x0;          // CLKDIV = 1 (2 for 1kHz)
    EPwmxRegs->TBCTL.bit.HSPCLKDIV = 0x1;        // HSPCLKDIV = 1 (5 for 1kHz)
    EPwmxRegs->TBCTL.bit.CTRMODE = 0x2;          // Up-down count mode
    EPwmxRegs->TBPRD = epwm_period;               // f = 40 kHz - PWM signal for Up-down count mode (f = 20 kHz - PWM signal for Up-count and Down-count modes)
}

void initEPWMxAB(volatile struct EPWM_REGS *EPwmxRegs)
{
    // initialization EPWM
    EPwmxRegs->CMPA.bit.CMPA = 0;   // Compare level
    EPwmxRegs->AQCTLA.bit.CAU = 0x1;             // Action When TBCTR = CMPA on Up Count - Clear: force EPWMxA output low
    EPwmxRegs->AQCTLA.bit.CAD = 0x2;             // Action When TBCTR = CMPA on Down Count - Set: force EPWMxA output high
    EPwmxRegs->AQCTLB.bit.CAU = 0x2;             // Action When TBCTR = CMPA on Up Count - Set: force EPWMxB output high
    EPwmxRegs->AQCTLB.bit.CAD = 0x1;             // Action When TBCTR = CMPA on Down Count - Clear: force EPWMxB output low
    EPwmxRegs->DBCTL.bit.OUT_MODE = 0x3;             // Dead-Band is fully enabled (i.e. both Rising Edge Delay and Falling Edge Delay active)
    EPwmxRegs->DBCTL.bit.POLSEL = 0x2;               // Active high complementary (AHC). EPWMxB is inverted
    EPwmxRegs->DBCTL.bit.IN_MODE = 0x0;              // EPWMxA In (from the action-qualifier) is the source for both falling-edge and rising-edge delay
    EPwmxRegs->DBRED.bit.DBRED = 0x5;                // 0x1 -> 10ns rise delay
    EPwmxRegs->DBFED.bit.DBFED = 0x5;                // 0x1 -> 10ns fall delay
}

void initEPWMxA(volatile struct EPWM_REGS *EPwmxRegs)
{
    // initialization EPWM
    EPwmxRegs->CMPA.bit.CMPA = 0;   // Compare level
    EPwmxRegs->AQCTLA.bit.CAU = 0x1;             // Action When TBCTR = CMPA on Up Count - Clear: force EPWMxA output low
    EPwmxRegs->AQCTLA.bit.CAD = 0x2;             // Action When TBCTR = CMPA on Down Count - Set: force EPWMxA output high
}

void initEPWMxB(volatile struct EPWM_REGS *EPwmxRegs)
{
    // initialization EPWM
    EPwmxRegs->CMPB.bit.CMPB = 0;   // Compare level
    EPwmxRegs->AQCTLB.bit.CBU = 0x1;             // Action When TBCTR = CMPA on Up Count - Set: force EPWMxB output high
    EPwmxRegs->AQCTLB.bit.CBD = 0x2;             // Action When TBCTR = CMPA on Down Count - Clear: force EPWMxB output low
}

void initEPWMxInt(volatile struct EPWM_REGS *EPwmxRegs)
{
    EPwmxRegs->ETPS.bit.INTCNT = 0x1;            // ePWM Interrupt Event (EPWMx_INT) Counter Register, 1 event has occurred.
    EPwmxRegs->ETPS.bit.INTPRD = 0x1;            // ePWM Interrupt (EPWMx_INT) Period Select, Generate an interrupt on the first event INTCNT = 01 (first event)
    EPwmxRegs->ETSEL.bit.INTSEL = 0x1;           // ePWM Interrupt (EPWMx_INT) Selection Options, Enable event time-base counter equal to zero. (TBCTR = 0x0000)
    EPwmxRegs->ETSEL.bit.INTEN = 0x1;            // Enable ePWM Interrupt (EPWMx_INT) Generation, Enable EPWMx_INT generation
}

void initEPWMxSyncIn(volatile struct EPWM_REGS *EPwmxRegs, uint16_t tbphs)
{
    EPwmxRegs->TBCTL.bit.PHSEN = 1;              // enable phase shift for ePWM3
    EPwmxRegs->TBCTL.bit.SYNCOSEL = 0;           // syncin = syncout
    EPwmxRegs->TBPHS.bit.TBPHS = tbphs;          // Phase Offset Register
    EPwmxRegs->TBPHS.bit.TBPHSHR = 0;            // Phase Offset (High Resolution) Register
}

void initEPWMxSyncOut(volatile struct EPWM_REGS *EPwmxRegs)
{
    EPwmxRegs->TBCTL.bit.SYNCOSEL = 1;           // generate a syncout if CTR = 0
}

void initEPWMxASyncSOC(volatile struct EPWM_REGS *EPwmxRegs, uint16_t soc_period_divider)
{
    EPwmxRegs->ETPS.bit.SOCACNT = 0x1;                  // 1 event has occurred.
    EPwmxRegs->ETPS.bit.SOCAPRD = soc_period_divider;   // Generate the EPWMxSOCA pulse on the first event
    EPwmxRegs->ETSEL.bit.SOCASEL = 0x1;                 // Enable event time-base counter equal to zero. (TBCTR = 0x0000)
    EPwmxRegs->ETSEL.bit.SOCAEN = 0x1;                  // Enable EPWMxSOCA pulse
}

void initEPWMxBSyncSOC(volatile struct EPWM_REGS *EPwmxRegs, uint16_t soc_period_divider)
{
    EPwmxRegs->ETPS.bit.SOCBCNT = 0x1;                  // 1 event has occurred.
    EPwmxRegs->ETPS.bit.SOCBPRD = soc_period_divider;   // Generate the EPWMxSOCB pulse on the N event
    EPwmxRegs->ETSEL.bit.SOCBSEL = 0x1;                 // Enable event time-base counter equal to zero. (TBCTR = 0x0000)
    EPwmxRegs->ETSEL.bit.SOCBEN = 0x1;                  // Enable EPWMxSOCB pulse
}

void initEPWM(void)
{
    InitGpioEPWM1();
    InitGpioEPWM2();
    InitGpioEPWM3();
    InitGpioEPWM6();
    initEPWMxMODE(&EPwm1Regs, EPWM_TBPRD);
//    initEPWMxAB(&EPwm1Regs);
    initEPWMxA(&EPwm1Regs);
    initEPWMxB(&EPwm1Regs);
    initEPWMxSyncOut(&EPwm1Regs);
//    initEPWMxASyncSOC(&EPwm1Regs);
//    initEPWMxInt(&EPwm1Regs);
//    initEPWMxASyncSOC(&EPwm1Regs);
//    initEPWMxBSyncSOC(&EPwm1Regs);

    initEPWMxMODE(&EPwm2Regs, EPWM_TBPRD);
//    initEPWMxAB(&EPwm2Regs);
    initEPWMxA(&EPwm2Regs);
    initEPWMxB(&EPwm2Regs);
//    initEPWMxInt(&EPwm2Regs);
    initEPWMxSyncIn(&EPwm2Regs, 0);
//    initEPWMxASyncSOC(&EPwm2Regs);
//    initEPWMxBSyncSOC(&EPwm2Regs);

    initEPWMxMODE(&EPwm3Regs, EPWM_TBPRD);
//    initEPWMxAB(&EPwm3Regs);
    initEPWMxA(&EPwm3Regs);
    initEPWMxB(&EPwm3Regs);
    initEPWMxInt(&EPwm3Regs);
    initEPWMxSyncIn(&EPwm3Regs, 40);
//    initEPWMxASyncSOC(&EPwm3Regs);
//    initEPWMxBSyncSOC(&EPwm3Regs);

    initEPWMxMODE(&EPwm6Regs, EPWM_TBPRD/25);
//    initEPWMxAB(&EPwm6Regs);
    initEPWMxA(&EPwm6Regs);
    initEPWMxB(&EPwm6Regs);
    initEPWMxInt(&EPwm6Regs);
    initEPWMxSyncIn(&EPwm6Regs, 0);
    initEPWMxASyncSOC(&EPwm6Regs, 1);
    initEPWMxBSyncSOC(&EPwm6Regs, 2);
//    initEPWMxBSyncSOC(&EPwm6Regs);

    initEPWMxMODE(&EPwm12Regs, EPWM_TBPRD/50);
    initEPWMxA(&EPwm12Regs);
    initEPWMxB(&EPwm12Regs);
//    initEPWMxInt(&EPwm12Regs);
    initEPWMxSyncIn(&EPwm12Regs, 0);
    initEPWMxASyncSOC(&EPwm12Regs, 1);
    initEPWMxBSyncSOC(&EPwm12Regs, 2);
//    initEPWMxBSyncSOC(&EPwm6Regs);

//    EPWMxA1Ph1PinOut(&EPwm1Regs, 100);
//    EPWMxB1Ph1PinOut(&EPwm1Regs, 200);
//    EPWMxA1Ph1PinOut(&EPwm3Regs, 300);
//    EPWMxB1Ph1PinOut(&EPwm3Regs, 400);
//    EPWMx2Ph2PinOut(&EPwm1Regs, 100);
//    EPWMx2Ph2PinOut(&EPwm3Regs, 400);
//    EPWMx1Ph2PinOut(&EPwm1Regs, 100);
//    EPWMx1Ph2PinOut(&EPwm3Regs, 400);
//    EPWMx2Ph4PinOut(&EPwm1Regs, &EPwm3Regs, 400);

//    initEPWMxMODE(&EPwm1Regs);
//    initEPWMxAB(&EPwm1Regs);
//    initEPWMxA(&EPwm1Regs);
//    initEPWMxB(&EPwm1Regs);
//    initEPWMxInt(&EPwm1Regs);
//    initEPWMxSyncIn(&EPwm1Regs, 0);
//    initEPWMxSyncOut(&EPwm1Regs);
//    initEPWMxASyncSOC(&EPwm1Regs);
//    initEPWMxBSyncSOC(&EPwm1Regs);
    // initialization EPWM8 for sampling interval
}

void InitGpioEPWM(void)
{
    //
    // Enable PWM1-3 on GPIO0-GPIO5
    //
    EALLOW;
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = 1;   // Disable pullup on GPIO0
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;   // Disable pullup on GPIO1
    GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;   // Disable pullup on GPIO2
    GpioCtrlRegs.GPAPUD.bit.GPIO3 = 1;   // Disable pullup on GPIO3
    GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;   // Disable pullup on GPIO4
    GpioCtrlRegs.GPAPUD.bit.GPIO5 = 1;   // Disable pullup on GPIO5
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1;  // GPIO0 = PWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1;  // GPIO1 = PWM1B
    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;  // GPIO2 = PWM2A
    GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1;  // GPIO3 = PWM2B
    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;  // GPIO4 = PWM3A
    GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1;  // GPIO5 = PWM3B
    EDIS;
}

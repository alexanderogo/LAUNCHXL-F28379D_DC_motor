/*
 * _adc.c
 *
 *  Created on: 7 џэт. 2019 у.
 *      Author: Alexander
 */

#include "F28x_Project.h"
#include "_current_loop.h"
#include "_globals.h"
#include "_adc.h"

extern volatile uint16_t DMA_Buf[2*CURR_TRFER_SZ*SIZE_CURR_BURST];
extern volatile uint16_t *currents;
extern volatile uint16_t *current1;
extern volatile uint16_t *current2;

extern volatile uint16_t *DMADest;
extern volatile uint16_t *DMASource;

extern volatile uint16_t DMA_force_buf[2*FORCE_TRFER_SZ*SIZE_FORCE_BURST];
extern volatile uint16_t *forces;
extern volatile uint16_t *force1;
extern volatile uint16_t *force2;

extern volatile uint16_t voltage[SIZE_VOLTAGE_BURST*NUM_VOLT_CH*VOLT_BUF_SZ];

void InitADC_sampling(volatile struct ADC_REGS *AdcxRegs, const uint16_t channels[], uint16_t size, uint16_t trigsel);
void InitDMAforADC_sampling(volatile struct CH_REGS *DmaCH, const volatile uint16_t *source, uint16_t brust_size, const volatile uint16_t *dest, uint16_t transver_size, uint16_t per_int_source);

uint32_t glob_cnt2 = 0;

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
    //
    // Acknowledge this interrupt to receive more interrupts from group 7
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;
}

__interrupt void dmach3_isr(void)
{
    static uint32_t dmach3_int_count = 0;
    dmach3_int_count++;
    //
    // Acknowledge this interrupt to receive more interrupts from group 7
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;
}

__interrupt void dmach4_isr(void)
{
    static uint32_t dmach4_int_count = 0;
    dmach4_int_count++;
    glob_cnt2++;
    //
    // Acknowledge this interrupt to receive more interrupts from group 7
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;
}
/*
void InitADC(volatile struct ADC_REGS *AdcxRegs, uint16_t channel1, uint16_t channel2)
{
    EALLOW;

    // Write prescale and mode configurations
    AdcxRegs->ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcxRegs->ADCCTL2.bit.RESOLUTION = 0; //12bit
    AdcxRegs->ADCCTL2.bit.SIGNALMODE = 0; //single-end

    //AdcxRegs->ADCBURSTCTL.bit.BURSTEN = 1;        //Enable ADC burst mode
    //AdcxRegs->ADCBURSTCTL.bit.BURSTTRIGSEL = 3;   //CPU1 Timer 2 will trigger burst of conversions
    //AdcxRegs->ADCBURSTCTL.bit.BURSTSIZE = 1;      // conversion bursts are 1 + 1 = 2 conversions long
    //AdcxRegs->SOCPRICTL.bit.SOCPRIORITY = 12; //SOC0 to SOC11 are high priority

    AdcxRegs->ADCSOC0CTL.bit.CHSEL = channel1;      // SOC0 will convert pin channelx
    AdcxRegs->ADCSOC0CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
    AdcxRegs->ADCSOC0CTL.bit.TRIGSEL = 15;          // SOC0 Trigger Source Select ePWM6

    AdcxRegs->ADCSOC1CTL.bit.CHSEL = channel2;      // SOC1 will convert pin channelx
    AdcxRegs->ADCSOC1CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
    AdcxRegs->ADCSOC1CTL.bit.TRIGSEL = 15;          // SOC1 Trigger Source Select ePWM6
//    AdcxRegs->ADCINTSOCSEL1.bit.SOC0 = 1;
//    AdcxRegs->ADCINTSOCSEL1.bit.SOC1 = 1;

    AdcxRegs->ADCINTSEL1N2.bit.INT1SEL = 1; //end of SOC1 will set INT1 flag
    AdcxRegs->ADCINTSEL1N2.bit.INT1CONT = 1; //end of SOC1 will set INT1 flag
    AdcxRegs->ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcxRegs->ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared

    // Set pulse positions to late
    AdcxRegs->ADCCTL1.bit.INTPULSEPOS = 1;

    // Power up the ADC
    AdcxRegs->ADCCTL1.bit.ADCPWDNZ = 1;

    //delay for 1ms to allow ADC time to power up
    DELAY_US(1000);

    EDIS;
}
*/
void InitADC_sampling(volatile struct ADC_REGS *AdcxRegs, const uint16_t channels[], uint16_t size, uint16_t trigsel)
{
    EALLOW;

    // Write prescale and mode configurations
    AdcxRegs->ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcxRegs->ADCCTL2.bit.RESOLUTION = 0; //12bit
    AdcxRegs->ADCCTL2.bit.SIGNALMODE = 0; //single-end

    //AdcxRegs->ADCBURSTCTL.bit.BURSTEN = 1;        //Enable ADC burst mode
    //AdcxRegs->ADCBURSTCTL.bit.BURSTTRIGSEL = 3;   //CPU1 Timer 2 will trigger burst of conversions
    //AdcxRegs->ADCBURSTCTL.bit.BURSTSIZE = 1;      // conversion bursts are 1 + 1 = 2 conversions long
    //AdcxRegs->SOCPRICTL.bit.SOCPRIORITY = 12; //SOC0 to SOC11 are high priority
    switch (size) {
    case 16:
        AdcxRegs->ADCSOC15CTL.bit.CHSEL = channels[15];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC15CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC15CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 15:
        AdcxRegs->ADCSOC14CTL.bit.CHSEL = channels[14];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC14CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC14CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 14:
        AdcxRegs->ADCSOC13CTL.bit.CHSEL = channels[13];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC13CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC13CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 13:
        AdcxRegs->ADCSOC12CTL.bit.CHSEL = channels[12];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC12CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC12CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 12:
        AdcxRegs->ADCSOC11CTL.bit.CHSEL = channels[11];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC11CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC11CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 11:
        AdcxRegs->ADCSOC10CTL.bit.CHSEL = channels[10];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC10CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC10CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 10:
        AdcxRegs->ADCSOC9CTL.bit.CHSEL = channels[9];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC9CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC9CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 9:
        AdcxRegs->ADCSOC8CTL.bit.CHSEL = channels[8];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC8CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC8CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 8:
        AdcxRegs->ADCSOC7CTL.bit.CHSEL = channels[7];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC7CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC7CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 7:
        AdcxRegs->ADCSOC6CTL.bit.CHSEL = channels[6];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC6CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC6CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 6:
        AdcxRegs->ADCSOC5CTL.bit.CHSEL = channels[5];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC5CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC5CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 5:
        AdcxRegs->ADCSOC4CTL.bit.CHSEL = channels[4];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC4CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC4CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 4:
        AdcxRegs->ADCSOC3CTL.bit.CHSEL = channels[3];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC3CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC3CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 3:
        AdcxRegs->ADCSOC2CTL.bit.CHSEL = channels[2];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC2CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC2CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 2:
        AdcxRegs->ADCSOC1CTL.bit.CHSEL = channels[1];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC1CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC1CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
    case 1:
        AdcxRegs->ADCSOC0CTL.bit.CHSEL = channels[0];       // SOC15 will convert pin channelx
        AdcxRegs->ADCSOC0CTL.bit.ACQPS = 36;            // Sample window is acqps + 1 SYSCLK cycles
        AdcxRegs->ADCSOC0CTL.bit.TRIGSEL = trigsel;          // SOC15 Trigger Source Select ePWM12a
        break;
    default:
        asm ("      ESTOP0");
        for(;;);
    }

//    AdcxRegs->ADCINTSOCSEL1.bit.SOC0 = 1;
//    AdcxRegs->ADCINTSOCSEL1.bit.SOC1 = 1;

    AdcxRegs->ADCINTSEL1N2.bit.INT1SEL = size-1;     //end of SOCx will set INT1 flag
    AdcxRegs->ADCINTSEL1N2.bit.INT1CONT = 1;    //ADCINT1 pulses are generated whenever an EOC pulse
    AdcxRegs->ADCINTSEL1N2.bit.INT1E = 1;       //enable INT1 flag
    AdcxRegs->ADCINTFLGCLR.bit.ADCINT1 = 1;     //make sure INT1 flag is cleared

    // Set pulse positions to late
    AdcxRegs->ADCCTL1.bit.INTPULSEPOS = 1;

    // Power up the ADC
    AdcxRegs->ADCCTL1.bit.ADCPWDNZ = 1;

    //delay for 1ms to allow ADC time to power up
    DELAY_US(1000);

    EDIS;
}
/*
void InitDMAforADCd(volatile struct ADC_RESULT_REGS *AdcxResultRegs)
{
    // Configure DMA Channel
    EALLOW;
    // Set up SOURCE address:
    DmaRegs.CH3.SRC_BEG_ADDR_SHADOW = (Uint32)&AdcdResultRegs.ADCRESULT0;   // Point to beginning of source buffer.
    DmaRegs.CH3.SRC_ADDR_SHADOW =     (Uint32)&AdcdResultRegs.ADCRESULT0;
    // Set up DESTINATION address:
    DmaRegs.CH3.DST_BEG_ADDR_SHADOW = (Uint32)&forces[0];  // Point to beginning of destination buffer.
    DmaRegs.CH3.DST_ADDR_SHADOW =     (Uint32)&forces[0];
    // Set up BURST registers:
    DmaRegs.CH3.BURST_SIZE.all = 1;     // Number of words(X-1) x-ferred in a burst.
    DmaRegs.CH3.SRC_BURST_STEP = 1;  // Increment source addr between each word x-ferred.
    DmaRegs.CH3.DST_BURST_STEP = FORCE_TRFER_SZ*SIZE_FORCE_BURST;  // Increment dest addr between each word x-ferred.
    // Set up TRANSFER registers:
    DmaRegs.CH3.TRANSFER_SIZE = (FORCE_TRFER_SZ*SIZE_FORCE_BURST)-1;        // Number of bursts per transfer, DMA interrupt will occur after completed transfer.
    DmaRegs.CH3.SRC_TRANSFER_STEP = 0; // TRANSFER_STEP is ignored when WRAP occurs.
    DmaRegs.CH3.DST_TRANSFER_STEP = 0; // TRANSFER_STEP is ignored when WRAP occurs.
    // Set up WRAP registers:
    DmaRegs.CH3.SRC_WRAP_SIZE = 0; // Wrap source address after N bursts
    DmaRegs.CH3.SRC_WRAP_STEP = 0; // Step for source wrap
    DmaRegs.CH3.DST_WRAP_SIZE = 0; // Wrap destination address after N bursts.
    DmaRegs.CH3.DST_WRAP_STEP = 1; // Step for destination wrap
    // Set up MODE Register:
    DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH3 = DMA_ADCDINT1;   // persel - Source select
    DmaRegs.CH3.MODE.bit.PERINTSEL = 2;                     // PERINTSEL - Should be hard coded to channel, above now selects source
    DmaRegs.CH3.MODE.bit.PERINTE = PERINT_ENABLE;           // PERINTE - Peripheral interrupt enable
    DmaRegs.CH3.MODE.bit.ONESHOT = ONESHOT_DISABLE;         // ONESHOT - Oneshot enable
//    DmaRegs.CH3.MODE.bit.CONTINUOUS = CONT_ENABLE;          // CONTINUOUS - Continuous enable
    DmaRegs.CH3.MODE.bit.CONTINUOUS = CONT_DISABLE;          // CONTINUOUS - Continuous enable
    DmaRegs.CH3.MODE.bit.OVRINTE = OVRFLOW_DISABLE;         // OVRINTE - Enable/disable the overflow interrupt
    DmaRegs.CH3.MODE.bit.DATASIZE = SIXTEEN_BIT;            // DATASIZE - 16-bit/32-bit data size transfers
    DmaRegs.CH3.MODE.bit.CHINTMODE = CHINT_END;             // CHINTMODE - Generate interrupt to CPU at beginning/end of transfer
    DmaRegs.CH3.MODE.bit.CHINTE = CHINT_DISABLE;             // CHINTE - Channel Interrupt to  CPU enable
//    DmaRegs.CH3.MODE.bit.CHINTE = CHINT_ENABLE;             // CHINTE - Channel Interrupt to  CPU enable
    // Clear any spurious flags: Interrupt flags and sync error flags
    DmaRegs.CH3.CONTROL.bit.PERINTCLR = 1;
    DmaRegs.CH3.CONTROL.bit.ERRCLR = 1;
    // Starts DMA Channel 3
//    DmaRegs.CH3.CONTROL.bit.RUN = 1;
    EDIS;
}
*/
void InitDMAforADC_sampling(volatile struct CH_REGS *DmaCH, const volatile uint16_t *source, uint16_t brust_size, const volatile uint16_t *dest, uint16_t transver_size, uint16_t per_int_source)
{
    // Configure DMA Channel
    EALLOW;
    // Set up SOURCE address:
    DmaCH->SRC_BEG_ADDR_SHADOW = (Uint32)source;   // Point to beginning of source buffer.
    DmaCH->SRC_ADDR_SHADOW =     (Uint32)source;
    // Set up DESTINATION address:
    DmaCH->DST_BEG_ADDR_SHADOW = (Uint32)dest;  // Point to beginning of destination buffer.
    DmaCH->DST_ADDR_SHADOW =     (Uint32)dest;
    // Set up BURST registers:
    DmaCH->BURST_SIZE.all = brust_size - 1;     // Number of words(X-1) x-ferred in a burst.
    DmaCH->SRC_BURST_STEP = 1;  // Increment source addr between each word x-ferred.
    DmaCH->DST_BURST_STEP = transver_size; //FORCE_TRFER_SZ*SIZE_FORCE_BURST;  // Increment dest addr between each word x-ferred.
    // Set up TRANSFER registers:
    DmaCH->TRANSFER_SIZE = transver_size - 1; //(FORCE_TRFER_SZ*SIZE_FORCE_BURST)-1;        // Number of bursts per transfer, DMA interrupt will occur after completed transfer.
    DmaCH->SRC_TRANSFER_STEP = 0; // TRANSFER_STEP is ignored when WRAP occurs.
    DmaCH->DST_TRANSFER_STEP = 0; // TRANSFER_STEP is ignored when WRAP occurs.
    // Set up WRAP registers:
    DmaCH->SRC_WRAP_SIZE = 0; // Wrap source address after N bursts
    DmaCH->SRC_WRAP_STEP = 0; // Step for source wrap
    DmaCH->DST_WRAP_SIZE = 0; // Wrap destination address after N bursts.
    DmaCH->DST_WRAP_STEP = 1; // Step for destination wrap
    // Set up MODE Register:
    if (DmaCH == &(DmaRegs.CH1)) {
        DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH1 = per_int_source; //per_int_source - Source select
        DmaCH->MODE.bit.PERINTSEL = 1;                          // PERINTSEL - Should be hard coded to channel, above now selects source
    } else if (DmaCH == &(DmaRegs.CH2)) {
        DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH2 = per_int_source; //per_int_source - Source select
        DmaCH->MODE.bit.PERINTSEL = 2;                          // PERINTSEL - Should be hard coded to channel, above now selects source
    } else if (DmaCH == &(DmaRegs.CH3)) {
        DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH3 = per_int_source; //per_int_source - Source select
        DmaCH->MODE.bit.PERINTSEL = 3;                          // PERINTSEL - Should be hard coded to channel, above now selects source
    } else if (DmaCH == &(DmaRegs.CH4)) {
        DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH4 = per_int_source; //per_int_source - Source select
        DmaCH->MODE.bit.PERINTSEL = 4;                          // PERINTSEL - Should be hard coded to channel, above now selects source
    } else if (DmaCH == &(DmaRegs.CH5)) {
        DmaClaSrcSelRegs.DMACHSRCSEL2.bit.CH5 = per_int_source; //per_int_source - Source select
        DmaCH->MODE.bit.PERINTSEL = 5;                          // PERINTSEL - Should be hard coded to channel, above now selects source
    } else if (DmaCH == &(DmaRegs.CH6)) {
        DmaClaSrcSelRegs.DMACHSRCSEL2.bit.CH6 = per_int_source; //per_int_source - Source select
        DmaCH->MODE.bit.PERINTSEL = 6;                          // PERINTSEL - Should be hard coded to channel, above now selects source
    } else {
        asm ("      ESTOP0");
        for(;;);
    }
//    DmaCH->MODE.bit.PERINTSEL = 2;                     // PERINTSEL - Should be hard coded to channel, above now selects source
    DmaCH->MODE.bit.PERINTE = PERINT_ENABLE;           // PERINTE - Peripheral interrupt enable
    DmaCH->MODE.bit.ONESHOT = ONESHOT_DISABLE;         // ONESHOT - Oneshot enable
    DmaCH->MODE.bit.CONTINUOUS = CONT_ENABLE;          // CONTINUOUS - Continuous enable
//    DmaCH->MODE.bit.CONTINUOUS = CONT_DISABLE;          // CONTINUOUS - Continuous disable
    DmaCH->MODE.bit.OVRINTE = OVRFLOW_DISABLE;         // OVRINTE - Enable/disable the overflow interrupt
    DmaCH->MODE.bit.DATASIZE = SIXTEEN_BIT;            // DATASIZE - 16-bit/32-bit data size transfers
    DmaCH->MODE.bit.CHINTMODE = CHINT_END;             // CHINTMODE - Generate interrupt to CPU at beginning/end of transfer
    DmaCH->MODE.bit.CHINTE = CHINT_DISABLE;             // CHINTE - Channel Interrupt to  CPU disable
//    DmaCH->MODE.bit.CHINTE = CHINT_ENABLE;             // CHINTE - Channel Interrupt to  CPU enable
    // Clear any spurious flags: Interrupt flags and sync error flags
    DmaCH->CONTROL.bit.PERINTCLR = 1;
    DmaCH->CONTROL.bit.ERRCLR = 1;
    // Starts DMA Channel 3
//    DmaCH->CONTROL.bit.RUN = 1;
    EDIS;
}

void InitADCforCurrents(void)
{
    uint16_t channels[NUM_CURR_CH] = {2, 3};
    InitADC_sampling(&AdcbRegs, channels, NUM_CURR_CH, ADC_TRIGSEL_EPWM12_ADCSOCB);
    InitDMAforADC_sampling(&(DmaRegs.CH2), &AdcbResultRegs.ADCRESULT0, NUM_CURR_CH, &currents[0], CURR_TRFER_SZ*SIZE_CURR_BURST, DMA_ADCBINT1);
}

void InitADCforForces(void)
{
    uint16_t channels[NUM_FORCE_CH] = {2, 3};
    InitADC_sampling(&AdcdRegs, channels, NUM_FORCE_CH, ADC_TRIGSEL_EPWM12_ADCSOCB);
    InitDMAforADC_sampling(&(DmaRegs.CH3), &AdcdResultRegs.ADCRESULT0, NUM_FORCE_CH, &forces[0], FORCE_TRFER_SZ*SIZE_FORCE_BURST, DMA_ADCDINT1);
}

void InitADCforVoltages(void)
{
    uint16_t channels[NUM_VOLT_CH] = {3};
    InitADC_sampling(&AdcaRegs, channels, NUM_VOLT_CH, ADC_TRIGSEL_EPWM12_ADCSOCA);
    InitDMAforADC_sampling(&(DmaRegs.CH4), &AdcaResultRegs.ADCRESULT0, NUM_VOLT_CH, &voltage[0], VOLT_BUF_SZ*SIZE_VOLTAGE_BURST, DMA_ADCAINT1);
}

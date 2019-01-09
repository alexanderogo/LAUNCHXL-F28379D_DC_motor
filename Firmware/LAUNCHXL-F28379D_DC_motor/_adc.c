/*
 * _adc.c
 *
 *  Created on: 7 џэт. 2019 у.
 *      Author: Alexander
 */

#include "F28x_Project.h"
#include "_current_loop.h"
#include "_globals.h"

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

void InitADC(volatile struct ADC_REGS *AdcxRegs, uint16_t channel1, uint16_t channel2);

void InitDMAforADCb(volatile struct ADC_RESULT_REGS *AdcxResultRegs);
void InitDMAforADCd(volatile struct ADC_RESULT_REGS *AdcxResultRegs);

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
    DmaRegs.CH2.MODE.bit.CHINTE = CHINT_DISABLE;             // CHINTE - Channel Interrupt to  CPU enable
//    DmaRegs.CH2.MODE.bit.CHINTE = CHINT_ENABLE;             // CHINTE - Channel Interrupt to  CPU enable
    // Clear any spurious flags: Interrupt flags and sync error flags
    DmaRegs.CH2.CONTROL.bit.PERINTCLR = 1;
    DmaRegs.CH2.CONTROL.bit.ERRCLR = 1;
    // Initialize PIE vector for CPU interrupt
//    PieCtrlRegs.PIEIER7.bit.INTx2 = 1;                      // Enable DMA CH2 interrupt in PIE
    // Starts DMA Channel 2
//    DmaRegs.CH2.CONTROL.bit.RUN = 1;
    EDIS;
//    DMACH2ModeConfig(DMA_ADCBINT1,PERINT_ENABLE,ONESHOT_DISABLE,CONT_ENABLE,SYNC_DISABLE,SYNC_SRC,
//                     OVRFLOW_DISABLE,SIXTEEN_BIT,CHINT_END,CHINT_ENABLE);
//    StartDMACH2();
}

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

void InitDMAforADC(volatile struct CH_REGS *DmaCH, volatile uint16_t *source, volatile uint16_t *dest, uint16_t size, uint16_t per_int_source)
{
    // Configure DMA Channel
    EALLOW;
    // Set up SOURCE address:
    DmaCH->SRC_BEG_ADDR_SHADOW = (Uint32)&AdcdResultRegs.ADCRESULT0;   // Point to beginning of source buffer.
    DmaCH->SRC_ADDR_SHADOW =     (Uint32)&AdcdResultRegs.ADCRESULT0;
    // Set up DESTINATION address:
    DmaCH->DST_BEG_ADDR_SHADOW = (Uint32)dest;  // Point to beginning of destination buffer.
    DmaCH->DST_ADDR_SHADOW =     (Uint32)dest;
    // Set up BURST registers:
    DmaCH->BURST_SIZE.all = 1;     // Number of words(X-1) x-ferred in a burst.
    DmaCH->SRC_BURST_STEP = 1;  // Increment source addr between each word x-ferred.
    DmaCH->DST_BURST_STEP = size; //FORCE_TRFER_SZ*SIZE_FORCE_BURST;  // Increment dest addr between each word x-ferred.
    // Set up TRANSFER registers:
    DmaCH->TRANSFER_SIZE = size - 1; //(FORCE_TRFER_SZ*SIZE_FORCE_BURST)-1;        // Number of bursts per transfer, DMA interrupt will occur after completed transfer.
    DmaCH->SRC_TRANSFER_STEP = 0; // TRANSFER_STEP is ignored when WRAP occurs.
    DmaCH->DST_TRANSFER_STEP = 0; // TRANSFER_STEP is ignored when WRAP occurs.
    // Set up WRAP registers:
    DmaCH->SRC_WRAP_SIZE = 0; // Wrap source address after N bursts
    DmaCH->SRC_WRAP_STEP = 0; // Step for source wrap
    DmaCH->DST_WRAP_SIZE = 0; // Wrap destination address after N bursts.
    DmaCH->DST_WRAP_STEP = 1; // Step for destination wrap
    // Set up MODE Register:
    DmaClaSrcSelRegs.DMACHSRCSEL1.bit.CH3 = per_int_source; //DMA_ADCDINT1;   // persel - Source select
    DmaCH->MODE.bit.PERINTSEL = 2;                     // PERINTSEL - Should be hard coded to channel, above now selects source
    DmaCH->MODE.bit.PERINTE = PERINT_ENABLE;           // PERINTE - Peripheral interrupt enable
    DmaCH->MODE.bit.ONESHOT = ONESHOT_DISABLE;         // ONESHOT - Oneshot enable
//    DmaCH->MODE.bit.CONTINUOUS = CONT_ENABLE;          // CONTINUOUS - Continuous enable
    DmaCH->MODE.bit.CONTINUOUS = CONT_DISABLE;          // CONTINUOUS - Continuous enable
    DmaCH->MODE.bit.OVRINTE = OVRFLOW_DISABLE;         // OVRINTE - Enable/disable the overflow interrupt
    DmaCH->MODE.bit.DATASIZE = SIXTEEN_BIT;            // DATASIZE - 16-bit/32-bit data size transfers
    DmaCH->MODE.bit.CHINTMODE = CHINT_END;             // CHINTMODE - Generate interrupt to CPU at beginning/end of transfer
    DmaCH->MODE.bit.CHINTE = CHINT_DISABLE;             // CHINTE - Channel Interrupt to  CPU enable
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
    InitADC(&AdcbRegs, 2, 3);
    InitDMAforADCb(&AdcbResultRegs);
}

void InitADCforForces(void)
{
    InitADC(&AdcdRegs, 2, 3);
    InitDMAforADC(&(DmaRegs.CH3), &AdcdResultRegs.ADCRESULT0, &forces[0], FORCE_TRFER_SZ*SIZE_FORCE_BURST, DMA_ADCDINT1);
//    InitDMAforADCd(&AdcdResultRegs);
}

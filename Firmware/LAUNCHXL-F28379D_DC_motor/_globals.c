/*
 * _globals.c
 *
 *  Created on: 17 дек. 2018 г.
 *      Author: Alexander
 */
#include "_globals.h"
#include <stdint.h>
#include <filter.h>
#include "fpu_filter.h"

#pragma DATA_SECTION(DMA_Buf, "ramgs0");    // map the TX data to memory
volatile uint16_t DMA_Buf[2*CURR_TRFER_SZ*SIZE_CURR_BURST];
volatile uint16_t *currents = DMA_Buf;
volatile uint16_t *current1 = DMA_Buf;
volatile uint16_t *current2 = DMA_Buf + CURR_TRFER_SZ*SIZE_CURR_BURST;
volatile float current1f;
volatile float current2f;

volatile uint16_t *DMADest;
volatile uint16_t *DMASource;

//#define FIR_ORDER 24
//#define SIGNAL_LENGTH 100

#ifndef __cplusplus
#pragma DATA_SECTION(fir_fixp_curr1, "firfilt");
#else
#pragma DATA_SECTION("firfilt");
#endif
FIR32 fir_fixp_curr1= FIR32_DEFAULTS;
FIR32 fir_fixp_curr2= FIR32_DEFAULTS;

// Define the Delay buffer for the "FIR_ORDER"th order filter
// and place it in "firldb" section.
// Its size should be FIR_ORDER+1
// The delay line buffer must be aligned to a 256 word boundary
#ifndef __cplusplus
#pragma DATA_SECTION(dbuf_curr1,"firldb");
#else
#pragma DATA_SECTION("firfilt");
#endif
int32_t dbuf_curr1[FIR_ORDER+1];
int32_t dbuf_curr2[FIR_ORDER+1];

// Define Constant Coefficient Array  and place it in the "coefffilt"
// section. The size of the array is FIR_ORDER+1
#ifndef __cplusplus
#pragma DATA_SECTION(coeff, "coefffilt");
#else
#pragma DATA_SECTION("coefffilt");
#endif
const int32_t coeff_curr1[FIR_ORDER+1] = {INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,\
                                    INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,\
                                    INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,\
                                    INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,\
                                    INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2};
const int32_t coeff_curr2[FIR_ORDER+1] = {INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,\
                                    INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,\
                                    INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,\
                                    INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,\
                                    INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2,INT32_MAX/(FIR_ORDER+1)*2};

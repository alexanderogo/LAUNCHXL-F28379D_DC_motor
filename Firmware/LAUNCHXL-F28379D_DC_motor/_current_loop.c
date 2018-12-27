/*
 * _current_loop.c
 *
 *  Created on: 17 дек. 2018 г.
 *      Author: Alexander
 */

#include "F28x_Project.h"
#include <stdint.h>
#include <filter.h>
#include "fpu_filter.h"
#include "fpu_vector.h"
#include "_globals.h"
#include "_current_loop.h"


#pragma DATA_SECTION(DMA_Buf, "ramgs0");    // map the TX data to memory
volatile uint16_t DMA_Buf[2*CURR_TRFER_SZ*SIZE_CURR_BURST];
volatile uint16_t *currents = DMA_Buf;
volatile uint16_t *current1 = DMA_Buf;
volatile uint16_t *current2 = DMA_Buf + CURR_TRFER_SZ*SIZE_CURR_BURST;
float current1f;
float current2f;
float current1ff;
float current2ff;

volatile uint16_t *DMADest;
volatile uint16_t *DMASource;

//#define FIR_ORDER_CURR 24

#ifndef __cplusplus
#pragma DATA_SECTION(fir_fixp_curr1, "firfilt_fixp_c1");
#else
#pragma DATA_SECTION("firfilt_fixp_c1");
#endif
FIR32 fir_fixp_curr1= FIR32_DEFAULTS;
#ifndef __cplusplus
#pragma DATA_SECTION(fir_fixp_curr2, "firfilt_fixp_c2");
#else
#pragma DATA_SECTION("firfilt_fixp_c2");
#endif
FIR32 fir_fixp_curr2= FIR32_DEFAULTS;

#ifdef __cplusplus
#pragma DATA_SECTION("firfilt_fp_c1")
#else
#pragma DATA_SECTION(firFPc1, "firfilt_fp_c1")
#endif
FIR_FP  firFPc1 = FIR_FP_DEFAULTS;
#ifdef __cplusplus
#pragma DATA_SECTION("firfilt_fp_c2")
#else
#pragma DATA_SECTION(firFPc2, "firfilt_fp_c2")
#endif
FIR_FP  firFPc2 = FIR_FP_DEFAULTS;


// Define the Delay buffer for the "FIR_ORDER_CURR"th order filter
// and place it in "firldb" section.
// Its size should be FIR_ORDER_CURR+1
// The delay line buffer must be aligned to a 256 word boundary
#ifndef __cplusplus
#pragma DATA_SECTION(dbuf_curr1,"firldb_fixp_c1");
#else
#pragma DATA_SECTION("firldb_fixp_c1");
#endif
int32_t dbuf_curr1[FIR_ORDER_CURR+1];
#ifndef __cplusplus
#pragma DATA_SECTION(dbuf_curr2,"firldb_fixp_c2");
#else
#pragma DATA_SECTION("firldb_fixp_c2");
#endif
int32_t dbuf_curr2[FIR_ORDER_CURR+1];

#ifdef __cplusplus
#pragma DATA_SECTION("firldb_fp_c1")
#else
#pragma DATA_SECTION(dbuf_fp_c1, "firldb_fp_c1")
#endif
float dbuf_fp_c1[FPU_FIR_ORDER_CURR+1];
#ifdef __cplusplus
#pragma DATA_SECTION("firldb_fp_c2")
#else
#pragma DATA_SECTION(dbuf_fp_c2, "firldb_fp_c2")
#endif
float dbuf_fp_c2[FPU_FIR_ORDER_CURR+1];


// Define Constant Coefficient Array  and place it in the "coefffilt"
// section. The size of the array is FIR_ORDER_CURR+1
#ifndef __cplusplus
#pragma DATA_SECTION(coeff_curr1, "coefffilt_fixp_c1");
#else
#pragma DATA_SECTION("coefffilt_fixp_c1");
#endif
const int32_t coeff_curr1[FIR_ORDER_CURR+1] = {INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2};
#ifndef __cplusplus
#pragma DATA_SECTION(coeff_curr2, "coefffilt_fixp_c2");
#else
#pragma DATA_SECTION("coefffilt_fixp_c2");
#endif
const int32_t coeff_curr2[FIR_ORDER_CURR+1] = {INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2};

#ifdef __cplusplus
#pragma DATA_SECTION("coefffilt_fp_c1");
#else
#pragma DATA_SECTION(coeff_fp_c1, "coefffilt_fp_c1");
#endif
const float coeff_fp_c1[FPU_FIR_ORDER_CURR+1] = {1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),\
                                                 1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),\
                                                 1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),\
                                                 1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),\
                                                 1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1)};
#ifdef __cplusplus
#pragma DATA_SECTION("coefffilt_fp_c2");
#else
#pragma DATA_SECTION(coeff_fp_c2, "coefffilt_fp_c2");
#endif
const float coeff_fp_c2[FPU_FIR_ORDER_CURR+1] = {1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),\
                                                 1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),\
                                                 1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),\
                                                 1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),\
                                                 1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1),1/(float)(FPU_FIR_ORDER_CURR+1)};




void init_filter_fixp_current1(void)
{
    fir_fixp_curr1.order       = FIR_ORDER_CURR;
    fir_fixp_curr1.dbuffer_ptr = dbuf_curr1;
    fir_fixp_curr1.coeff_ptr   =(int32_t *)coeff_curr1;
    fir_fixp_curr1.init(&fir_fixp_curr1);
}
void init_filter_fixp_current2(void)
{
    fir_fixp_curr2.order       = FIR_ORDER_CURR;
    fir_fixp_curr2.dbuffer_ptr = dbuf_curr2;
    fir_fixp_curr2.coeff_ptr   =(int32_t *)coeff_curr2;
    fir_fixp_curr2.init(&fir_fixp_curr2);
}
void init_filter_fp_current1(void)
{
    firFPc1.order       = FPU_FIR_ORDER_CURR;
    firFPc1.dbuffer_ptr = dbuf_fp_c1;
    firFPc1.coeff_ptr   =(float *)coeff_fp_c1;
    firFPc1.init(&firFPc1);
}
void init_filter_fp_current2(void)
{
    firFPc2.order       = FPU_FIR_ORDER_CURR;
    firFPc2.dbuffer_ptr = dbuf_fp_c2;
    firFPc2.coeff_ptr   =(float *)coeff_fp_c2;
    firFPc2.init(&firFPc2);
}

void init_current_filters(void)
{
    init_filter_fixp_current1();
    init_filter_fixp_current2();
    init_filter_fp_current1();
    init_filter_fp_current2();
}


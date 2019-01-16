/*
 * _globals.c
 *
 *  Created on: 17 дек. 2018 г.
 *      Author: Alexander
 */
#include "_globals.h"
#include "F28x_Project.h"
#include <stdint.h>


#pragma DATA_SECTION(voltage, "ramgs0");    // map the TX data to memory
volatile uint16_t voltage[SIZE_VOLTAGE_BURST*NUM_VOLT_CH*VOLT_BUF_SZ];

//struct DRIVE_ANG_SENS {
//    uint16_t sr_load;
//    uint16_t sr_rotor;
//};
//struct ANGLES {
//    struct DRIVE_ANG_SENS drive1;
//    struct DRIVE_ANG_SENS drive2;
//};
//struct DRIVE_ANG_SENS_FILT {
//    float32 sr_load;
//    float32 sr_rotor;
//};
//struct ANGLES_FILT {
//    struct DRIVE_ANG_SENS_FILT drive1;
//    struct DRIVE_ANG_SENS_FILT drive2;
//};
/*
static volatile struct ANGLES angles;
static volatile struct ANGLES_FILT anglesf;

struct ANGLES get_angles(void) { return angles; }
void set_angles(const volatile struct ANGLES *angl) { angles = *angl; }
volatile struct ANGLES *angles_ptr(void) { return &angles; }
*/
/*
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

//#define FIR_ORDER_CURR 24

#ifndef __cplusplus
#pragma DATA_SECTION(fir_fixp_curr1, "firfilt");
#else
#pragma DATA_SECTION("firfilt");
#endif
FIR32 fir_fixp_curr1= FIR32_DEFAULTS;
#ifndef __cplusplus
#pragma DATA_SECTION(fir_fixp_curr2, "firfilt");
#else
#pragma DATA_SECTION("firfilt");
#endif
FIR32 fir_fixp_curr2= FIR32_DEFAULTS;

// Define the Delay buffer for the "FIR_ORDER_CURR"th order filter
// and place it in "firldb" section.
// Its size should be FIR_ORDER_CURR+1
// The delay line buffer must be aligned to a 256 word boundary
#ifndef __cplusplus
#pragma DATA_SECTION(dbuf_curr1,"firldb");
#else
#pragma DATA_SECTION("firfilt");
#endif
int32_t dbuf_curr1[FIR_ORDER_CURR+1];
#ifndef __cplusplus
#pragma DATA_SECTION(dbuf_curr2,"firldb");
#else
#pragma DATA_SECTION("firfilt");
#endif
int32_t dbuf_curr2[FIR_ORDER_CURR+1];

// Define Constant Coefficient Array  and place it in the "coefffilt"
// section. The size of the array is FIR_ORDER_CURR+1
#ifndef __cplusplus
#pragma DATA_SECTION(coeff_curr1, "coefffilt");
#else
#pragma DATA_SECTION("coefffilt");
#endif
const int32_t coeff_curr1[FIR_ORDER_CURR+1] = {INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2};
#ifndef __cplusplus
#pragma DATA_SECTION(coeff_curr2, "coefffilt");
#else
#pragma DATA_SECTION("coefffilt");
#endif
const int32_t coeff_curr2[FIR_ORDER_CURR+1] = {INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,\
                                    INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2,INT32_MAX/(FIR_ORDER_CURR+1)*2};
*/

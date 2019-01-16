/*
 * _angle_loop.c
 *
 *  Created on: 12 џэт. 2019 у.
 *      Author: Alexander
 */

#ifndef ANGLE_LOOP_C_
#define ANGLE_LOOP_C_

#include "F28x_Project.h"
#include <stdint.h>
#include <math.h>
#include "fpu_filter.h"
#include "DCLF32.h"
#include "_spi.h"
#include "_globals.h"
#include "_angle_loop.h"

#define ANG_FILT_ORDER  3 // filter order = N - 1
//#ifndef M_PI
#define M_PI            3.14159265358979323846  /* pi */
//#endif

//static volatile struct ANGLES angles;
//static volatile struct ANGLES_FILT anglesf;
static volatile VECTOR2F ang_load_sr;
static volatile VECTOR2F ang_rotor_sr;
static volatile VECTOR2F ang_load;
static volatile VECTOR2F ang_rotor;
static volatile VECTOR2F ang_load_filt;
static volatile VECTOR2F ang_rotor_filt;
static volatile VECTOR2F ang_err_load;
static volatile VECTOR2F ang_err_rotor;
static volatile VECTOR2F ang_target_load;
static volatile VECTOR2F ang_controller_out;

// Define filter structure
#ifdef __cplusplus
#pragma DATA_SECTION("firfilt_fp_ang1")
#else
#pragma DATA_SECTION(firFPang1, "firfilt_fp_ang1")
#endif
static FIR_FP firFPang1 = FIR_FP_DEFAULTS;
static FIR_FP_Handle firFPang1ptr = &firFPang1;
#ifdef __cplusplus
#pragma DATA_SECTION("firfilt_fp_ang2")
#else
#pragma DATA_SECTION(firFPang2, "firfilt_fp_ang2")
#endif
static FIR_FP firFPang2 = FIR_FP_DEFAULTS;
static FIR_FP_Handle firFPang2ptr = &firFPang2;

// Define the Delay buffer for filter
#ifdef __cplusplus
#pragma DATA_SECTION("firldb_fp_ang1")
#else
#pragma DATA_SECTION(dbuf_fp_ang1, "firldb_fp_ang1")
#endif
float dbuf_fp_ang1[ANG_FILT_ORDER+1];
#ifdef __cplusplus
#pragma DATA_SECTION("firldb_fp_ang2")
#else
#pragma DATA_SECTION(dbuf_fp_ang2, "firldb_fp_ang2")
#endif
float dbuf_fp_ang2[ANG_FILT_ORDER+1];

// Define Constant Coefficient Array  and place it in the "coefffilt" section.
#ifdef __cplusplus
#pragma DATA_SECTION("coefffilt_fp_ang1");
#else
#pragma DATA_SECTION(coeff_fp_ang1, "coefffilt_fp_ang1");
#endif
const float coeff_fp_ang1[ANG_FILT_ORDER+1] = {0.25, 0.25, 0.25, 0.25};
#ifdef __cplusplus
#pragma DATA_SECTION("coefffilt_fp_ang2");
#else
#pragma DATA_SECTION(coeff_fp_ang2, "coefffilt_fp_ang2");
#endif
const float coeff_fp_ang2[ANG_FILT_ORDER+1] = {0.25, 0.25, 0.25, 0.25};

static DCL_PID pid_ang1 = PID_DEFAULTS;
static DCL_PID *pid_ang1_ptr = &pid_ang1;
static DCL_PID pid_ang2 = PID_DEFAULTS;
static DCL_PID *pid_ang2_ptr = &pid_ang2;



volatile VECTOR2F *ang_load_sr_ptr(void) { return &ang_load_sr; }
volatile VECTOR2F *ang_rotor_sr_ptr(void) { return &ang_rotor_sr; }
volatile VECTOR2F *ang_load_ptr(void) { return &ang_load; }
volatile VECTOR2F *ang_rotor_ptr(void) { return &ang_rotor; }
volatile VECTOR2F *ang_load_filt_ptr(void) { return &ang_load_filt; }
volatile VECTOR2F *ang_rotor_filt_ptr(void) { return &ang_rotor_filt; }
volatile VECTOR2F *ang_err_load_ptr(void) { return &ang_err_load; }
volatile VECTOR2F *ang_err_rotor_ptr(void) { return &ang_err_rotor; }
volatile VECTOR2F *ang_controller_out_ptr(void) { return &ang_controller_out; }
volatile VECTOR2F *ang_target_load_ptr(void) { return &ang_target_load; }
VECTOR2F get_ang_target_load(void) { return ang_target_load; }
void set_ang_target_load(const volatile VECTOR2F *angles) { ang_target_load = *angles; }


void init_fp_filter(FIR_FP *filt, int order, float *dbuf_ptr, const float *coeff_ptr);
void init_ang_PID(DCL_PID *pid);
void get_angle(uint32_t phase, VECTOR2F *load_sr, VECTOR2F *rotor_sr);
VECTOR2F ang_load_calc(VECTOR2F *angle);
VECTOR2F ang_rotor_calc(VECTOR2F *angle);
VECTOR2F ang_filt_calc(FIR_FP *firFP1, FIR_FP *firFP2, VECTOR2F *angle);
VECTOR2F ang_err_calc(VECTOR2F *signal_target, VECTOR2F *signal_current);
VECTOR2F ang_controller_calc(VECTOR2F *signal);

static float rka = 1.0f;           // reference
static float yka = 0.0f;           // feedback
static float lka = 1.0f;           // saturation
static float uka = 0.0f;           // output

void init_ang_loop(void)
{
    init_fp_filter(&firFPang1, ANG_FILT_ORDER, dbuf_fp_ang1, coeff_fp_ang1);
    init_fp_filter(&firFPang2, ANG_FILT_ORDER, dbuf_fp_ang2, coeff_fp_ang2);
    init_ang_PID(pid_ang1_ptr);
    init_ang_PID(pid_ang2_ptr);
}

void init_fp_filter(FIR_FP *filt, int order, float *dbuf_ptr, const float *coeff_ptr)
{
    filt->order         = order;
    filt->dbuffer_ptr   = dbuf_ptr;
    filt->coeff_ptr     = (float *)coeff_ptr;
    filt->init(filt);
}

void init_ang_PID(DCL_PID *pid)
{
    pid->Kp = 10.0f;
    pid->Ki = 0.0f;
    pid->Kd = 0.0f;
    pid->Kr = 1.0f;
    pid->c1 = 0.0f;
    pid->c2 = 0.0f;
    pid->d2 = 0.0f;
    pid->d3 = 0.0f;
    pid->i10 = 0.0f;
    pid->i14 = 1.0f;
    pid->Umax = 100.0f;
    pid->Umin = -100.0f;
}

void angle_loop_calc(VECTOR2F ang_target, uint32_t divider)
{
    static uint32_t angle_count = 0;
    uint32_t phase = angle_count%divider;
    get_angle(phase, ang_load_sr_ptr(), ang_rotor_sr_ptr());
    if (phase == 3) {
        *ang_target_load_ptr() = ang_target;
        *ang_load_ptr() = ang_load_calc(ang_load_sr_ptr());
        *ang_rotor_ptr() = ang_rotor_calc(ang_rotor_sr_ptr());
//        *ang_load_filt_ptr() = ang_filt_calc(&firFPang1, &firFPang2, ang_load_ptr());
        *ang_load_filt_ptr() = ang_filt_calc(firFPang1ptr, firFPang2ptr, ang_load_ptr());
        *ang_err_load_ptr() = ang_err_calc(ang_target_load_ptr(), ang_load_filt_ptr());
        *ang_controller_out_ptr() = ang_controller_calc(ang_err_load_ptr());
    }
    angle_count++;
}

void get_angle(uint32_t phase, VECTOR2F *load_sr, VECTOR2F *rotor_sr)
{
    // divider should be > 110us / sample time (for f = 50kHz, T = 20us divider > 5.5)
//    static uint32_t ang_count = 0;
    switch (phase) {
    case 0:
        CS4_SPIA_off();
//        angles_ptr()->drive2.sr_rotor = (SpiaRegs.SPIRXBUF & 0x3FFF);
        rotor_sr->drive2 = (SpiaRegs.SPIRXBUF & 0x3FFF);
        CS1_SPIA_on();
        SpiaRegs.SPITXBUF = 0x7FFE;
        break;
    case 1:
        CS1_SPIA_off();
//        angles_ptr()->drive1.sr_load = (SpiaRegs.SPIRXBUF & 0x3FFF);
        load_sr->drive1 = (SpiaRegs.SPIRXBUF & 0x3FFF);
        CS2_SPIA_on();
        SpiaRegs.SPITXBUF = 0x7FFE;
        break;
    case 2:
        CS2_SPIA_off();
//        angles_ptr()->drive1.sr_rotor = (SpiaRegs.SPIRXBUF & 0x3FFF);
        rotor_sr->drive1 = (SpiaRegs.SPIRXBUF & 0x3FFF);
        CS3_SPIA_on();
        SpiaRegs.SPITXBUF = 0x7FFE;
        break;
    case 3:
        CS3_SPIA_off();
//        angles_ptr()->drive2.sr_load = (SpiaRegs.SPIRXBUF & 0x3FFF);
        load_sr->drive2 = (SpiaRegs.SPIRXBUF & 0x3FFF);
        CS4_SPIA_on();
        SpiaRegs.SPITXBUF = 0x7FFE;
        break;
    default:
//        asm ("      ESTOP0");
//        for(;;);
        break;
    }
//    ang_count++;
}

VECTOR2F ang_load_calc(VECTOR2F *angle)
{
    static VECTOR2F base = {0, 0};
    VECTOR2F res;
    res.drive1 = (angle->drive1 - base.drive1)*2.0*M_PI/16383.0;
    res.drive2 = (angle->drive2 - base.drive2)*2.0*M_PI/16383.0;
    return res;
}
VECTOR2F ang_rotor_calc(VECTOR2F *angle)
{
    static VECTOR2F base = {0, 0};
    static VECTOR2F prev = {0, 0};
    static int32_t revol1 = 0;
    static int32_t revol2 = 0;
    VECTOR2F interm;
    VECTOR2F res;
    interm.drive1 = (angle->drive1 - base.drive1)*2.0*M_PI/16383.0;
    interm.drive2 = (angle->drive2 - base.drive2)*2.0*M_PI/16383.0;
    if (interm.drive1 - prev.drive1 > M_PI) {
        revol1--;
    } else if (interm.drive1 - prev.drive1 < - M_PI){
        revol1++;
    }
    if (interm.drive2 - prev.drive2 > M_PI) {
        revol2--;
    } else if (interm.drive1 - prev.drive1 < - M_PI){
        revol2++;
    }
    prev.drive1 = interm.drive1;
    prev.drive2 = interm.drive2;
    res.drive1 = interm.drive1 + revol1*2.0*M_PI;
    res.drive2 = interm.drive2 + revol2*2.0*M_PI;
    return res;
}
VECTOR2F ang_filt_calc(FIR_FP *firFP1, FIR_FP *firFP2, VECTOR2F *angle)
{
    VECTOR2F res;
    firFP1->input = angle->drive1;
    firFP1->calc(firFP1);
    res.drive1 = firFP1->output;
    firFP2->input = angle->drive2;
    firFP2->calc(firFP2);
    res.drive2 = firFP2->output;
    return res;
}
VECTOR2F ang_err_calc(VECTOR2F *signal_target, VECTOR2F *signal_current)
{
    VECTOR2F res;
    res.drive1 = signal_target->drive1 - signal_current->drive1;
    res.drive2 = signal_target->drive2 - signal_current->drive2;
    return res;
}
VECTOR2F ang_controller_calc(VECTOR2F *signal)
{
    VECTOR2F res;
//    res.drive1 = DCL_runPID_C3(pid_ang1_ptr, rka, yka, lka);
//    res.drive2 = DCL_runPID_C3(pid_ang2_ptr, rka, yka, lka);
    res.drive1 = DCL_runPID_C4(pid_ang1_ptr, rka, yka, lka);
    res.drive2 = DCL_runPID_C4(pid_ang2_ptr, rka, yka, lka);
//    res.drive1 = 10*signal->drive1;
//    res.drive2 = 10*signal->drive2;
    return res;
}

#endif /* ANGLE_LOOP_C_ */

/*
 * _current_loop.c
 *
 *  Created on: 17 дек. 2018 г.
 *      Author: Alexander
 */

#include "F28x_Project.h"
#include <stdint.h>
#include <filter.h>
#include "_current_loop.h"
#include "_globals.h"

extern FIR32 fir_fixp_curr1;
extern FIR32 fir_fixp_curr2;
extern int32_t dbuf_curr1[FIR_ORDER+1];
extern int32_t dbuf_curr2[FIR_ORDER+1];
extern int32_t coeff_curr1[FIR_ORDER+1];
extern int32_t coeff_curr2[FIR_ORDER+1];

void init_filter_current1(void)
{
    fir_fixp_curr1.order       = FIR_ORDER;
    fir_fixp_curr1.dbuffer_ptr = dbuf_curr1;
    fir_fixp_curr1.coeff_ptr   =(int32_t *)coeff_curr1;
    fir_fixp_curr1.init(&fir_fixp_curr1);
}
void init_filter_current2(void)
{
    fir_fixp_curr2.order       = FIR_ORDER;
    fir_fixp_curr2.dbuffer_ptr = dbuf_curr2;
    fir_fixp_curr2.coeff_ptr   =(int32_t *)coeff_curr2;
    fir_fixp_curr2.init(&fir_fixp_curr1);
}

void init_current_filters(void)
{
    init_filter_current1();
    init_filter_current2();
}


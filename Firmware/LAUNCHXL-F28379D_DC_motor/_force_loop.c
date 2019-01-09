/*
 * _force_loop.c
 *
 *  Created on: 8 џэт. 2019 у.
 *      Author: Alexander
 */
#include "F28x_Project.h"
#include <stdint.h>
#include "_globals.h"

#pragma DATA_SECTION(DMA_force_buf, "ramgs0");    // map the TX data to memory
volatile uint16_t DMA_force_buf[2*FORCE_TRFER_SZ*SIZE_FORCE_BURST];
volatile uint16_t *forces = DMA_force_buf;
volatile uint16_t *force1 = DMA_force_buf;
volatile uint16_t *force2 = DMA_force_buf + FORCE_TRFER_SZ*SIZE_FORCE_BURST;

void init_force_filters(void)
{

}

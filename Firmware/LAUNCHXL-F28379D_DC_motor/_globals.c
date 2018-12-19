/*
 * _globals.c
 *
 *  Created on: 17 дек. 2018 г.
 *      Author: Alexander
 */
#include "_globals.h"
#include <stdint.h>

#pragma DATA_SECTION(DMA_Buf, "ramgs0");    // map the TX data to memory
volatile uint16_t DMA_Buf[4*SIZE_CURR_ARR];
volatile uint16_t *currents = DMA_Buf;
volatile uint16_t *current1 = DMA_Buf;
volatile uint16_t *current2 = DMA_Buf + 2*SIZE_CURR_ARR;
volatile float current1f;
volatile float current2f;

volatile uint16_t *DMADest;
volatile uint16_t *DMASource;

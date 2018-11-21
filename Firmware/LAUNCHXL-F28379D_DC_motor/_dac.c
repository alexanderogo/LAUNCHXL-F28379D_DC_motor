/*
 * _dac.c
 *
 *  Created on: 20 но€б. 2018 г.
 *      Author: Alexander
 */
#include "_dac.h"
#include "F28x_Project.h"

void InitDAC(void)
{
    InitDACx(&DacaRegs);
    InitDACx(&DacbRegs);
//    InitDACx(&DaccRegs);
}

void InitDACx(volatile struct DAC_REGS *DacxRegs)
{
    EALLOW;
    DacxRegs->DACCTL.bit.DACREFSEL = 1;
    DacxRegs->DACOUTEN.bit.DACOUTEN = 1;
    DacxRegs->DACVALS.all = 0;
    DELAY_US(10);
    EDIS;
}

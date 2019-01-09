/*
 * _spi.c
 *
 *  Created on: 3 џэт. 2019 у.
 *      Author: Alexander
 */


#include "F28x_Project.h"
#include <stdint.h>
#include "_spi.h"


void InitGpioSPIA(void);
void InitGpioSPIA_CS(void);
void InitGpioSPIB(void);
void InitGpioSPIB_CS(void);
void InitSPI(volatile struct SPI_REGS *SpixRegs);

//
// Globals
//

__interrupt void spia_tx_int_isr(void)
{
    static uint32_t SpiaTxIntCount = 0;
    SpiaTxIntCount++;
//    SpiaRegs.SPITXBUF=sdata[0];
    SpiaRegs.SPIFFTX.bit.TXFFINTCLR=1;  // Clear Interrupt flag
    PieCtrlRegs.PIEACK.all|=0x20;       // Issue PIE ACK
}
__interrupt void spia_rx_int_isr(void)
{
    static uint32_t SpiaRxIntCount = 0;
//    if (SpiaRxIntCount < 64) {
//        get_byte[SpiaRxIntCount] = (SpiaRegs.SPIRXBUF & 0x3FFF);
//    } else {
//        rdata[0] = (SpiaRegs.SPIRXBUF & 0x3FFF);
//    }
    SpiaRxIntCount++;
    SpiaRegs.SPIFFRX.bit.RXFFOVFCLR=1;  // Clear Overflow flag
    SpiaRegs.SPIFFRX.bit.RXFFINTCLR=1;  // Clear Interrupt flag
    PieCtrlRegs.PIEACK.all|=0x20;       // Issue PIE ack
}

void InitSPI(volatile struct SPI_REGS *SpixRegs)
{
    //
    // Initialize SPI FIFO registers
    //
//    SpiaRegs.SPIFFTX.all = 0xC021;          // Enable FIFOs, set TX FIFO level to 2
    SpiaRegs.SPIFFTX.all = 0xC001;          // Enable FIFOs, set TX FIFO level to 2
//    SpiaRegs.SPIFFTX.bit.TXFFIENA = 1;      // TX FIFO Interrupt Enable
//    SpiaRegs.SPIFFRX.all = 0x0021;          // Set RX FIFO level to 2
    SpiaRegs.SPIFFRX.all = 0x0001;          // Set RX FIFO level to 2
//    SpiaRegs.SPIFFRX.bit.RXFFIENA = 1;      // RX FIFO Interrupt Enable

    SpiaRegs.SPIFFCT.all = 0x00;

    SpiaRegs.SPIFFTX.bit.TXFIFO=1;
    SpiaRegs.SPIFFRX.bit.RXFIFORESET=1;

    // Initialize SPI-A

    // Set reset low before configuration changes
    // Clock polarity (0 == rising, 1 == falling)
    // 16-bit character
    // Enable loop-back
    SpiaRegs.SPICCR.bit.SPISWRESET = 0;
    SpiaRegs.SPICCR.bit.CLKPOLARITY = 0;
    SpiaRegs.SPICCR.bit.SPICHAR = (16-1);
    SpiaRegs.SPICCR.bit.SPILBK = 0;

    // Enable master (0 == slave, 1 == master)
    // Enable transmission (Talk)
    // Clock phase (0 == normal, 1 == delayed)
    // SPI interrupts are disabled
    SpiaRegs.SPICTL.bit.MASTER_SLAVE = 1;
    SpiaRegs.SPICTL.bit.TALK = 1;
    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;
    SpiaRegs.SPICTL.bit.SPIINTENA = 0;

    // Set the baud rate
    SpiaRegs.SPIBRR.bit.SPI_BIT_RATE = ((200E6 / 4) / 4000E3) - 1;

    // Set FREE bit
    // Halting on a breakpoint will not halt the SPI
    SpiaRegs.SPIPRI.bit.FREE = 1;

    // Release the SPI from reset
    SpiaRegs.SPICCR.bit.SPISWRESET = 1;
}
void InitSPIA(void)
{
    InitGpioSPIA();
    InitGpioSPIA_CS();
    InitSPI(&SpiaRegs);
}

void InitSPIB(void)
{
    InitGpioSPIB();
    InitGpioSPIB_CS();
    InitSPI(&SpibRegs);
}

void InitGpioSPIA(void)
{
    EALLOW;

     //
     // Enable internal pull-up for the selected pins
     //
     // Pull-ups can be enabled or disabled by the user.
     // This will enable the pullups for the specified pins.
     // Comment out other unwanted lines.
     //
     GpioCtrlRegs.GPBPUD.bit.GPIO58 = 0;  // Enable pull-up on GPIO58 (SPISIMOA)
     GpioCtrlRegs.GPBPUD.bit.GPIO59 = 0;  // Enable pull-up on GPIO59 (SPISOMIA)
     GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;  // Enable pull-up on GPIO60 (SPICLKA)
     GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;  // Enable pull-up on GPIO61 (SPISTEA)

     //
     // Set qualification for selected pins to asynch only
     //
     // This will select asynch (no qualification) for the selected pins.
     // Comment out other unwanted lines.
     //
     GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 3; // Asynch input GPIO58 (SPISIMOA)
     GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 3; // Asynch input GPIO59 (SPISOMIA)
     GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 3; // Asynch input GPIO60 (SPICLKA)
     GpioCtrlRegs.GPBQSEL2.bit.GPIO61 = 3; // Asynch input GPIO61 (SPISTEA)

     //
     //Configure SPI-A pins using GPIO regs
     //
     // This specifies which of the possible GPIO pins will be SPI functional
     // pins.
     // Comment out other unwanted lines.
     //
     GpioCtrlRegs.GPBGMUX2.bit.GPIO58 = 3; // Configure GPIO58 as SPISIMOA
     GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 3; // Configure GPIO58 as SPISIMOA
     GpioCtrlRegs.GPBGMUX2.bit.GPIO59 = 3; // Configure GPIO59 as SPISOMIA
     GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 3; // Configure GPIO59 as SPISOMIA
     GpioCtrlRegs.GPBGMUX2.bit.GPIO60 = 3; // Configure GPIO60 as SPICLKA
     GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 3; // Configure GPIO60 as SPICLKA
     GpioCtrlRegs.GPBGMUX2.bit.GPIO61 = 3; // Configure GPIO61 as SPISTEA
     GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 3; // Configure GPIO61 as SPISTEA

     EDIS;
}

void InitGpioSPIB(void)
{
    EALLOW;

     //
     // Enable internal pull-up for the selected pins
     //
     // Pull-ups can be enabled or disabled by the user.
     // This will enable the pullups for the specified pins.
     // Comment out other unwanted lines.
     //
     GpioCtrlRegs.GPBPUD.bit.GPIO63 = 0;  // Enable pull-up on GPIO63 (SPISIMOA)
     GpioCtrlRegs.GPCPUD.bit.GPIO64 = 0;  // Enable pull-up on GPIO64 (SPISOMIA)
     GpioCtrlRegs.GPCPUD.bit.GPIO65 = 0;  // Enable pull-up on GPIO65 (SPICLKA)
     GpioCtrlRegs.GPCPUD.bit.GPIO66 = 0;  // Enable pull-up on GPIO66 (SPISTEA)

     //
     // Set qualification for selected pins to asynch only
     //
     // This will select asynch (no qualification) for the selected pins.
     // Comment out other unwanted lines.
     //
     GpioCtrlRegs.GPBQSEL2.bit.GPIO63 = 3; // Asynch input GPIO63 (SPISIMOA)
     GpioCtrlRegs.GPCQSEL1.bit.GPIO64 = 3; // Asynch input GPIO64 (SPISOMIA)
     GpioCtrlRegs.GPCQSEL1.bit.GPIO65 = 3; // Asynch input GPIO65 (SPICLKA)
     GpioCtrlRegs.GPCQSEL1.bit.GPIO66 = 3; // Asynch input GPIO66 (SPISTEA)

     //
     //Configure SPI-A pins using GPIO regs
     //
     // This specifies which of the possible GPIO pins will be SPI functional
     // pins.
     // Comment out other unwanted lines.
     //
     GpioCtrlRegs.GPBGMUX2.bit.GPIO63 = 3; // Configure GPIO63 as SPISIMOA
     GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 3; // Configure GPIO63 as SPISIMOA
     GpioCtrlRegs.GPCGMUX1.bit.GPIO64 = 3; // Configure GPIO64 as SPISOMIA
     GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 3; // Configure GPIO64 as SPISOMIA
     GpioCtrlRegs.GPCGMUX1.bit.GPIO65 = 3; // Configure GPIO65 as SPICLKA
     GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 3; // Configure GPIO65 as SPICLKA
     GpioCtrlRegs.GPCGMUX1.bit.GPIO66 = 3; // Configure GPIO66 as SPISTEA
     GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 3; // Configure GPIO66 as SPISTEA

     EDIS;
}

void InitGpioSPIA_CS(void)
{
    EALLOW;
    //
    // Initialization GPIOs as outputs
    //
    GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPBSET.bit.GPIO61 = 1;     // Load output latch
    GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPDPUD.bit.GPIO123 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPDSET.bit.GPIO123 = 1;     // Load output latch
    GpioCtrlRegs.GPDMUX2.bit.GPIO123 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPDDIR.bit.GPIO123 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPDPUD.bit.GPIO124 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPDSET.bit.GPIO124 = 1;     // Load output latch
    GpioCtrlRegs.GPDMUX2.bit.GPIO124 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPDDIR.bit.GPIO124 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPDPUD.bit.GPIO125 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPDSET.bit.GPIO125 = 1;     // Load output latch
    GpioCtrlRegs.GPDMUX2.bit.GPIO125 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPDDIR.bit.GPIO125 = 1;     // GPIOXX is output
    EDIS;
}

void InitGpioSPIB_CS(void)
{
    EALLOW;
    //
    // Initialization GPIOs as outputs
    //
    GpioCtrlRegs.GPCPUD.bit.GPIO66 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPCSET.bit.GPIO66 = 1;     // Load output latch
    GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPCDIR.bit.GPIO66 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPEPUD.bit.GPIO131 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPESET.bit.GPIO131 = 1;     // Load output latch
    GpioCtrlRegs.GPEMUX1.bit.GPIO131 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPEDIR.bit.GPIO131 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPASET.bit.GPIO26 = 1;     // Load output latch
    GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO26 = 1;     // GPIOXX is output

    GpioCtrlRegs.GPAPUD.bit.GPIO26 = 0;     // Enable pullup on GPIOXX
    GpioDataRegs.GPASET.bit.GPIO26 = 1;     // Load output latch
    GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0;    // GPIOXX is GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO26 = 1;     // GPIOXX is output
    EDIS;
}

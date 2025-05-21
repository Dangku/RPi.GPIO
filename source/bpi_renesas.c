/*
 * Bananapi specific code
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/mman.h>
#include "cpuinfo.h"

#define DEFINE_BANANAPI_VARS
#include "bananapi.h"
#include "bpi_renesas.h"

/* =======================================================================================
 *
 * Rzv2n specific
 *
 * ========================================================================================
 */
#ifdef RZV2N_SUPPORT
static volatile uint32_t *rzv2n_gpio;

int wiringPiSetupRzv2n (void)
{
    int fd;

    // Open the master /dev/memory device
    if (access("/dev/gpiomem", 0) == 0)
    {
        if ((fd = open("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupRzv2n: Unable to open /dev/gpiomem: %s\n", strerror(errno));
    }
    else
    {
        if (geteuid() != 0)
            (void)wiringPiFailure(WPI_FATAL, "wiringPiSetupRzv2n: Must be root. (Did you forget sudo?)\n");

        if ((fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupRzv2n: Unable to open /dev/mem: %s\n", strerror(errno));
    }


    if (piModel == PI_MODEL_BANANAPIAI2N) {
        rzv2n_gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, RZV2N_GPIO_BASE);
        if (rzv2n_gpio == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupRzv2n: mmap (GPIO) failed: %s\n", strerror(errno));
    }

    return 0;
}

void wiringPiCleanupRzv2n (void)
{
    munmap((void *)rzv2n_gpio, BLOCK_SIZE);
}

/*
 * Sets the mode of a pin to be input, output or PWM output
 *********************************************************************************
 */

void pinModeRzv2n (int pin, int mode)
{
    int port, bit, offset;
    int pmc_shift, pm_shift;
    uint32_t pmc_phyaddr, pmc_mmap_seek;
    uint32_t pfc_phyaddr, pfc_mmap_seek;
    uint32_t pm_phyaddr, pm_mmap_seek;

    offset = OFFSET(pin);
    port = PIN_ID_TO_PORT_OFFSET(offset);
    bit = PIN_ID_TO_PIN(offset);

    //PMC
    pmc_phyaddr = PMC(port);
    pmc_mmap_seek = pmc_phyaddr >> 2;
    pmc_shift = (pmc_phyaddr % 4) * 8;

    //PFC
    pfc_phyaddr = PFC(port);
    pfc_mmap_seek = pfc_phyaddr >> 2;

    //PM
    pm_phyaddr = PM(port);
    pm_mmap_seek = pm_phyaddr >> 2;
    pm_shift = (pm_phyaddr % 4) * 8;

    if (mode == INPUT)
    {
        *(rzv2n_gpio + pmc_mmap_seek) &= ~((0x1 << bit) << pmc_shift);
        *(rzv2n_gpio + pm_mmap_seek) &= ~((0x3 << (bit *2)) << pm_shift);
        *(rzv2n_gpio + pm_mmap_seek) |= ((PM_INPUT << (bit *2)) << pm_shift); 
    }
    else if (mode == OUTPUT)
    {
        *(rzv2n_gpio + pmc_mmap_seek) &= ~((0x1 << bit) << pmc_shift);
        *(rzv2n_gpio + pm_mmap_seek) &= ~((0x3 << (bit *2)) << pm_shift);
        *(rzv2n_gpio + pm_mmap_seek) |= ((PM_OUTPUT << (bit *2)) << pm_shift); 
    }
}


/*
 * Control the internal pull-up/down resistors on a GPIO pin
 * The Arduino only has pull-ups and these are enabled by writing 1
 * to a port when in input mode - this paradigm doesn't quite apply
 * here though.
 *********************************************************************************
 */

void pullUpDnControlRzv2n (int pin, int pud)
{
    int offset, port, port_offset, bit;
    uint32_t pupd_phyaddr, pupd_mmap_seek;
    uint32_t reg;
    int bit_value;

    offset = OFFSET(pin);
    port = PIN_ID_TO_PORT_OFFSET(offset);
    port_offset = port + EXTENDED_REG_OFFSET;
    bit = PIN_ID_TO_PIN(offset);

    pupd_phyaddr = PUPD(port_offset);

    //bit[3:0], lower address = phyaddr
    //bit[7:4], higher address = phyaddr + 4
    if(bit >= 4) {
            bit -= 4;
            pupd_phyaddr += 4;
    }
    pupd_mmap_seek = pupd_phyaddr >> 2;

    /* set bit */
    switch(pud)
    {
        case PUD_UP:
            bit_value = PFC_PULL_UP;
            break;
        case PUD_DOWN:
            bit_value = PFC_PULL_DOWN;
            break;
        case PUD_OFF:
            bit_value = PFC_PULL_DIS0;
            break;
        default:
            bit_value = 0;
        break;
    }

    reg = *(rzv2n_gpio + pupd_mmap_seek) &= ~(3 << (bit * 8));
    *(rzv2n_gpio + pupd_mmap_seek) = reg | (bit_value << (bit * 8));
}

/*
 * Read the value of a given Pin, returning HIGH or LOW
 *********************************************************************************
 */

int digitalReadRzv2n (int pin)
{
    int port, bit, offset;
    int pm_shift, p_shift, pin_shift;
    uint32_t p_phyaddr, p_mmap_seek;
    uint32_t pm_phyaddr, pm_mmap_seek;
    uint32_t pin_phyaddr, pin_mmap_seek;
    uint32_t gpiomode;

    offset = OFFSET(pin);
    port = PIN_ID_TO_PORT_OFFSET(offset);
    bit = PIN_ID_TO_PIN(offset);

    //P, output value
    p_phyaddr = P(port);
    p_mmap_seek = p_phyaddr >> 2;
    p_shift = (p_phyaddr % 4) * 8;

    //PM, output or input
    pm_phyaddr = PM(port);
    pm_mmap_seek = pm_phyaddr >> 2;
    pm_shift = (pm_phyaddr % 4) * 8;

    //PIN, input value
    pin_phyaddr = PIN(port);
    pin_mmap_seek = pin_phyaddr >> 2;
    pin_shift = (pin_phyaddr % 4) * 8;

    gpiomode = *(rzv2n_gpio + pm_mmap_seek) >> pm_shift ;
    gpiomode = (gpiomode >> (bit * 2)) & 0x3;

    if (gpiomode == PM_INPUT) {
        return !!((*(rzv2n_gpio + pin_mmap_seek) >> pin_shift) & (1 <<bit));
    } else if (gpiomode == PM_OUTPUT) {
        return !!((*(rzv2n_gpio + p_mmap_seek) >> p_shift) & (1 <<bit));
    }

    return 0;  //high-z
}

/*
 * Set an output bit
 *********************************************************************************
 */

void digitalWriteRzv2n (int pin, int value)
{
    int port, bit, offset;
    int p_shift;
    uint32_t p_phyaddr, p_mmap_seek;

    offset = OFFSET(pin);
    port = PIN_ID_TO_PORT_OFFSET(offset);
    bit = PIN_ID_TO_PIN(offset);

    //P
    p_phyaddr = P(port);
    p_mmap_seek = p_phyaddr >> 2;
    p_shift = (p_phyaddr % 4) * 8;

    if (value == LOW)
        *(rzv2n_gpio + p_mmap_seek) &= ~((1 << bit) << p_shift);
    else
        *(rzv2n_gpio + p_mmap_seek) |= ((1 << bit) << p_shift);
}

/*
 * ead the analog value of a given Pin.
 * here is no on-board Pi analog hardware,
 * so this needs to go to a new node.
 *********************************************************************************
 */

int analogReadRzv2n (int pin)
{
    wiringPiFailure(WPI_FATAL, "analogReadRzv2n: No ADC pin on Bananapi\n");
    return 0;
}

/*
 * Write the analog value to the given Pin.
 * There is no on-board Pi analog hardware,
 * so this needs to go to a new node.
 *********************************************************************************
 */

void analogWriteRzv2n (int pin, int value)
{
    wiringPiFailure(WPI_FATAL, "analogWriteRzv2n: No DAC pin on Bananapi\n");
}

/*
 * Gets the mode of a pin to be input, output
 *********************************************************************************
 */

int pinGetModeRzv2n (int pin)
{
    int port, bit, offset;
    int pmc_shift, pm_shift;
    uint32_t pmc_phyaddr, pmc_mmap_seek;
    uint32_t pfc_phyaddr, pfc_mmap_seek;
    uint32_t pm_phyaddr, pm_mmap_seek;
    uint32_t mode;
    uint32_t gpiomode;

    offset = OFFSET(pin);
    port = PIN_ID_TO_PORT_OFFSET(offset);
    bit = PIN_ID_TO_PIN(offset);

    //PMC
    pmc_phyaddr = PMC(port);
    pmc_mmap_seek = pmc_phyaddr >> 2;
    pmc_shift = (pmc_phyaddr % 4) * 8;

    //PFC
    pfc_phyaddr = PFC(port);
    pfc_mmap_seek = pfc_phyaddr >> 2;

    //PM
    pm_phyaddr = PM(port);
    pm_mmap_seek = pm_phyaddr >> 2;
    pm_shift = (pm_phyaddr % 4) * 8;

    mode = (*(rzv2n_gpio + pmc_mmap_seek) >> pmc_shift) & (1<< bit);
    if (!mode)
    {
        //gpio mode, 0->hi-z, 1->input, 2->output
        gpiomode = *(rzv2n_gpio + pm_mmap_seek) >> pm_shift;
        gpiomode = (gpiomode >> (bit * 2)) & 0x3;
        if (gpiomode == PM_OUTPUT)
            return OUTPUT;
        else if (gpiomode == PM_INPUT)
            return INPUT;
    }

    return INPUT;
}

void setInfoRzv2n(char *hardware, void *vinfo)
{
   rpi_info *info = (rpi_info *)vinfo;

   if (strstr(hardware, "BPI-AI2N") ||
       strstr(hardware, "BananaPi AI2N"))
   {
       piModel = PI_MODEL_BANANAPIAI2N;
       info->type = "BPI-AI2N";
       info->p1_revision = 3;
       info->ram = "8192M";
       info->manufacturer = "Bananapi";
       info->processor = "Renesas RZV2N";
   }
   else
       wiringPiFailure(WPI_FATAL, "setInfoRzv2n: This code should only be called for Bananapi\n");
   
    return;
}

void setMappingPtrsRzv2n(void)
{
    if (piModel == PI_MODEL_BANANAPIAI2N)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiAI2N;
        bcm_to_rzv2ngpio = &bcmToOGpioBananapiAI2N;
    }
}
#endif /* end RZV2N_SUPPORT */

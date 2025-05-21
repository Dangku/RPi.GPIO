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
#include "bpi_spacemit.h"

static volatile uint32_t *spacemit_gpio;
static volatile uint32_t *spacemit_pinctrl;

/* =======================================================================================
 *
 * Spacemit specific
 *
 * ========================================================================================
 */
#ifdef SPACEMIT_SUPPORT
int wiringPiSetupSpacemit (void)
{
    int fd;

    // Open the master /dev/memory device
    if (access("/dev/gpiomem", 0) == 0)
    {
        if ((fd = open("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupSpacemit: Unable to open /dev/gpiomem: %s\n", strerror(errno));
    }
    else
    {
        if (geteuid() != 0)
            (void)wiringPiFailure(WPI_FATAL, "wiringPiSetupSpacemit: Must be root. (Did you forget sudo?)\n");

        if ((fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupSpacemit: Unable to open /dev/mem: %s\n", strerror(errno));
    }


    if (piModel == PI_MODEL_BANANAPIF3) {
        spacemit_gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SPACEMIT_GPIO_BASE);
        if (spacemit_gpio == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupSpacemit: mmap (GPIO) failed: %s\n", strerror(errno));

        spacemit_pinctrl = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SPACEMIT_PINCTRL_BASE);
        if (spacemit_pinctrl == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupSpacemit: mmap (PINCTRL) failed: %s\n", strerror(errno));
        
    }

    return 0;
}

void wiringPiCleanupSpacemit (void)
{
    munmap((void *)spacemit_gpio, BLOCK_SIZE);
}

/*
 * Sets the mode of a pin to be input, output
 *********************************************************************************
 */

void pinModeSpacemit (int pin, int mode)
{
    int bank, index, phyaddr, mmap_seek, p_phyaddr, p_mmap_seek;

    //pinmux mode
    p_phyaddr = F3_MFPR(pin);
    p_mmap_seek = p_phyaddr >> 2;
	
    //gpio mode
    bank = pin >> 5;
    index = pin - (bank << 5);
    phyaddr = (bank == 3) ? F3_BANK3_OFFSET : F3_BANK012_OFFSET(bank);

    if (mode == INPUT)
    {
        if (piModel == PI_MODEL_BANANAPIF3) {
            *(spacemit_pinctrl + p_mmap_seek) &= ~(F3_AF_SEL_MASK);
            *(spacemit_pinctrl + p_mmap_seek) |= (F3_GPIO_ALT(pin) << F3_AF_SEL_OFFSET);
            mmap_seek = (phyaddr + F3_GCDR) >> 2;
            *(spacemit_gpio + mmap_seek) |= (1 << index);
        }
        else
            wiringPiFailure(WPI_FATAL, "pinModeSpacemit: This code should only be called for Bananapi\n");
    }
    else if (mode == OUTPUT)
    {
        if (piModel == PI_MODEL_BANANAPIF3) {
            *(spacemit_pinctrl + p_mmap_seek) &= ~(F3_AF_SEL_MASK);
            *(spacemit_pinctrl + p_mmap_seek) |= (F3_GPIO_ALT(pin) << F3_AF_SEL_OFFSET);
            mmap_seek = (phyaddr + F3_GSDR) >> 2;
            *(spacemit_gpio + mmap_seek) |=  (1 << index);
        }
        else
            wiringPiFailure(WPI_FATAL, "pinModeSpacemit: This code should only be called for Bananapi\n");
    }
}


/*
 * Control the internal pull-up/down resistors on a GPIO pin
 * The Arduino only has pull-ups and these are enabled by writing 1
 * to a port when in input mode - this paradigm doesn't quite apply
 * here though.
 *********************************************************************************
 */

void pullUpDnControlSpacemit (int pin, int pud)
{
    int phyaddr, mmap_seek;
    int bit_value = 0;

    /* set bit */
    switch(pud)
    {
        case PUD_UP:
            bit_value = F3_PULL_UP;
            break;
        case PUD_DOWN:
            bit_value = F3_PULL_DOWN;
            break;
        case PUD_OFF:
            bit_value = F3_PULL_DIS;
            break;
        default:
            break;
    }

    if (piModel == PI_MODEL_BANANAPIF3) {
        phyaddr = F3_MFPR(pin);
        mmap_seek = phyaddr >> 2;
        *(spacemit_pinctrl + mmap_seek) &= ~(F3_PULL_MASK);
        *(spacemit_pinctrl + mmap_seek) |= (bit_value & 7) << F3_PULL_OFFSET;
    }
    else
        wiringPiFailure(WPI_FATAL, "pullUpDnControlSpacemit: This code should only be called for Bananapi\n");
}

/*
 * Read the value of a given Pin, returning HIGH or LOW
 *********************************************************************************
 */

int digitalReadSpacemit (int pin)
{
    int bank, index, phyaddr, mmap_seek;

    bank = pin >> 5;
    index = pin - (bank << 5);
    phyaddr = (bank == 3) ? F3_BANK3_OFFSET : F3_BANK012_OFFSET(bank);
    mmap_seek = (phyaddr + F3_GPLR) >> 2;

    if (piModel == PI_MODEL_BANANAPIF3) {
        if (*(spacemit_gpio + mmap_seek) & (1 << index))
            return HIGH;
        else
            return LOW;
    }
    else
        wiringPiFailure(WPI_FATAL, "digitalReadSpacemit: This code should only be called for Bananapi\n");

    return 0;
}

/*
 * Set an output bit
 *********************************************************************************
 */

void digitalWriteSpacemit (int pin, int value)
{
    int bank, index, phyaddr, mmap_seek;


    bank = pin >> 5;
    index = pin - (bank << 5);
    phyaddr = (bank == 3) ? F3_BANK3_OFFSET : F3_BANK012_OFFSET(bank);

    if (piModel == PI_MODEL_BANANAPIF3) {
        if (value == LOW)
            mmap_seek = (phyaddr + F3_GPCR) >> 2;
        else
            mmap_seek = (phyaddr + F3_GPSR) >> 2;

        *(spacemit_gpio + mmap_seek) |= (1 << index);
    }
    else
        wiringPiFailure(WPI_FATAL, "digitalWriteSpacemit: This code should only be called for Bananapi\n");
}

/*
 * ead the analog value of a given Pin.
 * here is no on-board Pi analog hardware,
 * so this needs to go to a new node.
 *********************************************************************************
 */

int analogReadSpacemit (int pin)
{
    wiringPiFailure(WPI_FATAL, "analogReadSpacemit: No ADC pin on Bananapi\n");
    return 0;
}

/*
 * Write the analog value to the given Pin.
 * There is no on-board Pi analog hardware,
 * so this needs to go to a new node.
 *********************************************************************************
 */

void analogWriteSpacemit (int pin, int value)
{
    wiringPiFailure(WPI_FATAL, "analogWriteSpacemit: No DAC pin on Bananapi\n");
}

/*
 * Gets the mode of a pin to be input, output
 *********************************************************************************
 */

int pinGetModeSpacemit (int pin)
{
    int bank, index, phyaddr, mmap_seek, p_phyaddr, p_mmap_seek, af_sel;
    int alt = 0;

    //get pinmux_mode
    p_phyaddr = F3_MFPR(pin);
    p_mmap_seek = p_phyaddr >> 2;
    

    if (piModel == PI_MODEL_BANANAPIF3) {
        af_sel = *(spacemit_pinctrl + p_mmap_seek) & F3_AF_SEL_MASK;

        if (af_sel == F3_GPIO_ALT(pin)) {
                //gpio mode
                bank = pin >> 5;
                index = pin - (bank << 5);
                phyaddr = (bank == 3) ? F3_BANK3_OFFSET : F3_BANK012_OFFSET(bank);
                mmap_seek = (phyaddr + F3_GPDR) >> 2;
                alt = (*(spacemit_gpio + mmap_seek) >> index) & 1;
        } else {
                alt = af_sel + 2;
        }
    }
    else
        wiringPiFailure(WPI_FATAL, "pinGetModeSpacemit: This code should only be called for Bananapi\n");

    return alt;
}

void setInfoSpacemit(char *hardware, void *vinfo)
{
   rpi_info *info = (rpi_info *)vinfo;

   if (strstr(hardware, "BananaPi BPI-F3") ||
       strstr(hardware, "k1-x deb1"))
   {
       piModel = PI_MODEL_BANANAPIF3;
       info->type = "BPI-F3";
       info->p1_revision = 3;
       info->ram = "2048M/4096M/8192M/16384M";
       info->manufacturer = "Bananapi";
       info->processor = "SPACEMIT K1";
   }
   else
       wiringPiFailure(WPI_FATAL, "setInfoSpacemit: This code should only be called for Bananapi\n");
   
    return;
}

void setMappingPtrsSpacemit(void)
{
    if (piModel == PI_MODEL_BANANAPIF3)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiF3;
        bcm_to_spacemitgpio = &bcmToOGpioBananapiF3;
    }
}
#endif /* end SPACEMIT_SUPPORT */

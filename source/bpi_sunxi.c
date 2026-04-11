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
#include "bpi_sunxi.h"

/* =======================================================================================
 *
 * Sun50iw9 specific
 *
 * ========================================================================================
 */
#ifdef SUN50IW9_SUPPORT
static volatile uint32_t *sun50iw9_gpio;

int wiringPiSetupSun50iw9 (void)
{
    int fd;

    // Open the master /dev/memory device
    if (access("/dev/gpiomem", 0) == 0)
    {
        if ((fd = open("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupSun50iw9: Unable to open /dev/gpiomem: %s\n", strerror(errno));
    }
    else
    {
        if (geteuid() != 0)
            (void)wiringPiFailure(WPI_FATAL, "wiringPiSetupSun50iw9: Must be root. (Did you forget sudo?)\n");

        if ((fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupSun50iw9: Unable to open /dev/mem: %s\n", strerror(errno));
    }


    if (piModel == PI_MODEL_BANANAPIM4BERRY || piModel == PI_MODEL_BANANAPIM4ZERO || piModel == PI_MODEL_BANANAPIM4ZERO_V1) {
        sun50iw9_gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SUN50IW9_GPIO_BASE);
        if (sun50iw9_gpio == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupSun50iw9: mmap (GPIO) failed: %s\n", strerror(errno));
    }

    return 0;
}

void wiringPiCleanupSun50iw9 (void)
{
    munmap((void *)sun50iw9_gpio, BLOCK_SIZE);
}

/*
 * Sets the mode of a pin to be input, output or PWM output
 *********************************************************************************
 */

void pinModeSun50iw9 (int pin, int mode)
{
    int bank, index, offset, phyaddr, mmap_seek;

    //Sun50iw9: For our purposes pin comes in as gpio, original code converted
    //pin to gpio and kept origPin as pin#
	
    bank = pin >> 5;
    index = pin - (bank << 5);
    offset = ((index - ((index >> 3) << 3)) << 2);
    phyaddr = (bank * 36) + ((index >> 3) << 2);
    mmap_seek = phyaddr >> 2;

    if (mode == INPUT)
    {
        if (piModel == PI_MODEL_BANANAPIM4BERRY||
            piModel == PI_MODEL_BANANAPIM4ZERO || piModel == PI_MODEL_BANANAPIM4ZERO_V1) {
            *(sun50iw9_gpio + mmap_seek) &= ~(7 << offset);
        }
        else
            wiringPiFailure(WPI_FATAL, "pinModeSun50iw9: This code should only be called for Bananapi\n");
    }
    else if (mode == OUTPUT)
    {
        if (piModel == PI_MODEL_BANANAPIM4BERRY||
            piModel == PI_MODEL_BANANAPIM4ZERO || piModel == PI_MODEL_BANANAPIM4ZERO_V1) {
            *(sun50iw9_gpio + mmap_seek) &= ~(7 << offset);
            *(sun50iw9_gpio + mmap_seek) |=  (1 << offset);
        }
        else
            wiringPiFailure(WPI_FATAL, "pinModeSun50iw9: This code should only be called for Bananapi\n");
    }
}


/*
 * Control the internal pull-up/down resistors on a GPIO pin
 * The Arduino only has pull-ups and these are enabled by writing 1
 * to a port when in input mode - this paradigm doesn't quite apply
 * here though.
 *********************************************************************************
 */

void pullUpDnControlSun50iw9 (int pin, int pud)
{
    int bank, index, offset, phyaddr, mmap_seek, bit_value;

    bank = pin >> 5;
    index = pin - (bank << 5);
    offset = ((index % 16) << 1);
    phyaddr = (bank * 36) + ((index >> 4) << 2) + 0x1c;
    mmap_seek = phyaddr >> 2;

    switch (pud) {
        case PUD_OFF:
		bit_value = 0;
		break;
	case PUD_UP:
		bit_value = 1;
		break;
	case PUD_DOWN:
		bit_value = 2;
		break;
	default:
		bit_value = 0;
		break;
    }

    if (piModel == PI_MODEL_BANANAPIM4BERRY||
            piModel == PI_MODEL_BANANAPIM4ZERO || piModel == PI_MODEL_BANANAPIM4ZERO_V1) {
        *(sun50iw9_gpio + mmap_seek) &= ~(3 << offset);
        *(sun50iw9_gpio + mmap_seek) |= (bit_value & 3) << offset;
    }
    else
        wiringPiFailure(WPI_FATAL, "pullUpDnControlSun50iw9: This code should only be called for Bananapi\n");
}

/*
 * Read the value of a given Pin, returning HIGH or LOW
 *********************************************************************************
 */

int digitalReadSun50iw9 (int pin)
{
    int bank, index, phyaddr, mmap_seek, retval=0;

    bank = pin >> 5;
    index = pin - (bank << 5);
    phyaddr = (bank * 36) + 0x10;
    mmap_seek = phyaddr >> 2;

    if (piModel == PI_MODEL_BANANAPIM4BERRY||
            piModel == PI_MODEL_BANANAPIM4ZERO || piModel == PI_MODEL_BANANAPIM4ZERO_V1) {
        if (*(sun50iw9_gpio + mmap_seek) & (1 << index))
            retval = HIGH;
        else
            retval = LOW;
    }
    else
        wiringPiFailure(WPI_FATAL, "digitalReadSun50iw9: This code should only be called for Bananapi\n");

    return retval;
}

/*
 * Set an output bit
 *********************************************************************************
 */

void digitalWriteSun50iw9 (int pin, int value)
{
    int bank, index, phyaddr, mmap_seek;

    bank = pin >> 5;
    index = pin - (bank << 5);
    phyaddr = (bank * 36) + 0x10;
    mmap_seek = phyaddr >> 2;


    if (piModel == PI_MODEL_BANANAPIM4BERRY||
            piModel == PI_MODEL_BANANAPIM4ZERO || piModel == PI_MODEL_BANANAPIM4ZERO_V1) {
        if (value == LOW)
            *(sun50iw9_gpio + mmap_seek) &= ~(1 << index);
        else
            *(sun50iw9_gpio + mmap_seek) |= (1 << index);
    }
    else
        wiringPiFailure(WPI_FATAL, "digitalWriteSun50iw9: This code should only be called for Bananapi\n");
}

/*
 * ead the analog value of a given Pin.
 * here is no on-board Pi analog hardware,
 * so this needs to go to a new node.
 *********************************************************************************
 */

int analogReadSun50iw9 (int pin)
{
    wiringPiFailure(WPI_FATAL, "analogReadSun50iw9: No ADC pin on Bananapi\n");
    return 0;
}

/*
 * Write the analog value to the given Pin.
 * There is no on-board Pi analog hardware,
 * so this needs to go to a new node.
 *********************************************************************************
 */

void analogWriteSun50iw9 (int pin, int value)
{
    wiringPiFailure(WPI_FATAL, "analogWriteSun50iw9: No DAC pin on Bananapi\n");
}

/*
 * Gets the mode of a pin to be input, output
 *********************************************************************************
 */

int pinGetModeSun50iw9 (int pin)
{
    int bank, index, offset, phyaddr, mmap_seek, retval=0;

    bank = pin >> 5;
    index = pin - (bank << 5);
    offset = ((index - ((index >> 3) << 3)) << 2);
    phyaddr = (bank * 36) + ((index >> 3) << 2);
    mmap_seek = phyaddr >> 2;

    if (piModel == PI_MODEL_BANANAPIM4BERRY||
            piModel == PI_MODEL_BANANAPIM4ZERO || piModel == PI_MODEL_BANANAPIM4ZERO_V1) {
            retval = (*(sun50iw9_gpio + mmap_seek) >> offset) & 7;
    }
    else
        wiringPiFailure(WPI_FATAL, "pinGetModeSun50iw9: This code should only be called for Bananapi\n");

    return retval;
}

void setInfoSun50iw9(char *hardware, void *vinfo)
{
   rpi_info *info = (rpi_info *)vinfo;

   if (strstr(hardware, "BPI-M4Berry") ||
       strstr(hardware, "BananaPi M4 Berry"))
   {
       piModel = PI_MODEL_BANANAPIM4BERRY;
       info->type = "BPI-M4Berry";
       info->p1_revision = 3;
       info->ram = "2048M/4096M";
       info->manufacturer = "Bananapi";
       info->processor = "AW SUN50IW9";
   }
   else if (strstr(hardware, "BPI-M4Zero") ||
       strstr(hardware, "BananaPi M4 Zero") ||
       strstr(hardware, "BananaPi BPI-M4-Zero v2"))
   {
       piModel = PI_MODEL_BANANAPIM4ZERO;
       info->type = "BPI-M4Zero";
       info->p1_revision = 3;
       info->ram = "2048M/4096M";
       info->manufacturer = "Bananapi";
       info->processor = "AW SUN50IW9";
   }
   else if (strstr(hardware, "BananaPi BPI-M4-Zero"))
   {
       piModel = PI_MODEL_BANANAPIM4ZERO_V1;
       info->type = "BPI-M4Zero V1";
       info->p1_revision = 3;
       info->ram = "2048M/4096M";
       info->manufacturer = "Bananapi";
       info->processor = "AW SUN50IW9";
   }
   else
       wiringPiFailure(WPI_FATAL, "setInfoSun50iw9: This code should only be called for Bananapi\n");
   
    return;
}

void setMappingPtrsSun50iw9(void)
{
    if (piModel == PI_MODEL_BANANAPIM4BERRY)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiM4Berry;
        bcm_to_sun50iw9gpio = &bcmToOGpioBananapiM4Berry;
    }
    else if (piModel == PI_MODEL_BANANAPIM4ZERO)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiM4Zero;
        bcm_to_sun50iw9gpio = &bcmToOGpioBananapiM4Zero;
    }
    else if (piModel == PI_MODEL_BANANAPIM4ZERO_V1)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiM4ZeroV1;
        bcm_to_sun50iw9gpio = &bcmToOGpioBananapiM4ZeroV1;
    }
}
#endif /* end SUN50IW9_SUPPORT */

/* =======================================================================================
 *
 * Sun55iw3 specific
 *
 * ========================================================================================
 */
#ifdef SUN55IW3_SUPPORT
static volatile uint32_t *sun55iw3_gpio;
static volatile uint32_t *sun55iw3_gpior;

static int isGpioRPin(int pin)
{
    if (pin >= SUN55IW3_GPIOR_PIN_START && pin <= SUN55IW3_GPIO_PIN_END)
        return 1;
    else
        return 0;
}

int wiringPiSetupSun55iw3 (void)
{
    int fd;

    printf("wiringPiSetupSun55iw3()\n");

    // Open the master /dev/memory device
    if (access("/dev/gpiomem", 0) == 0)
    {
        if ((fd = open("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupSun55iw3: Unable to open /dev/gpiomem: %s\n", strerror(errno));
    }
    else
    {
        if (geteuid() != 0)
            (void)wiringPiFailure(WPI_FATAL, "wiringPiSetupSun55iw3: Must be root. (Did you forget sudo?)\n");

        if ((fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupSun55iw3: Unable to open /dev/mem: %s\n", strerror(errno));
    }


    if (piModel == PI_MODEL_BANANAPIF5) {
        sun55iw3_gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SUN55IW3_GPIO_BASE);
        if (sun55iw3_gpio == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupSun55iw3: mmap (GPIO) failed: %s\n", strerror(errno));

        sun55iw3_gpior = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, SUN55IW3_GPIO_R_BASE);
        if (sun55iw3_gpior == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupSun55iw3: mmap (GPIO_R) failed: %s\n", strerror(errno));
    }

    return 0;
}

void wiringPiCleanupSun55iw3 (void)
{
    munmap((void *)sun55iw3_gpio, BLOCK_SIZE);
    munmap((void *)sun55iw3_gpior, BLOCK_SIZE);
}

/*
 * Sets the mode of a pin to be input, output or PWM output
 *********************************************************************************
 */

void pinModeSun55iw3 (int pin, int mode)
{
    int bank, index, offset, phyaddr, mmap_seek;

    //Sun55iw3: For our purposes pin comes in as gpio, original code converted
    //pin to gpio and kept origPin as pin#
	
    bank = pin >> 5;
    index = pin - (bank << 5);
    offset = ((index - ((index >> 3) << 3)) << 2);
    if (isGpioRPin(pin) )
        phyaddr = ((bank - 11) * 48) + ((index >> 3) << 2);
    else
        phyaddr = (bank * 48) + ((index >> 3) << 2);
    mmap_seek = phyaddr >> 2;

    if (mode == INPUT)
    {
        if (piModel == PI_MODEL_BANANAPIF5) {
            *((isGpioRPin(pin) ? sun55iw3_gpior : sun55iw3_gpio) + mmap_seek) &= ~(0xf << offset);
        }
        else
            wiringPiFailure(WPI_FATAL, "pinModeSun55iw3: This code should only be called for Bananapi\n");
    }
    else if (mode == OUTPUT)
    {
        if (piModel == PI_MODEL_BANANAPIF5) {
            *((isGpioRPin(pin) ? sun55iw3_gpior : sun55iw3_gpio) + mmap_seek) &= ~(0xf << offset);
            *((isGpioRPin(pin) ? sun55iw3_gpior : sun55iw3_gpio) + mmap_seek) |=  (1 << offset);
        }
        else
            wiringPiFailure(WPI_FATAL, "pinModeSun55iw3: This code should only be called for Bananapi\n");
    }
}


/*
 * Control the internal pull-up/down resistors on a GPIO pin
 * The Arduino only has pull-ups and these are enabled by writing 1
 * to a port when in input mode - this paradigm doesn't quite apply
 * here though.
 *********************************************************************************
 */

void pullUpDnControlSun55iw3 (int pin, int pud)
{
    int bank, index, offset, phyaddr, mmap_seek, bit_value;

    bank = pin >> 5;
    index = pin - (bank << 5);
    offset = ((index % 16) << 1);
    if (isGpioRPin(pin) )
        phyaddr = ((bank - 11) * 48) + ((index >> 4) << 2) + 0x24;
    else
        phyaddr = (bank * 48) + ((index >> 4) << 2) + 0x24;
    mmap_seek = phyaddr >> 2;

    switch (pud) {
        case PUD_OFF:
		bit_value = 0;
		break;
	case PUD_UP:
		bit_value = 1;
		break;
	case PUD_DOWN:
		bit_value = 2;
		break;
	default:
		bit_value = 0;
		break;
    }

    if (piModel == PI_MODEL_BANANAPIF5) {
        *((isGpioRPin(pin) ? sun55iw3_gpior : sun55iw3_gpio) + mmap_seek) &= ~(3 << offset);
        *((isGpioRPin(pin) ? sun55iw3_gpior : sun55iw3_gpio) + mmap_seek) |= (bit_value & 3) << offset;
    }
    else
        wiringPiFailure(WPI_FATAL, "pullUpDnControlSun55iw3: This code should only be called for Bananapi\n");
}

/*
 * Read the value of a given Pin, returning HIGH or LOW
 *********************************************************************************
 */

int digitalReadSun55iw3 (int pin)
{
    int bank, index, phyaddr, mmap_seek, retval=0;

    bank = pin >> 5;
    index = pin - (bank << 5);
    if (isGpioRPin(pin) )
        phyaddr = ((bank -11) * 48) + 0x10;
    else
        phyaddr = (bank * 48) + 0x10;
    mmap_seek = phyaddr >> 2;

    if (piModel == PI_MODEL_BANANAPIF5) {
        if (*((isGpioRPin(pin) ? sun55iw3_gpior : sun55iw3_gpio) + mmap_seek) & (1 << index))
            retval = HIGH;
        else
            retval = LOW;
    }
    else
        wiringPiFailure(WPI_FATAL, "digitalReadSun55iw3: This code should only be called for Bananapi\n");

    return retval;
}

/*
 * Set an output bit
 *********************************************************************************
 */

void digitalWriteSun55iw3 (int pin, int value)
{
    int bank, index, phyaddr, mmap_seek;

    bank = pin >> 5;
    index = pin - (bank << 5);
    if (isGpioRPin(pin) )
        phyaddr = ((bank - 11) * 48) + 0x10;
    else
        phyaddr = (bank * 48) + 0x10;
    mmap_seek = phyaddr >> 2;


    if (piModel == PI_MODEL_BANANAPIF5) {
        if (value == LOW)
            *((isGpioRPin(pin) ? sun55iw3_gpior : sun55iw3_gpio) + mmap_seek) &= ~(1 << index);
        else
            *((isGpioRPin(pin) ? sun55iw3_gpior : sun55iw3_gpio) + mmap_seek) |= (1 << index);
    }
    else
        wiringPiFailure(WPI_FATAL, "digitalWriteSun55iw3: This code should only be called for Bananapi\n");
}

/*
 * ead the analog value of a given Pin.
 * here is no on-board Pi analog hardware,
 * so this needs to go to a new node.
 *********************************************************************************
 */

int analogReadSun55iw3 (int pin)
{
    wiringPiFailure(WPI_FATAL, "analogReadSun55iw3: No ADC pin on Bananapi\n");
    return 0;
}

/*
 * Write the analog value to the given Pin.
 * There is no on-board Pi analog hardware,
 * so this needs to go to a new node.
 *********************************************************************************
 */

void analogWriteSun55iw3 (int pin, int value)
{
    wiringPiFailure(WPI_FATAL, "analogWriteSun55iw3: No DAC pin on Bananapi\n");
}

/*
 * Gets the mode of a pin to be input, output
 *********************************************************************************
 */

int pinGetModeSun55iw3 (int pin)
{
    int bank, index, offset, phyaddr, mmap_seek, retval=0;

    bank = pin >> 5;
    index = pin - (bank << 5);
    offset = ((index - ((index >> 3) << 3)) << 2);
    if (isGpioRPin(pin) )
        phyaddr = ((bank -11) * 48) + ((index >> 3) << 2);
    else
        phyaddr = (bank * 48) + ((index >> 3) << 2);
    mmap_seek = phyaddr >> 2;

    if (piModel == PI_MODEL_BANANAPIF5) {
            retval = (*((isGpioRPin(pin) ? sun55iw3_gpior : sun55iw3_gpio) + mmap_seek) >> offset) & 7;
    }
    else
        wiringPiFailure(WPI_FATAL, "pinGetModeSun55iw3: This code should only be called for Bananapi\n");

    return retval;
}

void setInfoSun55iw3(char *hardware, void *vinfo)
{
   rpi_info *info = (rpi_info *)vinfo;

   if (strstr(hardware, "BPI-F5") ||
       strstr(hardware, "BananaPi F5"))
   {
       piModel = PI_MODEL_BANANAPIF5;
       info->type = "BPI-F5";
       info->p1_revision = 3;
       info->ram = "2048M/4096M";
       info->manufacturer = "Bananapi";
       info->processor = "AW SUN55IW3";
   }
   else
       wiringPiFailure(WPI_FATAL, "setInfoSun55iw3: This code should only be called for Bananapi\n");
   
    return;
}

void setMappingPtrsSun55iw3(void)
{
    if (piModel == PI_MODEL_BANANAPIF5)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiF5;
        bcm_to_sun55iw3gpio = &bcmToOGpioBananapiF5;
    }
}
#endif /* end SUN55IW3_SUPPORT */

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
#include "bpi_aml.h"

static volatile uint32_t *aml_gpio, *aml_gpioao;

/* =======================================================================================
 *
 * Amlogic specific
 *
 * ========================================================================================
 */
#ifdef AML_SUPPORT
static int isGpioAOPin(int pin)
{
    int start = 0;
    int end = 0;

    if (piModel == PI_MODEL_BANANAPIM2S) {
        start = M2S_GPIOAO_PIN_START;
        end = M2S_GPIOAO_PIN_END;
    }
    else if (piModel == PI_MODEL_BANANAPIM5) {
        start = M5_GPIOAO_PIN_START;
        end = M5_GPIOAO_PIN_END;
    }
    else if (piModel == PI_MODEL_BANANAPICM4 || piModel == PI_MODEL_BANANAPIRPICM4) {
        start = CM4_GPIOAO_PIN_START;
        end = CM4_GPIOAO_PIN_END;
    }
    else if (piModel == PI_MODEL_BANANAPICM5IO || piModel == PI_MODEL_BANANAPICM5BPICM4IO) {
        /*no GPIOAO*/
        return 0;
    }
	
    if (pin >= start && pin <= end)
        return 1;
    else
        return 0;
}

//
// offset to the GPIO Pin Mux register
//
static int gpioToMuxReg (int pin)
{
    if (piModel == PI_MODEL_BANANAPIM2S) {
        switch (pin) {
	        case    M2S_GPIOH_PIN_START     ...M2S_GPIOH_PIN_END:
	                return  M2S_GPIOH_MUX_B_REG_OFFSET;
	        case    M2S_GPIOA_PIN_START     ...M2S_GPIOA_PIN_START + 7:
	                return  M2S_GPIOA_MUX_D_REG_OFFSET;
	        case    M2S_GPIOA_PIN_START + 8 ...M2S_GPIOA_PIN_END:
	                return  M2S_GPIOA_MUX_E_REG_OFFSET;
	        case    M2S_GPIOX_PIN_START     ...M2S_GPIOX_PIN_START + 7:
	                return  M2S_GPIOX_MUX_3_REG_OFFSET;
	        case    M2S_GPIOX_PIN_START + 8 ...M2S_GPIOX_PIN_START + 15:
	                return  M2S_GPIOX_MUX_4_REG_OFFSET;
	        case    M2S_GPIOX_PIN_START + 16        ...M2S_GPIOX_PIN_END:
	                return  M2S_GPIOX_MUX_5_REG_OFFSET;
	        case    M2S_GPIOAO_PIN_START    ...M2S_GPIOAO_PIN_START + 7:
	                return  M2S_GPIOAO_MUX_REG0_OFFSET;
	        case    M2S_GPIOAO_PIN_START + 8        ...M2S_GPIOAO_PIN_START + 11:
	                return  M2S_GPIOAO_MUX_REG1_OFFSET;
	        default:
	                return -1;
        }
    }
    else if (piModel == PI_MODEL_BANANAPIM5) {
        switch (pin) {
	        case    M5_GPIOH_PIN_START      ...M5_GPIOH_PIN_END:
	                return  M5_GPIOH_MUX_B_REG_OFFSET;
	        case    M5_GPIOA_PIN_START      ...M5_GPIOA_PIN_START + 7:
	                return  M5_GPIOA_MUX_D_REG_OFFSET;
	        case    M5_GPIOA_PIN_START + 8  ...M5_GPIOA_PIN_END:
	                return  M5_GPIOA_MUX_E_REG_OFFSET;
	        case    M5_GPIOX_PIN_START      ...M5_GPIOX_PIN_START + 7:
	                return  M5_GPIOX_MUX_3_REG_OFFSET;
	        case    M5_GPIOX_PIN_START + 8  ...M5_GPIOX_PIN_START + 15:
	                return  M5_GPIOX_MUX_4_REG_OFFSET;
	        case    M5_GPIOX_PIN_START + 16 ...M5_GPIOX_PIN_END:
	                return  M5_GPIOX_MUX_5_REG_OFFSET;
	        case    M5_GPIOAO_PIN_START     ...M5_GPIOAO_PIN_START + 7:
	                return  M5_GPIOAO_MUX_REG0_OFFSET;
	        case    M5_GPIOAO_PIN_START + 8 ...M5_GPIOAO_PIN_START + 11:
	                return  M5_GPIOAO_MUX_REG1_OFFSET;
	        default:
	                return -1;
        }
    }
    else if (piModel == PI_MODEL_BANANAPICM4 || piModel == PI_MODEL_BANANAPIRPICM4) {
        switch (pin) {
	        case    CM4_GPIOH_PIN_START     ...CM4_GPIOH_PIN_END:
	                return  CM4_GPIOH_MUX_B_REG_OFFSET;
	        case    CM4_GPIOA_PIN_START     ...CM4_GPIOA_PIN_START + 7:
	                return  CM4_GPIOA_MUX_D_REG_OFFSET;
	        case    CM4_GPIOA_PIN_START + 8 ...CM4_GPIOA_PIN_END:
	                return  CM4_GPIOA_MUX_E_REG_OFFSET;
	        case    CM4_GPIOX_PIN_START     ...CM4_GPIOX_PIN_START + 7:
	                return  CM4_GPIOX_MUX_3_REG_OFFSET;
	        case    CM4_GPIOX_PIN_START + 8 ...CM4_GPIOX_PIN_START + 15:
	                return  CM4_GPIOX_MUX_4_REG_OFFSET;
	        case    CM4_GPIOX_PIN_START + 16        ...CM4_GPIOX_PIN_END:
	                return  CM4_GPIOX_MUX_5_REG_OFFSET;
	        case    CM4_GPIOAO_PIN_START    ...CM4_GPIOAO_PIN_START + 7:
	                return  CM4_GPIOAO_MUX_REG0_OFFSET;
	        case    CM4_GPIOAO_PIN_START + 8        ...CM4_GPIOAO_PIN_START + 11:
	                return  CM4_GPIOAO_MUX_REG1_OFFSET;
	        default:
	                return -1;
        }
    }
	else if (piModel == PI_MODEL_BANANAPICM5IO || piModel == PI_MODEL_BANANAPICM5BPICM4IO) {
		switch (pin) {
			case CM5_GPIOY_PIN_START  ...CM5_GPIOY_PIN_START + 7:
				return  CM5_GPIOY_MUX_J_REG_OFFSET;
			case CM5_GPIOY_PIN_START + 8  ...CM5_GPIOY_PIN_START + 15:
				return  CM5_GPIOY_MUX_K_REG_OFFSET;
			case CM5_GPIOY_PIN_START + 16  ...CM5_GPIOY_PIN_END:
				return  CM5_GPIOY_MUX_L_REG_OFFSET;
			case CM5_GPIOT_PIN_START  ...CM5_GPIOT_PIN_START + 7:
				return  CM5_GPIOT_MUX_F_REG_OFFSET;
			case CM5_GPIOT_PIN_START + 8  ...CM5_GPIOT_PIN_START + 15:
				return  CM5_GPIOT_MUX_G_REG_OFFSET;
			case CM5_GPIOT_PIN_START + 16  ...CM5_GPIOT_PIN_END:
				return  CM5_GPIOT_MUX_H_REG_OFFSET;
			case CM5_GPIOD_PIN_START  ...CM5_GPIOD_PIN_START + 7:
				return  CM5_GPIOD_MUX_A_REG_OFFSET;
			case CM5_GPIOD_PIN_START + 8  ...CM5_GPIOD_PIN_END:
				return  CM5_GPIOD_MUX_B_REG_OFFSET;
			default:
	        	return -1;
		}
	}
    else
        wiringPiFailure(WPI_FATAL, "gpioToMuxReg: This code should only be called for Bananapi\n");

    return -1;
}

//
// offset to the GPIO Set regsiter
//
static int gpioToGPSETReg (int pin)
{
    if (piModel == PI_MODEL_BANANAPIM2S) {
        if (pin >= M2S_GPIOH_PIN_START && pin <= M2S_GPIOH_PIN_END)
                return  M2S_GPIOH_OUTP_REG_OFFSET;
        if (pin >= M2S_GPIOA_PIN_START && pin <= M2S_GPIOA_PIN_END)
                return  M2S_GPIOA_OUTP_REG_OFFSET;
        if (pin >= M2S_GPIOX_PIN_START && pin <= M2S_GPIOX_PIN_END)
                return  M2S_GPIOX_OUTP_REG_OFFSET;
        if (pin >= M2S_GPIOAO_PIN_START && pin <= M2S_GPIOAO_PIN_END)
                return  M2S_GPIOAO_OUTP_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPIM5) {
        if (pin >= M5_GPIOH_PIN_START && pin <= M5_GPIOH_PIN_END)
                return  M5_GPIOH_OUTP_REG_OFFSET;
        if (pin >= M5_GPIOA_PIN_START && pin <= M5_GPIOA_PIN_END)
                return  M5_GPIOA_OUTP_REG_OFFSET;
        if (pin >= M5_GPIOX_PIN_START && pin <= M5_GPIOX_PIN_END)
                return  M5_GPIOX_OUTP_REG_OFFSET;
        if (pin >= M5_GPIOAO_PIN_START && pin <= M5_GPIOAO_PIN_END)
                return  M5_GPIOAO_OUTP_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPICM4 || piModel == PI_MODEL_BANANAPIRPICM4) {
        if (pin >= CM4_GPIOH_PIN_START && pin <= CM4_GPIOH_PIN_END)
                return  CM4_GPIOH_OUTP_REG_OFFSET;
        if (pin >= CM4_GPIOA_PIN_START && pin <= CM4_GPIOA_PIN_END)
                return  CM4_GPIOA_OUTP_REG_OFFSET;
        if (pin >= CM4_GPIOX_PIN_START && pin <= CM4_GPIOX_PIN_END)
                return  CM4_GPIOX_OUTP_REG_OFFSET;
        if (pin >= CM4_GPIOAO_PIN_START && pin <= CM4_GPIOAO_PIN_END)
                return  CM4_GPIOAO_OUTP_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPICM5IO || piModel == PI_MODEL_BANANAPICM5BPICM4IO) {
        if(pin >= CM5_GPIOY_PIN_START && pin <= CM5_GPIOY_PIN_END)
                return CM5_GPIOY_OUTP_REG_OFFSET;
        if(pin >= CM5_GPIOT_PIN_START && pin <= CM5_GPIOT_PIN_END)
                return CM5_GPIOT_OUTP_REG_OFFSET;
        if(pin >= CM5_GPIOD_PIN_START && pin <= CM5_GPIOD_PIN_END)
                return CM5_GPIOD_OUTP_REG_OFFSET;
    }
    else
        wiringPiFailure(WPI_FATAL, "gpioToGPSETReg: This code should only be called for Bananapi\n");

    return  -1;
}

//
// offset to the GPIO Input regsiter
//
static int  gpioToGPLEVReg (int pin)
{
    if (piModel == PI_MODEL_BANANAPIM2S) {
        if (pin >= M2S_GPIOH_PIN_START && pin <= M2S_GPIOH_PIN_END)
                return  M2S_GPIOH_INP_REG_OFFSET;
        if (pin >= M2S_GPIOA_PIN_START && pin <= M2S_GPIOA_PIN_END)
                return  M2S_GPIOA_INP_REG_OFFSET;
        if (pin >= M2S_GPIOX_PIN_START && pin <= M2S_GPIOX_PIN_END)
                return  M2S_GPIOX_INP_REG_OFFSET;
        if (pin >= M2S_GPIOAO_PIN_START && pin <= M2S_GPIOAO_PIN_END)
                return  M2S_GPIOAO_INP_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPIM5) {
        if (pin >= M5_GPIOH_PIN_START && pin <= M5_GPIOH_PIN_END)
                return  M5_GPIOH_INP_REG_OFFSET;
        if (pin >= M5_GPIOA_PIN_START && pin <= M5_GPIOA_PIN_END)
                return  M5_GPIOA_INP_REG_OFFSET;
        if (pin >= M5_GPIOX_PIN_START && pin <= M5_GPIOX_PIN_END)
                return  M5_GPIOX_INP_REG_OFFSET;
        if (pin >= M5_GPIOAO_PIN_START && pin <= M5_GPIOAO_PIN_END)
                return  M5_GPIOAO_INP_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPICM4 || piModel == PI_MODEL_BANANAPIRPICM4) {
        if (pin >= CM4_GPIOH_PIN_START && pin <= CM4_GPIOH_PIN_END)
                return  CM4_GPIOH_INP_REG_OFFSET;
        if (pin >= CM4_GPIOA_PIN_START && pin <= CM4_GPIOA_PIN_END)
                return  CM4_GPIOA_INP_REG_OFFSET;
        if (pin >= CM4_GPIOX_PIN_START && pin <= CM4_GPIOX_PIN_END)
                return  CM4_GPIOX_INP_REG_OFFSET;
        if (pin >= CM4_GPIOAO_PIN_START && pin <= CM4_GPIOAO_PIN_END)
                return  CM4_GPIOAO_INP_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPICM5IO || piModel == PI_MODEL_BANANAPICM5BPICM4IO) {
        if(pin >= CM5_GPIOY_PIN_START && pin <= CM5_GPIOY_PIN_END)
                return CM5_GPIOY_INP_REG_OFFSET;
        if(pin >= CM5_GPIOT_PIN_START && pin <= CM5_GPIOT_PIN_END)
                return CM5_GPIOT_INP_REG_OFFSET;
        if(pin >= CM5_GPIOD_PIN_START && pin <= CM5_GPIOD_PIN_END)
                return CM5_GPIOD_INP_REG_OFFSET;
    }
    else
        wiringPiFailure(WPI_FATAL, "gpioToGPLEVReg: This code should only be called for Bananapi\n");

    return  -1;
}

//
// offset to the GPIO Pull up/down enable regsiter
//
static int  gpioToPUENReg (int pin)
{
    if (piModel == PI_MODEL_BANANAPIM2S) {
        if (pin >= M2S_GPIOH_PIN_START && pin <= M2S_GPIOH_PIN_END)
                return  M2S_GPIOH_PUEN_REG_OFFSET;
        if (pin >= M2S_GPIOA_PIN_START && pin <= M2S_GPIOA_PIN_END)
                return  M2S_GPIOA_PUEN_REG_OFFSET;
        if (pin >= M2S_GPIOX_PIN_START && pin <= M2S_GPIOX_PIN_END)
                return  M2S_GPIOX_PUEN_REG_OFFSET;
        if (pin >= M2S_GPIOAO_PIN_START && pin <= M2S_GPIOAO_PIN_END)
                return  M2S_GPIOAO_PUEN_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPIM5) {
        if (pin >= M5_GPIOH_PIN_START && pin <= M5_GPIOH_PIN_END)
                return  M5_GPIOH_PUEN_REG_OFFSET;
        if (pin >= M5_GPIOA_PIN_START && pin <= M5_GPIOA_PIN_END)
                return  M5_GPIOA_PUEN_REG_OFFSET;
        if (pin >= M5_GPIOX_PIN_START && pin <= M5_GPIOX_PIN_END)
                return  M5_GPIOX_PUEN_REG_OFFSET;
        if (pin >= M5_GPIOAO_PIN_START && pin <= M5_GPIOAO_PIN_END)
                return  M5_GPIOAO_PUEN_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPICM4 || piModel == PI_MODEL_BANANAPIRPICM4) {
        if (pin >= CM4_GPIOH_PIN_START && pin <= CM4_GPIOH_PIN_END)
                return  CM4_GPIOH_PUEN_REG_OFFSET;
        if (pin >= CM4_GPIOA_PIN_START && pin <= CM4_GPIOA_PIN_END)
                return  CM4_GPIOA_PUEN_REG_OFFSET;
        if (pin >= CM4_GPIOX_PIN_START && pin <= CM4_GPIOX_PIN_END)
                return  CM4_GPIOX_PUEN_REG_OFFSET;
        if (pin >= CM4_GPIOAO_PIN_START && pin <= CM4_GPIOAO_PIN_END)
                return  CM4_GPIOAO_PUEN_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPICM5IO || piModel == PI_MODEL_BANANAPICM5BPICM4IO) {
        if(pin >= CM5_GPIOY_PIN_START && pin <= CM5_GPIOY_PIN_END)
                return CM5_GPIOY_PUEN_REG_OFFSET;
        if(pin >= CM5_GPIOT_PIN_START && pin <= CM5_GPIOT_PIN_END)
                return CM5_GPIOT_PUEN_REG_OFFSET;
        if(pin >= CM5_GPIOD_PIN_START && pin <= CM5_GPIOD_PIN_END)
                return CM5_GPIOD_PUEN_REG_OFFSET;
    }
    else
        wiringPiFailure(WPI_FATAL, "gpioToPUENReg: This code should only be called for Bananapi\n");

    return  -1;
}

//
// offset to the GPIO Pull up/down regsiter
//
static int  gpioToPUPDReg (int pin)
{
    if (piModel == PI_MODEL_BANANAPIM2S) {
        if (pin >= M2S_GPIOH_PIN_START && pin <= M2S_GPIOH_PIN_END)
                return  M2S_GPIOH_PUPD_REG_OFFSET;
        if (pin >= M2S_GPIOA_PIN_START && pin <= M2S_GPIOA_PIN_END)
                return  M2S_GPIOA_PUPD_REG_OFFSET;
        if (pin >= M2S_GPIOX_PIN_START && pin <= M2S_GPIOX_PIN_END)
                return  M2S_GPIOX_PUPD_REG_OFFSET;
        if (pin >= M2S_GPIOAO_PIN_START && pin <= M2S_GPIOAO_PIN_END)
                return  M2S_GPIOAO_PUPD_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPIM5) {
        if (pin >= M5_GPIOH_PIN_START && pin <= M5_GPIOH_PIN_END)
                return  M5_GPIOH_PUPD_REG_OFFSET;
        if (pin >= M5_GPIOA_PIN_START && pin <= M5_GPIOA_PIN_END)
                return  M5_GPIOA_PUPD_REG_OFFSET;
        if (pin >= M5_GPIOX_PIN_START && pin <= M5_GPIOX_PIN_END)
                return  M5_GPIOX_PUPD_REG_OFFSET;
        if (pin >= M5_GPIOAO_PIN_START && pin <= M5_GPIOAO_PIN_END)
                return  M5_GPIOAO_PUPD_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPICM4 || piModel == PI_MODEL_BANANAPIRPICM4) {
        if (pin >= CM4_GPIOH_PIN_START && pin <= CM4_GPIOH_PIN_END)
                return  CM4_GPIOH_PUPD_REG_OFFSET;
        if (pin >= CM4_GPIOA_PIN_START && pin <= CM4_GPIOA_PIN_END)
                return  CM4_GPIOA_PUPD_REG_OFFSET;
        if (pin >= CM4_GPIOX_PIN_START && pin <= CM4_GPIOX_PIN_END)
                return  CM4_GPIOX_PUPD_REG_OFFSET;
        if (pin >= CM4_GPIOAO_PIN_START && pin <= CM4_GPIOAO_PIN_END)
                return  CM4_GPIOAO_PUPD_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPICM5IO || piModel == PI_MODEL_BANANAPICM5BPICM4IO) {
        if(pin >= CM5_GPIOT_PIN_START && pin <= CM5_GPIOT_PIN_END)
                return CM5_GPIOT_PUPD_REG_OFFSET;
        if(pin >= CM5_GPIOY_PIN_START && pin <= CM5_GPIOY_PIN_END)
                return CM5_GPIOY_PUPD_REG_OFFSET;
        if(pin >= CM5_GPIOD_PIN_START && pin <= CM5_GPIOD_PIN_END)
                return CM5_GPIOD_PUPD_REG_OFFSET;
    }
    else
        wiringPiFailure(WPI_FATAL, "gpioToPUPDReg: This code should only be called for Bananapi\n");

    return  -1;
}

//
// offset to the GPIO bit
//
static int  gpioToShiftReg (int pin)
{
    if (piModel == PI_MODEL_BANANAPIM2S) {
        if (pin >= M2S_GPIOH_PIN_START && pin <= M2S_GPIOH_PIN_END)
                return  pin - M2S_GPIOH_PIN_START;
        if (pin >= M2S_GPIOA_PIN_START && pin <= M2S_GPIOA_PIN_END)
                return  pin - M2S_GPIOA_PIN_START;
        if (pin >= M2S_GPIOX_PIN_START && pin <= M2S_GPIOX_PIN_END)
                return  pin - M2S_GPIOX_PIN_START;
        if (pin >= M2S_GPIOAO_PIN_START && pin <= M2S_GPIOAO_PIN_END)
                return  pin - M2S_GPIOAO_PIN_START;
    }
    else if (piModel == PI_MODEL_BANANAPIM5) {
        if (pin >= M5_GPIOH_PIN_START && pin <= M5_GPIOH_PIN_END)
                return  pin - M5_GPIOH_PIN_START;
        if (pin >= M5_GPIOA_PIN_START && pin <= M5_GPIOA_PIN_END)
                return  pin - M5_GPIOA_PIN_START;
        if (pin >= M5_GPIOX_PIN_START && pin <= M5_GPIOX_PIN_END)
                return  pin - M5_GPIOX_PIN_START;
        if (pin >= M5_GPIOAO_PIN_START && pin <= M5_GPIOAO_PIN_END)
                return  pin - M5_GPIOAO_PIN_START;
    }
    else if (piModel == PI_MODEL_BANANAPICM4 || piModel == PI_MODEL_BANANAPIRPICM4) {
        if (pin >= CM4_GPIOH_PIN_START && pin <= CM4_GPIOH_PIN_END)
                return  pin - CM4_GPIOH_PIN_START;
        if (pin >= CM4_GPIOA_PIN_START && pin <= CM4_GPIOA_PIN_END)
                return  pin - CM4_GPIOA_PIN_START;
        if (pin >= CM4_GPIOX_PIN_START && pin <= CM4_GPIOX_PIN_END)
                return  pin - CM4_GPIOX_PIN_START;
        if (pin >= CM4_GPIOAO_PIN_START && pin <= CM4_GPIOAO_PIN_END)
                return  pin - CM4_GPIOAO_PIN_START;
    }
    else if (piModel == PI_MODEL_BANANAPICM5IO || piModel == PI_MODEL_BANANAPICM5BPICM4IO) {
        if(pin >= CM5_GPIOY_PIN_START && pin <= CM5_GPIOY_PIN_END)
                return pin - CM5_GPIOY_PIN_START;
        if(pin >= CM5_GPIOT_PIN_START && pin <= CM5_GPIOT_PIN_END)
                return pin - CM5_GPIOT_PIN_START;
        if(pin >= CM5_GPIOD_PIN_START && pin <= CM5_GPIOD_PIN_END)
                return pin - CM5_GPIOD_PIN_START;
    }
    else
        wiringPiFailure(WPI_FATAL, "gpioToShiftReg: This code should only be called for Bananapi\n");

    return  -1;
}

//
// offset to the GPIO Function register
//
static int  gpioToGPFSELReg (int pin)
{
    if (piModel == PI_MODEL_BANANAPIM2S) {
        if (pin >= M2S_GPIOH_PIN_START && pin <= M2S_GPIOH_PIN_END)
                return  M2S_GPIOH_FSEL_REG_OFFSET;
        if (pin >= M2S_GPIOA_PIN_START && pin <= M2S_GPIOA_PIN_END)
                return  M2S_GPIOA_FSEL_REG_OFFSET;
        if (pin >= M2S_GPIOX_PIN_START && pin <= M2S_GPIOX_PIN_END)
                return  M2S_GPIOX_FSEL_REG_OFFSET;
        if (pin >= M2S_GPIOAO_PIN_START && pin <= M2S_GPIOAO_PIN_END)
                return  M2S_GPIOAO_FSEL_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPIM5) {
        if (pin >= M5_GPIOH_PIN_START && pin <= M5_GPIOH_PIN_END)
                return  M5_GPIOH_FSEL_REG_OFFSET;
        if (pin >= M5_GPIOA_PIN_START && pin <= M5_GPIOA_PIN_END)
                return  M5_GPIOA_FSEL_REG_OFFSET;
        if (pin >= M5_GPIOX_PIN_START && pin <= M5_GPIOX_PIN_END)
                return  M5_GPIOX_FSEL_REG_OFFSET;
        if (pin >= M5_GPIOAO_PIN_START && pin <= M5_GPIOAO_PIN_END)
                return  M5_GPIOAO_FSEL_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPICM4 || piModel == PI_MODEL_BANANAPIRPICM4) {
        if (pin >= CM4_GPIOH_PIN_START && pin <= CM4_GPIOH_PIN_END)
                return  CM4_GPIOH_FSEL_REG_OFFSET;
        if (pin >= CM4_GPIOA_PIN_START && pin <= CM4_GPIOA_PIN_END)
                return  CM4_GPIOA_FSEL_REG_OFFSET;
        if (pin >= CM4_GPIOX_PIN_START && pin <= CM4_GPIOX_PIN_END)
                return  CM4_GPIOX_FSEL_REG_OFFSET;
        if (pin >= CM4_GPIOAO_PIN_START && pin <= CM4_GPIOAO_PIN_END)
                return  CM4_GPIOAO_FSEL_REG_OFFSET;
    }
    else if (piModel == PI_MODEL_BANANAPICM5IO || piModel == PI_MODEL_BANANAPICM5BPICM4IO) {
        if(pin >= CM5_GPIOD_PIN_START && pin <= CM5_GPIOD_PIN_END)
                return CM5_GPIOD_FSEL_REG_OFFSET;
        if(pin >= CM5_GPIOT_PIN_START && pin <= CM5_GPIOT_PIN_END)
                return CM5_GPIOT_FSEL_REG_OFFSET;
        if(pin >= CM5_GPIOY_PIN_START && pin <= CM5_GPIOY_PIN_END)
                return CM5_GPIOY_FSEL_REG_OFFSET;
    }
    else
        wiringPiFailure(WPI_FATAL, "gpioToGPFSELReg: This code should only be called for Bananapi\n");

    return  -1;
}

//***************************************************************************************************

/*
* Note: Unlike the above code, this is not copied directly from wiringPi
* Much of the code is identical, but un-necessary parts are deleted
*/

int wiringPiSetupAml (void)
{
    int fd;

    // Open the master /dev/memory device
    if (access("/dev/gpiomem", 0) == 0)
    {
        if ((fd = open("/dev/gpiomem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupAml: Unable to open /dev/gpiomem: %s\n", strerror(errno));
    }
    else
    {
        if (geteuid() != 0)
            (void)wiringPiFailure(WPI_FATAL, "wiringPiSetupAml: Must be root. (Did you forget sudo?)\n");

        if ((fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupAml: Unable to open /dev/mem: %s\n", strerror(errno));
    }

    //  piBoardId (&model, &rev, &mem, &maker, &overVolted) ;

    //wiringPi pin numbers are unused in rPI.GPIO
//    pinToGpio = NULL;
//    pin_array_count = 0;
    //physToGPIO replaced by pin_to_gpio in rPI.GPIO
//    physToGpio = NULL;

    if (piModel == PI_MODEL_BANANAPIM2S)
    {
        aml_gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, M2S_GPIO_BASE);
        if (aml_gpio == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupAml: mmap (GPIO) failed: %s\n", strerror(errno));
        aml_gpioao = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, M2S_GPIO_AO_BASE);
        if (aml_gpioao == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupAml: mmap (GPIO_AO) failed: %s\n", strerror(errno));
    }
    else if (piModel == PI_MODEL_BANANAPIM5) {
        aml_gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, M5_GPIO_BASE);
        if (aml_gpio == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupAml: mmap (GPIO) failed: %s\n", strerror(errno));
        aml_gpioao = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, M5_GPIO_AO_BASE);
        if (aml_gpioao == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupAml: mmap (GPIO_AO) failed: %s\n", strerror(errno));
    }
    else if (piModel == PI_MODEL_BANANAPICM4 || piModel == PI_MODEL_BANANAPIRPICM4)
    {
        aml_gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, CM4_GPIO_BASE);
        if (aml_gpio == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupAml: mmap (GPIO) failed: %s\n", strerror(errno));
        aml_gpioao = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, CM4_GPIO_AO_BASE);
        if (aml_gpioao == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupAml: mmap (GPIO_AO) failed: %s\n", strerror(errno));
    }
    else if (piModel == PI_MODEL_BANANAPICM5IO || piModel == PI_MODEL_BANANAPICM5BPICM4IO) {
        aml_gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, CM5_GPIO_BASE);
        if (aml_gpio == MAP_FAILED)
            return wiringPiFailure(WPI_ALMOST, "wiringPiSetupAml: mmap (GPIO) failed: %s\n", strerror(errno));
    }

    return 0;
}

void wiringPiCleanupAml (void)
{
    munmap((void *)aml_gpio, BLOCK_SIZE);
    if (aml_gpioao != NULL) munmap((void *)aml_gpioao, BLOCK_SIZE);
}

/*
 * pinModeAml:
 *	Sets the mode of a pin to be input, output or PWM output
 *********************************************************************************
 */

void pinModeAml (int pin, int mode)
{
    int fsel, mux, target, shift;

    //Aml: For our purposes pin comes in as gpio, original code converted
    //pin to gpio and kept origPin as pin#
	
    fsel = gpioToGPFSELReg(pin);
    mux = gpioToMuxReg(pin);
    shift = gpioToShiftReg (pin);
    target = shift * 4;

    if (mode == INPUT)
    {
        if (piModel == PI_MODEL_BANANAPIM2S ||
            piModel == PI_MODEL_BANANAPIM5 ||
            piModel == PI_MODEL_BANANAPICM4 ||
            piModel == PI_MODEL_BANANAPIRPICM4 ||
            piModel == PI_MODEL_BANANAPICM5IO ||
            piModel == PI_MODEL_BANANAPICM5BPICM4IO) {
            *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + mux)  = *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + mux) & ~(0xF << target);
            *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + fsel) = *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + fsel) | (1 << shift);
	}
	else
            wiringPiFailure(WPI_FATAL, "pinModeAml: This code should only be called for Bananapi\n");
    }
    else if (mode == OUTPUT)
    {
        if (piModel == PI_MODEL_BANANAPIM2S ||
            piModel == PI_MODEL_BANANAPIM5 ||
            piModel == PI_MODEL_BANANAPICM4 ||
            piModel == PI_MODEL_BANANAPIRPICM4 ||
            piModel == PI_MODEL_BANANAPICM5IO ||
	    piModel == PI_MODEL_BANANAPICM5BPICM4IO) {
            *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + mux)  = *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + mux) & ~(0xF << target);
            *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + fsel) = *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + fsel) & ~(1 << shift);
	}
	else
            wiringPiFailure(WPI_FATAL, "pinModeAml: This code should only be called for Bananapi\n");
    }
}


/*
 * pullUpDnControlAml:
 *	Control the internal pull-up/down resistors on a GPIO pin
 *	The Arduino only has pull-ups and these are enabled by writing 1
 *	to a port when in input mode - this paradigm doesn't quite apply
 *	here though.
 *********************************************************************************
 */

void pullUpDnControlAml (int pin, int pud)
{
    int puen, pupd, shift;

    shift = gpioToShiftReg(pin);
    puen = gpioToPUENReg(pin);
    pupd = gpioToPUPDReg(pin);

    if (piModel == PI_MODEL_BANANAPIM2S ||
        piModel == PI_MODEL_BANANAPIM5 ||
        piModel == PI_MODEL_BANANAPICM4 ||
        piModel == PI_MODEL_BANANAPIRPICM4 ||
        piModel == PI_MODEL_BANANAPICM5IO ||
	piModel == PI_MODEL_BANANAPICM5BPICM4IO)
    {

        if (pud)
        {
            // Enable Pull/Pull-down resister
            *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + puen) = (*((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + puen) | (1 << shift));

            if (pud == PUD_UP)
                *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + pupd) = (*((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + pupd) | (1 << shift));
            else
                *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + pupd) = (*((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + pupd) & ~(1 << shift));
        }
        else // Disable Pull/Pull-down resister
            *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + puen) = (*((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + puen) & ~(1 << shift));
    }
    else
        wiringPiFailure(WPI_FATAL, "pullUpDnControlAml: This code should only be called for Bananapi\n");
}

/*
 * digitalReadAml:
 *	Read the value of a given Pin, returning HIGH or LOW
 *********************************************************************************
 */

int digitalReadAml (int pin)
{

    if (piModel == PI_MODEL_BANANAPIM2S ||
        piModel == PI_MODEL_BANANAPIM5 ||
        piModel == PI_MODEL_BANANAPICM4 ||
        piModel == PI_MODEL_BANANAPIRPICM4 ||
        piModel == PI_MODEL_BANANAPICM5IO ||
        piModel == PI_MODEL_BANANAPICM5BPICM4IO)
    {
        if ((*((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + gpioToGPLEVReg(pin)) & (1 << gpioToShiftReg(pin))) != 0)
            return HIGH;
        else
            return LOW;
    }
    else
        wiringPiFailure(WPI_FATAL, "digitalReadAml: This code should only be called for Bananapi\n");

    return 0;
}

/*
 * digitalWriteAml:
 *	Set an output bit
 *********************************************************************************
 */

void digitalWriteAml (int pin, int value)
{

    if (piModel == PI_MODEL_BANANAPIM2S ||
        piModel == PI_MODEL_BANANAPIM5 ||
        piModel == PI_MODEL_BANANAPICM4 ||
        piModel == PI_MODEL_BANANAPIRPICM4 ||
        piModel == PI_MODEL_BANANAPICM5IO ||
	piModel == PI_MODEL_BANANAPICM5BPICM4IO)
    {
        if (value == LOW)
            *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + gpioToGPSETReg(pin)) &= ~(1 << gpioToShiftReg(pin));
        else
            *((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + gpioToGPSETReg(pin)) |= (1 << gpioToShiftReg(pin));
    }
    else
        wiringPiFailure(WPI_FATAL, "digitalWriteAml: This code should only be called for Bananapi\n");
}

/*
 * analogReadAml:
 *	Read the analog value of a given Pin.
 *	There is no on-board Pi analog hardware,
 *	so this needs to go to a new node.
 *********************************************************************************
 */

int analogReadAml (int pin)
{
    wiringPiFailure(WPI_FATAL, "analogReadAml: No ADC pin on Bananapi\n");
    return 0;
}

/*
 * analogWriteAml:
 *	Write the analog value to the given Pin.
 *	There is no on-board Pi analog hardware,
 *	so this needs to go to a new node.
 *********************************************************************************
 */

void analogWriteAml (int pin, int value)
{
    wiringPiFailure(WPI_FATAL, "analogWriteAml: No DAC pin on Bananapi\n");
}

/*
 * pinGetModeAml:
 *	Gets the mode of a pin to be input, output
 * Added by JF
 *********************************************************************************
 */

int pinGetModeAml (int pin)
{
    int rwbit, regval, retval=0;
    //aml: pin comes in as gpio

    if (piModel == PI_MODEL_BANANAPIM2S ||
        piModel == PI_MODEL_BANANAPIM5 ||
        piModel == PI_MODEL_BANANAPICM4 ||
        piModel == PI_MODEL_BANANAPIRPICM4 ||
        piModel == PI_MODEL_BANANAPICM5IO ||
        piModel == PI_MODEL_BANANAPICM5BPICM4IO)
    {
        regval = (*((isGpioAOPin(pin) ? aml_gpioao : aml_gpio) + gpioToGPFSELReg(pin)));
        rwbit = regval & (1 << gpioToShiftReg(pin));
        retval = ((rwbit!=0) ? 0 : 1);
    }
    else
        wiringPiFailure(WPI_FATAL, "pinGetModeAml: This code should only be called for Bananapi\n");

    return retval;
}

void setInfoAml(char *hardware, void *vinfo)
{
   rpi_info *info = (rpi_info *)vinfo;

   if (strstr(hardware, "BPI-M2S") ||
       strstr(hardware, "BananaPi M2S"))
   {
        piModel = PI_MODEL_BANANAPIM2S;
        info->type = "BPI-M2S";
        info->p1_revision = 3;
        info->ram = "2048M/4096M";
        info->manufacturer = "Bananapi";
        info->processor = "AMLS922X/AMLA311D";
   }
   else if (strstr(hardware, "BPI-M5") ||
        strstr(hardware, "BPI-M2-Pro") ||
        strstr(hardware, "Banana Pi BPI-M5") ||
        strstr(hardware, "Banana Pi BPI-M2-PRO"))
   {
        piModel = PI_MODEL_BANANAPIM5;
        info->type = "BPI-M5";
        info->p1_revision = 3;
        info->ram = "2048M/4096M";
        info->manufacturer = "Bananapi";
        info->processor = "AMLS905X3";
   }
   else if (strstr(hardware, "BPI-CM4") ||
        strstr(hardware, "BananaPi BPI-CM4IO"))
   {
        piModel = PI_MODEL_BANANAPICM4;
        info->type = "BPI-CM4";
        info->p1_revision = 3;
        info->ram = "2048M/4096M";
        info->manufacturer = "Bananapi";
        info->processor = "AMLA311D";
   }
   else if (strstr(hardware, "BPI-RPICM4") ||
        strstr(hardware, "BananaPi RPI-CM4IO"))
   {
        piModel = PI_MODEL_BANANAPIRPICM4;
        info->type = "BPI-RPICM4";
        info->p1_revision = 3;
        info->ram = "2048M/4096M";
        info->manufacturer = "Bananapi";
        info->processor = "AMLA311D";
   }
   else if (strstr(hardware, "BPI-CM5IO"))
   {
        piModel = PI_MODEL_BANANAPICM5IO;
        info->type = "BPI-CM5IO";
        info->p1_revision = 3;
        info->ram = "2048M/4096M";
        info->manufacturer = "Bananapi";
        info->processor = "AMLA311D2";
   }
   else if(strstr(hardware, "BPI-CM5-BPICM4IO"))
   {
        piModel = PI_MODEL_BANANAPICM5BPICM4IO;
        info->type = "BPI-CM5BPICM4IO";
        info->p1_revision = 3;
        info->ram = "2048M/4096M";
        info->manufacturer = "Bananapi";
        info->processor = "AMLA311D2";
   } 
   else
        wiringPiFailure(WPI_FATAL, "setInfoAml: This code should only be called for Bananapi\n");
   
   return;
}

void setMappingPtrsAml(void)
{
    if (piModel == PI_MODEL_BANANAPIM2S)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiM2S;
        bcm_to_amlgpio = &bcmToOGpioBananapiM2S;
    }
    else if (piModel == PI_MODEL_BANANAPIM5)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiM5;
        bcm_to_amlgpio = &bcmToOGpioBananapiM5;
    }
    else if (piModel == PI_MODEL_BANANAPICM4)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiCM4;
        bcm_to_amlgpio = &bcmToOGpioBananapiCM4;
    }
    else if (piModel == PI_MODEL_BANANAPIRPICM4)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiRPICM4;
        bcm_to_amlgpio = &bcmToOGpioBananapiRPICM4;
    }
    else if (piModel == PI_MODEL_BANANAPICM5IO)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiCM5IO;
        bcm_to_amlgpio = &bcmToOGpioBananapiCM5IO;
    }
    else if (piModel == PI_MODEL_BANANAPICM5BPICM4IO)
    {
        pin_to_gpio = (const int(*)[41]) & physToGpioBananapiCM5BPICM4IO;
        bcm_to_amlgpio = &bcmToOGpioBananapiCM5BPICM4IO;
    }
}
#endif /* end AML_SUPPORT */

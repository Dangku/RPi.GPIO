/*
 * Bananapi specific code
 */

#ifndef BPI_SPACEMIT_H_INCLUDED
#define BPI_SPACEMIT_H_INCLUDED

#ifdef DEFINE_BANANAPI_VARS

/* =======================================================================================
 *
 * Spacemit specific
 *
 * ========================================================================================
 */
#ifdef SPACEMIT_SUPPORT

#define SPACEMIT_GPIO_MASK		(0xFFFFFF00)
#define SPACEMIT_GPIO_BASE		0xd4019000
#define SPACEMIT_PINCTRL_BASE		0xd401e000

#define F3_BANK012_OFFSET(x)    ((x) << 2)
#define F3_BANK3_OFFSET         0x100

#define F3_GPLR                 0x0
#define F3_GPDR                 0xc
#define F3_GPSR                 0x18
#define F3_GPCR                 0x24
#define F3_GRER                 0x30
#define F3_GFER                 0x3c
#define F3_GEDR                 0x48
#define F3_GSDR                 0x54
#define F3_GCDR                 0x60
#define F3_GSRER                0x6c
#define F3_GCRER                0x78
#define F3_GSFER                0x84
#define F3_GCFER                0x90
#define F3_GAPMASK              0x9c
#define F3_GCPMASK              0xa8

#define F3_MFPR(x) \
    (((x) <= 85) ? (((x) + 1) << 2) : \
    (((x) <= 92) ? (((x) + 1) << 2) + 0x90 : \
    (((x) <= 127) ? (((x) + 1) << 2) + 0x4c : 0)))

#define F3_AF_SEL_OFFSET        (0)
#define F3_AF_SEL_MASK          (7 << 0)
#define F3_GPIO_ALT(x) \
    (((x) >= 70 && (x) <= 73) || ((x) >= 93 && (x) <= 103)) ? 1 : \
    (((x) >= 104 && (x) <= 109)) ? 4 : \
    0

/* pull up/down */
#define F3_PULL_DIS        (0)     /* bit[15:13] 000 */
#define F3_PULL_UP         (6)     /* bit[15:13] 110 */
#define F3_PULL_DOWN       (5)     /* bit[15:13] 101 */
#define F3_PULL_OFFSET     (13)
#define F3_PULL_MASK       (7 << 13)

/* Non-static add extern definition below */
int spacemit_found;
const int (*bcm_to_spacemitgpio)[64];

//
// pinToGpio:
//	Take a Wiring pin (0 through X) and re-map it to the AML_GPIO pin
//
static const int pinToGpioBananapiF3[64] = {
    // wiringPi number to native gpio number
    71, 74, //  0 |  1 : GPIO.71, GPIO.74(PWM_9)
    72, 73, //  2 |  3 : GPIO.72, GPIO.73
    91, 92, //  4 |  5 : GPIO.91, GPIO.92
    49, 70, //  6 |  7 : GPIO.49, GPIO.70
    52, 51, //  8 |  9 : GPIO.52(I2C-4_SDA), GPIO.51(I2C-4_SCL)
    76, 50, // 10 | 11 : GPIO.76(SPI_SS), GPIO.50
    77, 78, // 12 | 13 : GPIO.77(SPI_MOSI), GPIO.78(SPI_MISO)
    75, 47, // 14 | 15 : GPIO.75(SPI_CLK), GPIO.47(UART_0_TX)
    48, -1, // 16 | 17 : GPIO.48(UART_0_RX),
    // Padding:
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,         // 18...31
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 32...47
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, // 48...63
};

const int bcmToOGpioBananapiF3[64] = { // BCM ModE
     -1,  -1,  52,  51,  70,  -1,  -1,  50, // 0..7
     76,  78,  77,  75,  -1,  -1,  47,  48, // 8..15
     -1,  71,  74,  -1,  -1,  -1,  73,  91, // 16..23
     92,  49,  -1,  72,  -1,  -1,  -1,  -1, // 24..31
// Padding:
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 32..39
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 40..47
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 48..55
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1  // 56..63
};

static const int physToGpioBananapiF3[64] = {
    // physical header pin number to native gpio number
    -1,             //  0
    -1, -1, //  1 |  2 : 3.3V, 5.0V
    52, -1, //  3 |  4 : GPIO.52(I2C-4_SDA), 5.0V
    51, -1, //  5 |  6 : GPIO.51(I2C-4_SCL), GND
    70, 47, //  7 |  8 : GPIO.70, GPIO.47(UART_0_TX)
    -1, 48, //  9 | 10 : GND, GPIO.48(UART_0_RX)
    71, 74, // 11 | 12 : GPIO.71, GPIO.74(PWM_9)
    72, -1, // 13 | 14 : GPIO.72, GND
    73, 91, // 15 | 16 : GPIO.73, GPIO.91
    -1, 92, // 17 | 18 : 3.3V, GPIO.92
    77, -1, // 19 | 20 : GPIO.77(SPI_MOSI), GND
    78, 49, // 21 | 22 : GPIO.78(SPI_MISO), GPIO.49
    75, 76, // 23 | 24 : GPIO.75(SPI_CLK), GPIO.76(SPI_SS)
    -1, 50, // 25 | 26 : GND, GPIO.50
    // Not used
    -1, -1, -1, -1, -1, -1, -1, -1, // 27...34
    -1, -1, -1, -1, -1, -1,         // 35...40
    -1, -1, -1, -1, -1, -1, -1, -1, // 41...48
    -1, -1, -1, -1, -1, -1, -1, -1, // 49...56
    -1, -1, -1, -1, -1, -1, -1      // 57...63
};
#endif /* SPACEMIT_SUPPORT */

#else /*  DEFINE_BANANAPI_VARS */

/* =======================================================================================
 *
 * Spacemit specific
 *
 * ========================================================================================
 */
#ifdef SPACEMIT_SUPPORT
extern int spacemit_found;
extern const int (*bcm_to_spacemitgpio)[64];

extern const int physToGpioBananapiF3[64];
extern const int bcmToOGpioBananapiF3[64];

int wiringPiSetupSpacemit (void);
void wiringPiCleanupSpacemit (void);
void pinModeSpacemit (int pin, int mode);
void pullUpDnControlSpacemit (int pin, int pud);
int digitalReadSpacemit (int pin);
void digitalWriteSpacemit (int pin, int value);
int analogReadSpacemit (int pin);
void analogWriteSpacemit (int pin, int value);
int pinGetModeSpacemit (int pin);
void setInfoSpacemit(char *hardware, void *vinfo);
void setMappingPtrsSpacemit(void);
#endif /* SPACEMIT_SUPPORT */

#endif /* DEFINE_BANANAPI_VARS */

#endif /* BPI_SPACEMIT_H_INCLUDED */

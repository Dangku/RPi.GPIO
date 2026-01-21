/*
 * Bananapi specific code
 */

#ifndef BPI_RENESAS_H_INCLUDED
#define BPI_RENESAS_H_INCLUDED

#ifdef DEFINE_BANANAPI_VARS

/* =======================================================================================
 *
 * Rzv2hn specific
 *
 * ========================================================================================
 */
#ifdef RZV2HN_SUPPORT

#define RZV2HN_GPIO_BASE			  0x10410000
#define RZV2HN_GPIO_PIN_BASE		  416

#define PINS_PER_PORT                     8
#define EXTENDED_REG_OFFSET               0x10

#define OFFSET(pin)                       (pin - RZV2HN_GPIO_PIN_BASE)

#define PIN_ID_TO_PORT(n)                 (n / PINS_PER_PORT)     //port, bank, n=offset
#define PIN_ID_TO_PORT_OFFSET(n)          (PIN_ID_TO_PORT(n) + EXTENDED_REG_OFFSET)  //port reg offset
#define PIN_ID_TO_PIN(n)                  ((n) % PINS_PER_PORT)   //bit, n=offset

#define P(n)                              (0x0000 + 0x10 + (n))
#define PM(n)                             (0x0100 + 0x20 + (n) * 2)
#define PMC(n)                            (0x0200 + 0x10 + (n))
#define PFC(n)                            (0x0400 + 0x40 + (n) * 4)
#define PIN(n)                            (0x0800 + 0x10 + (n))
#define IOLH(n)                           (0x1000 + (n) * 8)   //n=port+EXTENDED_REG_OFFSET
#define PUPD(n)                           (0x1C00 + (n) * 8)

#define PM_INPUT                          0x1
#define PM_OUTPUT                         0x2
#define PM_HIZ                            0x12

#define PFC_PULL_DIS0                     0x0
#define PFC_PULL_DIS1                     0x1
#define PFC_PULL_UP                       0x3
#define PFC_PULL_DOWN                     0x2

#undef BLOCK_SIZE
#define BLOCK_SIZE                        0x2000

/* Non-static add extern definition below */
int rzv2hn_found;
const int (*bcm_to_rzv2hngpio)[64];

//
// pinToGpio:
//	Take a Wiring pin (0 through X) and re-map it to the AML_GPIO pin
//
static const int pinToGpioBananapiAI2HN[64] = {
	// wiringPi number to native gpio number
	488, 426,	//  0 |  1 : P90, P12
	489, 490,	//  2 |  3 : P91, P92
	463, 462,	//  4 |  5 : P57, P56
	459, 484,	//  6 |  7 : P53, P84(PWM-GEN)
	442, 443,	//  8 |  9 : P32(I2C1_SDA), P33(I2C1_SCL)
	503, 502,	// 10 | 11 : PA7(SPI2_SS), PA6
	508, 507,	// 12 | 13 : PB4(SPI2_MOSI), PB3(SPI2_MISO)
	509, 460,	// 14 | 15 : PB5(SPI2_CLK), PB54(UART2_TX)
	461,  -1,	// 16 | 17 : P55(UART2_RX),
	  -1,  -1,	// 18 | 19 :
	  -1, 491,	// 20 | 21 : , P93
	493, 458,	// 22 | 23 : P95, P52
	427, 495,	// 24 | 25 : P13, P97
	456, 457,	// 26 | 27 : P50, P51
	429, 420,	// 28 | 29 : P15, P04
	432, 433,	// 30 | 31 : P20(I2C2_SDA), P21(I2C2_SCL)
	// Padding:
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 32...47
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 48...63
};

const int bcmToOGpioBananapiAI2HN[64] = {	// BCM ModE
      -1,   -1, 442, 443, 484, 491, 493, 502, // 0..7
    503, 507, 508, 509, 456, 458, 460, 461, // 8..15
    457, 488, 426, 427, 429, 420, 490, 463, // 16..23
    462, 459, 495, 489,  -1,  -1,  -1,  -1, // 24..31
// Padding:
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 32..39
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 40..47
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 48..55
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1  // 56..63
};

static const int physToGpioBananapiAI2HN[64] = {
	// physical header pin number to native gpio number
	  -1,		//  0
	  -1,  -1,	//  1 |  2 : 3.3V, 5.0V
	442,  -1,	//  3 |  4 : P32(I2C2_SDA), 5.0V
	443,  -1,	//  5 |  6 : P33(I2C2_SCL), GND
	484, 460,	//  7 |  8 : P84(PWM), P54(UART2_TX)
	  -1, 461,	//  9 | 10 : GND, P55(UAR2_RX)
	488, 426,	// 11 | 12 : P90, P12
	489,  -1,	// 13 | 14 : P91, GND
	490, 463,	// 15 | 16 : P92, P57
	  -1, 462,	// 17 | 18 : 3.3V, P56
	508,  -1,	// 19 | 20 : PB4(SPI2_MOSI), GND
	507, 459,	// 21 | 22 : PB3(SPI2_MISO), P53
	509, 503,	// 23 | 24 : PB5(SPI2_CLK), PA7(SPI2_SS)
	  -1, 502,	// 25 | 26 : GND, PA6
	432, 433,	// 27 | 28 : PG20(I2C2_SDA), P21(I2C2_SCL)
	491,  -1,	// 29 | 30 : P93, GND
	493, 456,	// 31 | 32 : P95, P50
	458,  -1,	// 33 | 34 : P52, GND
	427, 457,	// 35 | 36 : P13, P51
	495, 429,	// 37 | 38 : P97, P15
	  -1, 420,	// 39 | 40 : GND, P04
	// Not used
	-1, -1, -1, -1, -1, -1, -1, -1,	// 41...48
	-1, -1, -1, -1, -1, -1, -1, -1,	// 49...56
	-1, -1, -1, -1, -1, -1, -1	// 57...63
};

#endif /* RZV2HN_SUPPORT */

#else  /* DEFINE_BANANAPI_VARS */

/* =======================================================================================
 *
 * rzv2hn specific
 *
 * ========================================================================================
 */
#ifdef RZV2HN_SUPPORT
extern int rzv2hn_found;
extern const int (*bcm_to_rzv2hngpio)[64];

extern const int physToGpioBananapiAI2HN[64];
extern const int bcmToOGpioBananapiAI2HN[64];

int wiringPiSetupRzv2hn (void);
void wiringPiCleanupRzv2hn (void);
void pinModeRzv2hn (int pin, int mode);
void pullUpDnControlRzv2hn (int pin, int pud);
int digitalReadRzv2hn (int pin);
void digitalWriteRzv2hn (int pin, int value);
int analogReadRzv2hn (int pin);
void analogWriteRzv2hn (int pin, int value);
int pinGetModeRzv2hn (int pin);
void setInfoRzv2hn(char *hardware, void *vinfo);
void setMappingPtrsRzv2hn(void);
#endif /* RZV2HN_SUPPORT */

#endif /* DEFINE_BANANAPI_VARS */

#endif /* BPI_RENESAS_H_INCLUDED */

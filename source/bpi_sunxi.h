/*
 * Bananapi specific code
 */

#ifndef BPI_SUNXI_H_INCLUDED
#define BPI_SUNXI_H_INCLUDED

#ifdef DEFINE_BANANAPI_VARS

/* =======================================================================================
 *
 * Sun50iw9 specific
 *
 * ========================================================================================
 */
#ifdef SUN50IW9_SUPPORT

#define SUN50IW9_GPIO_MASK			(0xFFFFFF00)
#define SUN50IW9_GPIO_BASE			0x0300B000

#define SUN50IW9_GPIO_PIN_BASE		0

/* Non-static add extern definition below */
int sun50iw9_found;
const int (*bcm_to_sun50iw9gpio)[64];

//
// pinToGpio:
//	Take a Wiring pin (0 through X) and re-map it to the AML_GPIO pin
//
static const int pinToGpioBananapiM4Berry[64] = {
	// wiringPi number to native gpio number
	226, 203,	//  0 |  1 : PH2, PG11
	227, 194,	//  2 |  3 : PH3, PG2
	200, 201,	//  4 |  5 : PG8, PG9
	193, 211,	//  6 |  7 : PG1, PG19(PWM1)
	208, 207,	//  8 |  9 : PG16(I2C4_SDA), PG15(I2C4_SCL)
	229, 233,	// 10 | 11 : PH5(SPI1_SS), PH9
	231, 232,	// 12 | 13 : PH7(SPI1_MOSI), PH8(SPI1_MISO)
	230, 198,	// 14 | 15 : PH6(SPI1_CLK), PG6(UART1_TX)
	199,  -1,	// 16 | 17 : PG7(UART1_RX),
	 -1,  -1,	// 18 | 19 :
	 -1, 195,	// 20 | 21 : , PG3
	196, 197,	// 22 | 23 : PG4, PG5
	204, 202,	// 24 | 25 : PG12, PG10
	192, 228,	// 26 | 27 : PG0, PH4
	206, 205,	// 28 | 29 : PG14, PG13
	210, 209,	// 30 | 31 : PG18(I2C3_SDA), PG17(I2C3_SCL)
	// Padding:
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 32...47
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 48...63
};

const int bcmToOGpioBananapiM4Berry[64] = {	// BCM ModE
     -1,  -1, 208, 207, 211, 195, 196, 233, // 0..7
    229, 232, 231, 230, 192, 197, 198, 199, // 8..15
    228, 226, 203, 204, 206, 205, 194, 200, // 16..23
    201, 193, 202, 227,  -1,  -1,  -1,  -1, // 24..31
// Padding:
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 32..39
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 40..47
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 48..55
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1  // 56..63
};

static const int physToGpioBananapiM4Berry[64] = {
	// physical header pin number to native gpio number
	 -1,		//  0
	 -1,  -1,	//  1 |  2 : 3.3V, 5.0V
	208,  -1,	//  3 |  4 : PG16(I2C4_SDA), 5.0V
	207,  -1,	//  5 |  6 : PG15(I2C4_SCL), GND
	211, 198,	//  7 |  8 : PG19, PG6(UART1_TX)
	 -1, 199,	//  9 | 10 : GND(PWM1), PG7(UAR1_RX)
	226, 203,	// 11 | 12 : PH2, PG11
	227,  -1,	// 13 | 14 : PH3, GND
	194, 200,	// 15 | 16 : PG2, PG8
	 -1, 201,	// 17 | 18 : 3.3V, PG9
	231,  -1,	// 19 | 20 : PH7(SPI1_MOSI), GND
	232, 193,	// 21 | 22 : PH8(SPI1_MISO), PG1
	230, 229,	// 23 | 24 : PH6(SPI1_CLK), PH5(SPI1_SS)
	 -1, 233,	// 25 | 26 : GND, PH9
	210, 209,	// 27 | 28 : PG18(I2C3_SDA), PG17(I2C3_SCL)
	195,  -1,	// 29 | 30 : PG3, GND
	196, 192,	// 31 | 32 : PG4, PG0
	197,  -1,	// 33 | 34 : PG5, GND
	204, 228,	// 35 | 36 : PG12, PH4
	202, 206,	// 37 | 38 : PG19, PG14
	 -1, 205,	// 39 | 40 : GND, PG13
	// Not used
	-1, -1, -1, -1, -1, -1, -1, -1,	// 41...48
	-1, -1, -1, -1, -1, -1, -1, -1,	// 49...56
	-1, -1, -1, -1, -1, -1, -1	// 57...63
};

static const int pinToGpioBananapiM4Zero[64] = {
	// wiringPi number to native gpio number
	226, 257,	//  0 |  1 : PH2, PI1
	227, 267,	//  2 |  3 : PH3, PI11
	271, 272,	//  4 |  5 : PI15, PI16
	 66, 268,	//  6 |  7 : PC2, PI12(PWM2)
	262, 261,	//  8 |  9 : PI6(I2C0_SDA), PI5(I2C0_SCL)
	229, 233,	// 10 | 11 : PH5(SPI1_SS), PH9
	231, 232,	// 12 | 13 : PH7(SPI1_MOSI), PH8(SPI1_MISO)
	230, 269,	// 14 | 15 : PH6(SPI1_CLK), PI13(UART4_TX)
	270,  -1,	// 16 | 17 : PI14(UART4_RX),
	 -1,  -1,	// 18 | 19 :
	 -1, 266,	// 20 | 21 : , PI10
	265, 234,	// 22 | 23 : PI9, PH10
	258, 256,	// 24 | 25 : PI2, PI0
	228,  71,	// 26 | 27 : PH4, PC7
	260, 259,	// 28 | 29 : PI4, PI3
	264, 263,	// 30 | 31 : PI8(I2C1_SDA), PI7(I2C1_SCL)
	// Padding:
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 32...47
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 48...63
};

const int bcmToOGpioBananapiM4Zero[64] = {	// BCM ModE
     -1,  -1, 262, 261, 268, 266, 265, 233, // 0..7
    229, 232, 231, 230, 228, 234, 269, 270, // 8..15
    71, 226, 257, 258, 260, 259, 267, 271, // 16..23
    272, 66, 256, 227,  -1,  -1,  -1,  -1, // 24..31
// Padding:
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 32..39
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 40..47
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 48..55
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1  // 56..63
};

static const int physToGpioBananapiM4Zero[64] = {
	// physical header pin number to native gpio number
	 -1,		//  0
	 -1,  -1,	//  1 |  2 : 3.3V, 5.0V
	262,  -1,	//  3 |  4 : PG16(I2C4_SDA), 5.0V
	261,  -1,	//  5 |  6 : PG15(I2C4_SCL), GND
	268, 269,	//  7 |  8 : PG19, PG6(UART1_TX)
	 -1, 270,	//  9 | 10 : GND(PWM1), PG7(UAR1_RX)
	226, 257,	// 11 | 12 : PH2, PG11
	227,  -1,	// 13 | 14 : PH3, GND
	267, 271,	// 15 | 16 : PG2, PG8
	 -1, 272,	// 17 | 18 : 3.3V, PG9
	231,  -1,	// 19 | 20 : PH7(SPI1_MOSI), GND
	232,  66,	// 21 | 22 : PH8(SPI1_MISO), PG1
	230, 229,	// 23 | 24 : PH6(SPI1_CLK), PH5(SPI1_SS)
	 -1, 233,	// 25 | 26 : GND, PH9
	264, 263,	// 27 | 28 : PG18(I2C3_SDA), PG17(I2C3_SCL)
	266,  -1,	// 29 | 30 : PG3, GND
	265, 228,	// 31 | 32 : PG4, PG0
	234,  -1,	// 33 | 34 : PG5, GND
	258,  71,	// 35 | 36 : PG12, PH4
	256, 260,	// 37 | 38 : PG19, PG14
	 -1, 259,	// 39 | 40 : GND, PG13
	// Not used
	-1, -1, -1, -1, -1, -1, -1, -1,	// 41...48
	-1, -1, -1, -1, -1, -1, -1, -1,	// 49...56
	-1, -1, -1, -1, -1, -1, -1	// 57...63
};

// BananaPi M4 Zero V1 (original board with Realtek WiFi, DT model "BananaPi BPI-M4-Zero")
static const int pinToGpioBananapiM4ZeroV1[64] = {
	// wiringPi number to native gpio number
	226, 203,	//  0 |  1 : PH2, PG11
	227, 194,	//  2 |  3 : PH3, PG2
	200, 201,	//  4 |  5 : PG8, PG9
	193, 211,	//  6 |  7 : PG1, PG19
	208, 207,	//  8 |  9 : PG16, PG15
	229, 233,	// 10 | 11 : PH5(SPI1_SS), PH9
	231, 232,	// 12 | 13 : PH7(SPI1_MOSI), PH8(SPI1_MISO)
	230, 198,	// 14 | 15 : PH6(SPI1_CLK), PG6
	199,  -1,	// 16 | 17 : PG7,
	 -1,  -1,	// 18 | 19 :
	 -1, 195,	// 20 | 21 : , PG3
	196, 197,	// 22 | 23 : PG4, PG5
	204, 202,	// 24 | 25 : PG12, PG10
	192, 228,	// 26 | 27 : PG0, PH4
	206, 205,	// 28 | 29 : PG14, PG13
	210, 209,	// 30 | 31 : PG18, PG17
	// Padding:
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 32...47
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 48...63
};

const int bcmToOGpioBananapiM4ZeroV1[64] = {	// BCM Mode
     -1,  -1, 208, 207, 211, 195, 196, 233,	// 0..7
    229, 232, 231, 230, 228, 197, 198, 199,	// 8..15
    228, 226, 203, 204, 206, 205, 194, 200,	// 16..23
    201, 193, 211, 227,  -1,  -1,  -1,  -1,	// 24..31
// Padding:
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,	// 32..39
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,	// 40..47
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,	// 48..55
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1	// 56..63
};

static const int physToGpioBananapiM4ZeroV1[64] = {
	// physical header pin number to native gpio number
	 -1,		//  0
	 -1,  -1,	//  1 |  2 : 3.3V, 5.0V
	208,  -1,	//  3 |  4 : PG16(I2C4_SDA), 5.0V
	207,  -1,	//  5 |  6 : PG15(I2C4_SCL), GND
	211, 198,	//  7 |  8 : PG19, PG6(UART1_TX)
	 -1, 199,	//  9 | 10 : GND, PG7(UART1_RX)
	226, 203,	// 11 | 12 : PH2, PG11
	227,  -1,	// 13 | 14 : PH3, GND
	194, 200,	// 15 | 16 : PG2, PG8
	 -1, 201,	// 17 | 18 : 3.3V, PG9
	231,  -1,	// 19 | 20 : PH7(SPI1_MOSI), GND
	232, 193,	// 21 | 22 : PH8(SPI1_MISO), PG1
	230, 229,	// 23 | 24 : PH6(SPI1_CLK), PH5(SPI1_SS)
	 -1, 233,	// 25 | 26 : GND, PH9
	210, 209,	// 27 | 28 : PG18(I2C3_SDA), PG17(I2C3_SCL)
	195,  -1,	// 29 | 30 : PG3, GND
	196, 192,	// 31 | 32 : PG4, PG0
	197,  -1,	// 33 | 34 : PG5, GND
	204, 228,	// 35 | 36 : PG12, PH4
	202, 206,	// 37 | 38 : PG10, PG14
	 -1, 205,	// 39 | 40 : GND, PG13
	// Not used
	-1, -1, -1, -1, -1, -1, -1, -1,	// 41...48
	-1, -1, -1, -1, -1, -1, -1, -1,	// 49...56
	-1, -1, -1, -1, -1, -1, -1	// 57...63
};

#endif /* SUN50IW9_SUPPORT */

/* =======================================================================================
 *
 * Sun55iw3 specific
 *
 * ========================================================================================
 */
#ifdef SUN55IW3_SUPPORT

#define SUN55IW3_GPIO_MASK                       (0xFFFFFF00)
#define SUN55IW3_GPIO_BASE                       0x02000000
#define SUN55IW3_GPIO_R_BASE                  0x07022000

#define SUN55IW3_GPIO_PIN_BASE               0
#define SUN55IW3_GPIO_PIN_END                 415
#define SUN55IW3_GPIOR_PIN_START          (SUN55IW3_GPIO_PIN_BASE + 352)
#define SUN55IW3_GPIOR_PIN_END               SUN55IW3_GPIO_PIN_END

/* Non-static add extern definition below */
int sun55iw3_found;
const int (*bcm_to_sun55iw3gpio)[64];

//
// pinToGpio:
//	Take a Wiring pin (0 through X) and re-map it to the AML_GPIO pin
//
static const int pinToGpioBananapiF5[64] = {
	// wiringPi number to native gpio number
	  32, 37,	//  0 |  1 : PB0, PB5
	  33, 34,	//  2 |  3 : PB1, PB2
	  43, 44,	//  4 |  5 : PB11, PB12
	263, 266,	//  6 |  7 : PI7, PI10(PWM0_11)
	265, 264,	//  8 |  9 : PI9(I2C5_SDA), PI8(I2C5_SCL)
	258, 262,	// 10 | 11 : PI2(SPI1_SS), PI6
	260, 261,	// 12 | 13 : PI4(SPI1_MOSI), PI5(SPI1_MISO)
	259, 45,	// 14 | 15 : PI3(SPI1_CLK), PB13(UART7_TX)
	  46,  -1,	// 16 | 17 : PB14(UART7_RX),
	 -1,  -1,	// 18 | 19 :
	 -1, 35,	// 20 | 21 : , PB3
	356, 357,	// 22 | 23 : PL4, PL5
	  38, 36,	// 24 | 25 : PB6, PB4
	267, 268,	// 26 | 27 : PI11, PI12
	  40, 39,	// 28 | 29 : PB8, PB7
	257, 256,	// 30 | 31 : PI1(I2C4_SDA), PI0(I2C4_SCL)
	// Padding:
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 32...47
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,	// 48...63
};

const int bcmToOGpioBananapiF5[64] = {	// BCM ModE
     -1,  -1, 265, 264, 266, 35, 356, 262, // 0..7
    258, 261, 260, 259, 267, 357, 45, 46, // 8..15
    268, 32, 37, 38, 40, 39, 34, 43, // 16..23
    44, 263, 36, 33,  -1,  -1,  -1,  -1, // 24..31
// Padding:
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 32..39
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 40..47
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1, // 48..55
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1  // 56..63
};

static const int physToGpioBananapiF5[64] = {
	// physical header pin number to native gpio number
	  -1,		//  0
	  -1,  -1,	//  1 |  2 : 3.3V, 5.0V
	265,  -1,	//  3 |  4 : PG16(I2C4_SDA), 5.0V
	264,  -1,	//  5 |  6 : PG15(I2C4_SCL), GND
	266, 45,	//  7 |  8 : PG19, PG6(UART1_TX)
	  -1, 46,	//  9 | 10 : GND(PWM1), PG7(UAR1_RX)
	  32, 37,	// 11 | 12 : PH2, PG11
	  33,  -1,	// 13 | 14 : PH3, GND
	  34, 43,	// 15 | 16 : PG2, PG8
	  -1, 44,	// 17 | 18 : 3.3V, PG9
	260,  -1,	// 19 | 20 : PH7(SPI1_MOSI), GND
	261, 263,	// 21 | 22 : PH8(SPI1_MISO), PG1
	259, 258,	// 23 | 24 : PH6(SPI1_CLK), PH5(SPI1_SS)
	  -1, 262,	// 25 | 26 : GND, PH9
	257, 256,	// 27 | 28 : PG18(I2C3_SDA), PG17(I2C3_SCL)
	  35,  -1,	// 29 | 30 : PG3, GND
	356, 267,	// 31 | 32 : PG4, PG0
	357,  -1,	// 33 | 34 : PG5, GND
	  38, 268,	// 35 | 36 : PG12, PH4
	  36, 40,	// 37 | 38 : PG19, PG14
	  -1, 39,	// 39 | 40 : GND, PG13
	// Not used
	-1, -1, -1, -1, -1, -1, -1, -1,	// 41...48
	-1, -1, -1, -1, -1, -1, -1, -1,	// 49...56
	-1, -1, -1, -1, -1, -1, -1	// 57...63

};
#endif /* end SUN55IW3_SUPPORT */

#else  /* DEFINE_BANANAPI_VARS */

/* =======================================================================================
 *
 * Sun50iw9 specific
 *
 * ========================================================================================
 */
#ifdef SUN50IW9_SUPPORT
extern int sun50iw9_found;
extern const int (*bcm_to_sun50iw9gpio)[64];

extern const int physToGpioBananapiM4Berry[64];
extern const int bcmToOGpioBananapiM4Berry[64];
extern const int physToGpioBananapiM4Zero[64];
extern const int bcmToOGpioBananapiM4Zero[64];
extern const int physToGpioBananapiM4ZeroV1[64];
extern const int bcmToOGpioBananapiM4ZeroV1[64];

int wiringPiSetupSun50iw9 (void);
void wiringPiCleanupSun50iw9 (void);
void pinModeSun50iw9 (int pin, int mode);
void pullUpDnControlSun50iw9 (int pin, int pud);
int digitalReadSun50iw9 (int pin);
void digitalWriteSun50iw9 (int pin, int value);
int analogReadSun50iw9 (int pin);
void analogWriteSun50iw9 (int pin, int value);
int pinGetModeSun50iw9 (int pin);
void setInfoSun50iw9(char *hardware, void *vinfo);
void setMappingPtrsSun50iw9(void);
#endif /* end SUN50IW9_SUPPORT */

/* =======================================================================================
 *
 * Sun55iw3 specific
 *
 * ========================================================================================
 */
#ifdef SUN55IW3_SUPPORT
extern int sun55iw3_found;
extern const int (*bcm_to_sun55iw3gpio)[64];

extern const int physToGpioBananapiF5[64];
extern const int bcmToOGpioBananapiF5[64];

int wiringPiSetupSun55iw3 (void);
void wiringPiCleanupSun55iw3 (void);
void pinModeSun55iw3 (int pin, int mode);
void pullUpDnControlSun55iw3 (int pin, int pud);
int digitalReadSun55iw3 (int pin);
void digitalWriteSun55iw3 (int pin, int value);
int analogReadSun55iw3 (int pin);
void analogWriteSun55iw3 (int pin, int value);
int pinGetModeSun55iw3 (int pin);
void setInfoSun55iw3(char *hardware, void *vinfo);
void setMappingPtrsSun55iw3(void);

#endif /* SUN55IW3_SUPPORT */

#endif /* DEFINE_BANANAPI_VARS */

#endif /* BPI_SUNXI_H_INCLUDED */

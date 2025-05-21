/*
 * Bananapi specific code
 */

#ifndef BANANAPI_H_INCLUDED
#define BANANAPI_H_INCLUDED

#include <stdarg.h>

#define	PI_MODEL_UNKNOWN                                           0
#define	PI_MODEL_A                                                              1
#define	PI_MODEL_B                                                              2
#define	PI_MODEL_BP                                                            3
#define	PI_MODEL_CM                                                           4
#define	PI_MODEL_AP                                                            5
#define	PI_MODEL_BANANAPIM5                                      6
#define	PI_MODEL_BANANAPIM2S                                   7
#define	PI_MODEL_BANANAPICM4                                   8
#define	PI_MODEL_BANANAPIRPICM4                            9
#define	PI_MODEL_BANANAPICM5IO                              10
#define	PI_MODEL_BANANAPICM5BPICM4IO               11
#define	PI_MODEL_BANANAPIM4BERRY                       12
#define	PI_MODEL_BANANAPIM4ZERO                          13
#define	PI_MODEL_BANANAPIF3                                      14
#define	PI_MODEL_BANANAPIF5                                      15

#define AML_SUPPORT
#define SUN50IW9_SUPPORT
#define SUN55IW3_SUPPORT
#define SPACEMIT_SUPPORT

#define INPUT  1 // is really 0 for control register!
#define OUTPUT 0 // is really 1 for control register!
#define ALT0   4

#define HIGH 1
#define LOW  0

#define PUD_OFF  0
#define PUD_DOWN 1
#define PUD_UP   2

// Failure modes
#define	WPI_FATAL	(1==1)
#define	WPI_ALMOST	(1==2)

#ifndef	TRUE
#define	TRUE		(1==1)
#define	FALSE		(1==2)
#endif

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x)	(sizeof(x) / sizeof((x)[0]))
#endif

#define PAGE_SIZE	(4*1024)
#define BLOCK_SIZE	(4*1024)

#define MAXPINCOUNT	40

extern const int (*pin_to_gpio)[MAXPINCOUNT+1];

//#include "bpi_sunxi.h"
//#include "bpi_aml.h"
//#include "bpi_spacemit.h"

#ifdef DEFINE_BANANAPI_VARS
static int piModel;
static int wiringPiReturnCodes = FALSE ;

static int wiringPiFailure (int fatal, const char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  if (!fatal && wiringPiReturnCodes)
    return -1 ;

  va_start (argp, message) ;
  vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  fprintf (stderr, "%s", buffer) ;
  exit (EXIT_FAILURE) ;

  return 0 ;
}
#else
extern int  piModel;
#endif

#endif /* BANANAPI_H_INCLUDED */

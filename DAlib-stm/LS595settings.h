// Strobe isn't actually used.
#include <libopencm3/stm32/gpio.h>

#define LS595Port               GPIOC_ODR
#define LS595Strobe             15
#define LS595Data               14
#define LS595Clock              13

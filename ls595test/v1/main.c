/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2009 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "stmcommon.h"
#include "LS595.h"

static void gpio_setup(void) {
	/* Enable GPIOC clock. */
	/* Manually: */
	// RCC_APB2ENR |= RCC_APB2ENR_IOPCEN;
	/* Using API functions: */
	rcc_periph_clock_enable(RCC_GPIOC);

	/* Set GPIO12 (in GPIO port C) to 'output push-pull'. */
	/* Manually: */
	// GPIOC_CRH = (GPIO_CNF_OUTPUT_PUSHPULL << (((12 - 8) * 4) + 2));
	// GPIOC_CRH |= (GPIO_MODE_OUTPUT_2_MHZ << ((12 - 8) * 4));
	/* Using API functions: */
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO14);
	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO15);
}

int main(void) {
  unsigned long i, j;

  gpio_setup();

  /* Blink the LED (PC12) on the board. */
  while (1) {
    for (j = 0; j < 256; j++) {
      
      LS595StrobeL();
      LS595Send(~j);
      LS595StrobeH();
    
      for (i = 0; i < 50000; i++)  NOP();

    }
  }

  return 0;
}


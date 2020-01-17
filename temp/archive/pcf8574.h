/*
 * This file is hacked from the libopencm3 project.
 *
 * Copyright (C) 2010 Thomas Otto <tommi@viadmin.org>
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

#ifndef STTS75_H
 #define STTS75_H

 #include <stdint.h>

 #define PCF8574_0		0x70
 #define PCF8574_1		0x71
 #define PCF8574_2		0x72
 #define PCF8574_3		0x73
 #define PCF8574_4		0x74
 #define PCF8574_5		0x75
 #define PCF8574_6		0x76
 #define PCF8574_7		0x77


 void pcf8574_write(uint32_t i2c, uint8_t sensor, uint8_t temp_os);
 //uint16_t stts75_read_temperature(uint32_t i2c, uint8_t sensor);

#endif

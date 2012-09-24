/* gps-bricklet
 * Copyright (C) 2012 Matthias Bolte <matthias@tinkerforge.com>
 *
 * config.h: GPS Bricklet specific configuration
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#include "bricklib/drivers/board/sam3s/SAM3S.h"
#include "bricklib/drivers/pio/pio.h"

#include "gps.h"

#define BOARD_MCK 64000000

#define BRICKLET_HARDWARE_NAME "GPS Bricklet 1.0"
#define BRICKLET_FIRMWARE_VERSION_MAJOR 1
#define BRICKLET_FIRMWARE_VERSION_MINOR 0
#define BRICKLET_FIRMWARE_VERSION_REVISION 0

#define LOGGING_LEVEL LOGGING_DEBUG
#define DEBUG_BRICKLET 1

#define INVOCATION_IN_BRICKLET_CODE

#define BUFFER_SIZE 250

typedef struct {
	uint32_t last_irq_value;
	char buffer[BUFFER_SIZE];
	uint8_t buffer_used;
	
	uint16_t timeout_counter;
	
	
	
	uint16_t utc, msec; // hhmmss.sss
} BrickContext;

#endif

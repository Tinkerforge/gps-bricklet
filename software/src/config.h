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
#define DEBUG_BRICKLET 0

#define INVOCATION_IN_BRICKLET_CODE

#define BUFFER_SIZE 180

typedef struct {
	char buffer[BUFFER_SIZE];
	uint8_t buffer_used;
	
	uint16_t timeout_counter;

	uint32_t period_coordinates;
	uint32_t period_coordinates_counter;
	bool period_coordinates_new;

	uint32_t period_status;
	uint32_t period_status_counter;
	bool period_status_new;

	bool is_configured_to_57600;

	// Coordinates
	char ns;
	uint32_t latitude;
	char ew;
	uint32_t longitude;
	uint16_t pdop;
	uint16_t hdop;
	uint16_t vdop;

	// Status
	uint8_t fix;
	uint8_t satellites_view;
	uint8_t satellites_used;
	uint16_t speed;  // km/h/100
	uint16_t course; // degree/100
	uint32_t date; // ddmmyy
	uint32_t time;  // hhmmss
	int16_t altitude;
	int16_t geoidal_separation;
} BrickContext;

#endif

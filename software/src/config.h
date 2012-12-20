/* gps-bricklet
 * Copyright (C) 2012 Matthias Bolte <matthias@tinkerforge.com>
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
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

#define BRICKLET_FIRMWARE_VERSION_MAJOR 2
#define BRICKLET_FIRMWARE_VERSION_MINOR 0
#define BRICKLET_FIRMWARE_VERSION_REVISION 0

#define BRICKLET_HARDWARE_VERSION_MAJOR 1
#define BRICKLET_HARDWARE_VERSION_MINOR 0
#define BRICKLET_HARDWARE_VERSION_REVISION 0

#define BRICKLET_DEVICE_IDENTIFIER 222

#define LOGGING_LEVEL LOGGING_DEBUG
#define DEBUG_BRICKLET 0

#define INVOCATION_IN_BRICKLET_CODE

typedef struct {
	uint8_t preamble[2];
	uint32_t time;
	uint32_t date;
	uint32_t latitude;
	uint8_t ns;
	uint32_t longitude;
	uint8_t ew;
	uint8_t fix_type;
	uint8_t fix_mode;
	uint32_t altitude;
	uint32_t geoidal_separation;
	uint32_t course;
	uint32_t speed;
	uint8_t satellites_view;
	uint8_t satellites_used;
	uint16_t pdop;
	uint16_t hdop;
	uint16_t vdop;
	uint16_t epe;
	uint8_t asterisk;
	uint8_t checksum;
	uint8_t end[2];
} __attribute__((packed)) PackedBinarySentence;

typedef struct {
	uint32_t time;
	uint32_t date;
	uint32_t latitude;
	uint8_t ns;
	uint32_t longitude;
	uint8_t ew;
	uint8_t fix_type;
	uint8_t fix_mode;
	uint32_t altitude;
	uint32_t geoidal_separation;
	uint32_t course;
	uint32_t speed;
	uint8_t satellites_view;
	uint8_t satellites_used;
	uint16_t pdop;
	uint16_t hdop;
	uint16_t vdop;
	uint16_t epe;
} UnpackedBinarySentence;

typedef struct {
	bool in_sync;
	PackedBinarySentence packed_sentence;
	uint8_t packed_sentence_used;
	UnpackedBinarySentence unpacked_sentence;

	uint32_t period_coordinates;
	uint32_t period_coordinates_counter;
	bool period_coordinates_new;

	uint32_t period_status;
	uint32_t period_status_counter;
	bool period_status_new;

	uint32_t period_altitude;
	uint32_t period_altitude_counter;
	bool period_altitude_new;

	uint32_t period_motion;
	uint32_t period_motion_counter;
	bool period_motion_new;

	uint32_t period_date_time;
	uint32_t period_date_time_counter;
	bool period_date_time_new;
} BrickContext;

#endif

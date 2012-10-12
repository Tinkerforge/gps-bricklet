/* gps-bricklet
 * Copyright (C) 2012 Matthias Bolte <matthias@tinkerforge.com>
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * gps.h: Implementation of GPS Bricklet messages
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

#ifndef GPS_H
#define GPS_H

#include <stdint.h>

#define TYPE_GET_COORDINATES 1
#define TYPE_GET_STATUS 2
#define TYPE_RESTART 3
#define TYPE_SET_COORDINATES_CALLBACK_PERIOD 4
#define TYPE_GET_COORDINATES_CALLBACK_PERIOD 5
#define TYPE_SET_STATUS_CALLBACK_PERIOD 6
#define TYPE_GET_STATUS_CALLBACK_PERIOD 7
#define TYPE_COORDINATES 8
#define TYPE_STATUS 9

#define NUM_MESSAGES 9

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetCoordinates;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	char ns;
	uint32_t latitude;
	char ew;
	uint32_t longitude;
	uint16_t pdop;
	uint16_t hdop;
	uint16_t vdop;
} __attribute__((__packed__)) GetCoordinatesReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetStatus;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t fix;
	uint8_t satellites_view;
	uint8_t satellites_used;
	uint16_t speed;
	uint16_t course;
	uint32_t date;
	uint32_t time;
	int16_t altitude;
	int16_t geoidal_separation;
} __attribute__((__packed__)) GetStatusReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t restart_type;
} __attribute__((__packed__)) Restart;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint32_t period;
} __attribute__((__packed__)) SetCoordinatesCallbackPeriod;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetCoordinatesCallbackPeriod;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint32_t period;
} __attribute__((__packed__)) GetCoordinatesCallbackPeriodReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint32_t period;
} __attribute__((__packed__)) SetStatusCallbackPeriod;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) GetStatusCallbackPeriod;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint32_t period;
} __attribute__((__packed__)) GetStatusCallbackPeriodReturn;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	char ns;
	uint32_t latitude;
	char ew;
	uint32_t longitude;
	uint16_t pdop;
	uint16_t hdop;
	uint16_t vdop;
} __attribute__((__packed__)) Coordinates;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
	uint8_t fix;
	uint8_t satellites_view;
	uint8_t satellites_used;
	uint16_t speed;
	uint16_t course;
	uint32_t date;
	uint32_t time;
	int16_t altitude;
	int16_t geoidal_separation;
} __attribute__((__packed__)) Status;

typedef struct {
	uint8_t stack_id;
	uint8_t type;
	uint16_t length;
} __attribute__((__packed__)) StandardMessage;

void get_coordinates(uint8_t com, const GetCoordinates *data);
void get_status(uint8_t com, const GetStatus *data);
void restart(uint8_t com, const Restart *data);
void set_coordinates_callback_period(uint8_t com, const SetCoordinatesCallbackPeriod *data);
void get_coordinates_callback_period(uint8_t com, const GetCoordinatesCallbackPeriod *data);
void set_status_callback_period(uint8_t com, const SetStatusCallbackPeriod *data);
void get_status_callback_period(uint8_t com, const GetStatusCallbackPeriod *data);


#define I2C_EEPROM_ADDRESS_HIGH 84

#define I2C_ADDRESS_HIGH 76 // 0b1001100
#define I2C_ADDRESS_LOW 77 // 0b1001101

#define I2C_INTERNAL_ADDRESS_RHR 0x00
#define I2C_INTERNAL_ADDRESS_THR 0x00
#define I2C_INTERNAL_ADDRESS_IER 0x01
#define I2C_INTERNAL_ADDRESS_FCR 0x02
#define I2C_INTERNAL_ADDRESS_IIR 0x02
#define I2C_INTERNAL_ADDRESS_LCR 0x03
#define I2C_INTERNAL_ADDRESS_LSR 0x05
#define I2C_INTERNAL_ADDRESS_RXLVL 0x09

#define I2C_INTERNAL_ADDRESS_DLL 0x00
#define I2C_INTERNAL_ADDRESS_DLH 0x01
#define I2C_INTERNAL_ADDRESS_EFR 0x02

#define SC16IS750_LSR_DATA_IN_RECEIVER (1 << 0)
#define SC16IS750_LSR_OVERRUN_ERROR    (1 << 1)

void sc16is750_init(uint8_t baud);
void mt3329_write_str(const char *str);
uint8_t sc16is750_get_address(void);
uint8_t sc16is750_read_register(uint8_t address);
void sc16is750_write_register(uint8_t address, uint8_t value);
uint8_t hex_to_int(char hex);
bool streq(const char *s1, const char *s2);
bool parse_buffer(void);
void mt3329_disable(void);
void mt3329_enable(void);
void sc16is750_reset(void);

void invocation(uint8_t com, uint8_t *data);
void constructor(void);
void destructor(void);
void tick(uint8_t tick_type);

#endif

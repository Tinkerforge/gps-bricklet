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

#include "bricklib/com/com_common.h"

#define FID_GET_COORDINATES 1
#define FID_GET_STATUS 2
#define FID_GET_ALTITUDE 3
#define FID_GET_MOTION 4
#define FID_GET_DATE_TIME 5
#define FID_RESTART 6
#define FID_SET_COORDINATES_CALLBACK_PERIOD 7
#define FID_GET_COORDINATES_CALLBACK_PERIOD 8
#define FID_SET_STATUS_CALLBACK_PERIOD 9
#define FID_GET_STATUS_CALLBACK_PERIOD 10
#define FID_SET_ALTITUDE_CALLBACK_PERIOD 11
#define FID_GET_ALTITUDE_CALLBACK_PERIOD 12
#define FID_SET_MOTION_CALLBACK_PERIOD 13
#define FID_GET_MOTION_CALLBACK_PERIOD 14
#define FID_SET_DATE_TIME_CALLBACK_PERIOD 15
#define FID_GET_DATE_TIME_CALLBACK_PERIOD 16
#define FID_COORDINATES 17
#define FID_STATUS 18
#define FID_ALTITUDE 19
#define FID_MOTION 20
#define FID_DATE_TIME 21

#define FID_LAST 21

#define NUM_MESSAGES 16

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetCoordinates;

typedef struct {
	MessageHeader header;
	uint32_t latitude;
	char ns;
	uint32_t longitude;
	char ew;
	uint16_t pdop;
	uint16_t hdop;
	uint16_t vdop;
	uint16_t epe;
} __attribute__((__packed__)) GetCoordinatesReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetStatus;

typedef struct {
	MessageHeader header;
	uint8_t fix;
	uint8_t satellites_view;
	uint8_t satellites_used;
} __attribute__((__packed__)) GetStatusReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetAltitude;

typedef struct {
	MessageHeader header;
	uint32_t altitude;
	uint32_t geoidal_separation;
} __attribute__((__packed__)) GetAltitudeReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetMotion;

typedef struct {
	MessageHeader header;
	uint32_t course;
	uint32_t speed;
} __attribute__((__packed__)) GetMotionReturn;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetDateTime;

typedef struct {
	MessageHeader header;
	uint32_t date;
	uint32_t time;
} __attribute__((__packed__)) GetDateTimeReturn;

typedef struct {
	MessageHeader header;
	uint8_t restart_type;
} __attribute__((__packed__)) Restart;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) SetCoordinatesCallbackPeriod;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetCoordinatesCallbackPeriod;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) GetCoordinatesCallbackPeriodReturn;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) SetStatusCallbackPeriod;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetStatusCallbackPeriod;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) GetStatusCallbackPeriodReturn;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) SetAltitudeCallbackPeriod;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetAltitudeCallbackPeriod;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) GetAltitudeCallbackPeriodReturn;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) SetMotionCallbackPeriod;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetMotionCallbackPeriod;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) GetMotionCallbackPeriodReturn;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) SetDateTimeCallbackPeriod;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) GetDateTimeCallbackPeriod;

typedef struct {
	MessageHeader header;
	uint32_t period;
} __attribute__((__packed__)) GetDateTimeCallbackPeriodReturn;

typedef struct {
	MessageHeader header;
	uint32_t latitude;
	char ns;
	uint32_t longitude;
	char ew;
	uint16_t pdop;
	uint16_t hdop;
	uint16_t vdop;
	uint16_t epe;
} __attribute__((__packed__)) Coordinates;

typedef struct {
	MessageHeader header;
	uint8_t fix;
	uint8_t satellites_view;
	uint8_t satellites_used;
} __attribute__((__packed__)) Status;

typedef struct {
	MessageHeader header;
	uint32_t altitude;
	uint32_t geoidal_separation;
} __attribute__((__packed__)) Altitude;

typedef struct {
	MessageHeader header;
	uint32_t course;
	uint32_t speed;
} __attribute__((__packed__)) Motion;

typedef struct {
	MessageHeader header;
	uint32_t date;
	uint32_t time;
} __attribute__((__packed__)) DateTime;

typedef struct {
	MessageHeader header;
} __attribute__((__packed__)) StandardMessage;

void get_coordinates(const ComType com, const GetCoordinates *data);
void get_status(const ComType com, const GetStatus *data);
void get_altitude(const ComType com, const GetAltitude *data);
void get_motion(const ComType com, const GetMotion *data);
void get_date_time(const ComType com, const GetDateTime *data);
void restart(const ComType com, const Restart *data);
void set_coordinates_callback_period(const ComType com, const SetCoordinatesCallbackPeriod *data);
void get_coordinates_callback_period(const ComType com, const GetCoordinatesCallbackPeriod *data);
void set_status_callback_period(const ComType com, const SetStatusCallbackPeriod *data);
void get_status_callback_period(const ComType com, const GetStatusCallbackPeriod *data);
void set_altitude_callback_period(const ComType com, const SetAltitudeCallbackPeriod *data);
void get_altitude_callback_period(const ComType com, const GetAltitudeCallbackPeriod *data);
void set_motion_callback_period(const ComType com, const SetMotionCallbackPeriod *data);
void get_motion_callback_period(const ComType com, const GetMotionCallbackPeriod *data);
void set_date_time_callback_period(const ComType com, const SetDateTimeCallbackPeriod *data);
void get_date_time_callback_period(const ComType com, const GetDateTimeCallbackPeriod *data);

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

#define SC16IS740_LSR_DATA_IN_RECEIVER (1 << 0)
#define SC16IS740_LSR_OVERRUN_ERROR    (1 << 1)
#define SC16IS740_LSR_THR_IS_EMPTY     (1 << 5)

void sc16is740_init(void);
void mt3339_write_str(const char *str);
uint8_t sc16is740_get_address(void);
uint8_t sc16is740_read_register(const uint8_t address);
void sc16is740_write_register(const uint8_t address, uint8_t value);
uint16_t swap_uint16(const uint16_t value);
uint32_t swap_uint32(const uint32_t value);
bool parse_buffer(void);
void mt3329_disable(void);
void mt3329_enable(void);
void sc16is740_reset(void);

void invocation(const ComType com, const uint8_t *data);
void constructor(void);
void destructor(void);
void tick(const uint8_t tick_type);

#endif

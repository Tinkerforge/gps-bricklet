/* gps-bricklet
 * Copyright (C) 2012 Matthias Bolte <matthias@tinkerforge.com>
 * Copyright (C) 2012 Olaf Lüke <olaf@tinkerforge.com>
 *
 * gps.c: Implementation of GPS Bricklet messages
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

#include "gps.h"

#include "brickletlib/bricklet_entry.h"
#include "bricklib/bricklet/bricklet_communication.h"
#include "bricklib/utility/util_definitions.h"
#include "config.h"

uint8_t sc16is740_get_address(void) {
	if(BS->address == I2C_EEPROM_ADDRESS_HIGH) {
		return I2C_ADDRESS_HIGH;
	} else {
		return I2C_ADDRESS_LOW;
	}
}

uint8_t sc16is740_read_register(const uint8_t address) {
	uint8_t value = 0;

	BA->TWID_Read(BA->twid,
	              sc16is740_get_address(),
	              address << 3,
	              1,
	              &value,
	              1,
	              NULL);

	return value;
}

void sc16is740_write_register(const uint8_t address, uint8_t value) {
	BA->TWID_Write(BA->twid,
	               sc16is740_get_address(),
	               address << 3,
	               1,
	               &value,
	               1,
	               NULL);
}

uint16_t swap_uint16(const uint16_t value) {
	uint16_t result;

	((uint8_t *)&result)[0] = ((uint8_t *)&value)[1];
	((uint8_t *)&result)[1] = ((uint8_t *)&value)[0];

	return result;
}

uint32_t swap_uint32(const uint32_t value) {
	uint32_t result;

	((uint8_t *)&result)[0] = ((uint8_t *)&value)[3];
	((uint8_t *)&result)[1] = ((uint8_t *)&value)[2];
	((uint8_t *)&result)[2] = ((uint8_t *)&value)[1];
	((uint8_t *)&result)[3] = ((uint8_t *)&value)[0];

	return result;
}

bool parse_buffer(void) {
	// Preamble and end
	if(BC->buffer.preamble[0] != 0x04 ||
	   BC->buffer.preamble[1] != '$' ||
	   BC->buffer.asterisk != '*' ||
	   BC->buffer.end[0] != '\r' ||
	   BC->buffer.end[1] != '\n') {
		return false;
	}

	// Checksum
	uint8_t xor = 0;

	for(uint8_t *p = ((uint8_t *)&BC->buffer) + offsetof(BinarySentence, time);
	    p < ((uint8_t *)&BC->buffer) + offsetof(BinarySentence, asterisk); p++) {
		xor ^= *p;
	}

	if(xor != BC->buffer.checksum) {
		return false;
	}

	// Validate
	if(BC->buffer.ns != 1 && BC->buffer.ns != 2) {
		return false;
	}

	if(BC->buffer.ew != 1 && BC->buffer.ew != 2) {
		return false;
	}

	if(BC->buffer.fix_type != 1 &&
	   BC->buffer.fix_type != 2 &&
	   BC->buffer.fix_type != 3) {
		return false;
	}

	if(BC->buffer.fix_mode != 0 &&
	   BC->buffer.fix_mode != 1 &&
	   BC->buffer.fix_mode != 2) {
		return false;
	}

	// Big to little endian
	BC->buffer.time               = swap_uint32(BC->buffer.time);
	BC->buffer.date               = swap_uint32(BC->buffer.date);
	BC->buffer.latitude           = swap_uint32(BC->buffer.latitude);
	BC->buffer.longitude          = swap_uint32(BC->buffer.longitude);
	BC->buffer.altitude           = swap_uint32(BC->buffer.altitude);
	BC->buffer.geoidal_separation = swap_uint32(BC->buffer.geoidal_separation);
	BC->buffer.course             = swap_uint32(BC->buffer.course);
	BC->buffer.speed              = swap_uint32(BC->buffer.speed);
	BC->buffer.pdop               = swap_uint16(BC->buffer.pdop);
	BC->buffer.hdop               = swap_uint16(BC->buffer.hdop);
	BC->buffer.vdop               = swap_uint16(BC->buffer.vdop);
	BC->buffer.epe                = swap_uint16(BC->buffer.epe);

	return true;
}

void mt3339_write_str(const char *str) {
	while(*str != '\0') {
		const uint8_t lsr = sc16is740_read_register(I2C_INTERNAL_ADDRESS_LSR);
		if(lsr & (1 << 5)) {
			sc16is740_write_register(I2C_INTERNAL_ADDRESS_THR, *str);
			str++;
		}
	}
}

void mt3339_disable(void) {
	BS->pin2_da.type = PIO_OUTPUT_0;
	BA->PIO_Configure(&BS->pin2_da, 1);
	SLEEP_MS(1);
}

void mt3339_enable(void) {
	BS->pin2_da.type = PIO_OUTPUT_1;
	BA->PIO_Configure(&BS->pin2_da, 1);
}

void sc16is740_reset(void) {
	BS->pin3_pwm.type = PIO_OUTPUT_0;
	BA->PIO_Configure(&BS->pin3_pwm, 1);

	SLEEP_MS(5);

	BS->pin3_pwm.type = PIO_OUTPUT_1;
	BA->PIO_Configure(&BS->pin3_pwm, 1);

	SLEEP_MS(100); // Wait for SC16IS740 initialization and clock stabilization
}

void sc16is740_init(void) {
	if(BA->mutex_take(*BA->mutex_twi_bricklet, 10)) {
		BA->bricklet_select(BS->port - 'a');

		// Configure UART
		uint8_t lcr = sc16is740_read_register(I2C_INTERNAL_ADDRESS_LCR);

		lcr |= (1 << 1) | (1 << 0); // Word length 8 bit: LCR[1], LCR[0] = 1, 1
		lcr &= ~(1 << 2); // 1 stop bit: LCR[2] = 0
		lcr &= ~(1 << 3); // No parity: LCR[3] = 0
		sc16is740_write_register(I2C_INTERNAL_ADDRESS_LCR, lcr);

		// Configure baudrate: ((14.7456 MHz * 1000000) / 1) /
		//                     (115200 baud * 16) = 8
		lcr |= 1 << 7; // Divisor latch enable: LCR[7] = 1
		sc16is740_write_register(I2C_INTERNAL_ADDRESS_LCR, lcr);

		sc16is740_write_register(I2C_INTERNAL_ADDRESS_DLL, 8);
		sc16is740_write_register(I2C_INTERNAL_ADDRESS_DLH, 0);

		lcr = sc16is740_read_register(I2C_INTERNAL_ADDRESS_LCR);
		lcr &= ~(1 << 7); // Divisor latch disable: LCR[7] = 0
		sc16is740_write_register(I2C_INTERNAL_ADDRESS_LCR, lcr);

		// Configure FIFOs
		uint8_t fcr = sc16is740_read_register(I2C_INTERNAL_ADDRESS_FCR);

		fcr |= (1 << 0); // Enable FIFOs: FCR[0] = 1
		sc16is740_write_register(I2C_INTERNAL_ADDRESS_FCR, fcr);

		BA->bricklet_deselect(BS->port - 'a');
		BA->mutex_give(*BA->mutex_twi_bricklet);
	}
}

void constructor(void) {
	_Static_assert(sizeof(BrickContext) <= BRICKLET_CONTEXT_MAX_SIZE,
	               "BrickContext too big");

	mt3339_disable();
	sc16is740_reset();

	BC->in_sync = false;

	BC->period_coordinates = 0;
	BC->period_coordinates_counter = 0;
	BC->period_coordinates_new = false;

	BC->period_status = 0;
	BC->period_status_counter = 0;
	BC->period_status_new = false;

	BC->period_altitude = 0;
	BC->period_altitude_counter = 0;
	BC->period_altitude_new = false;

	BC->period_motion = 0;
	BC->period_motion_counter = 0;
	BC->period_motion_new = false;

	BC->period_date_time = 0;
	BC->period_date_time_counter = 0;
	BC->period_date_time_new = false;

	sc16is740_init();
	mt3339_enable();
}

void destructor(void) {
}

void tick(const uint8_t tick_type) {
	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
		if(BA->mutex_take(*BA->mutex_twi_bricklet, 10)) {
			BA->bricklet_select(BS->port - 'a');

			uint8_t lsr = sc16is740_read_register(I2C_INTERNAL_ADDRESS_LSR);

			if((lsr & 0x1C) != 0) {
				// TODO: error handling?
				BC->in_sync = false;
			} else if(lsr & SC16IS740_LSR_OVERRUN_ERROR) {
				// TODO: error handling?
				BC->in_sync = false;
			} else if(lsr & SC16IS740_LSR_DATA_IN_RECEIVER) {
				uint8_t rxlvl = sc16is740_read_register(I2C_INTERNAL_ADDRESS_RXLVL);

				if(BC->in_sync) {
					if(rxlvl >= sizeof(BinarySentence)) {
						// read and parse all data
						uint8_t *p = (uint8_t *)&BC->buffer;

						for(uint8_t i = 0; i < sizeof(BinarySentence); i++) {
							*p++ = sc16is740_read_register(I2C_INTERNAL_ADDRESS_RHR);
						}

						bool result = parse_buffer();

						if(!result) {
							BC->in_sync = false;
						}

						BC->period_coordinates_new = result;
						BC->period_status_new = result;
						BC->period_altitude_new = result;
						BC->period_motion_new = result;
						BC->period_date_time_new = result;
					}
				} else {
					// drop all data
					for(uint8_t i = 0; i < rxlvl; i++) {
						sc16is740_read_register(I2C_INTERNAL_ADDRESS_RHR);
					}
				}
			} else if(!BC->in_sync) {
				BC->in_sync = true;
			}

			BA->bricklet_deselect(BS->port - 'a');
			BA->mutex_give(*BA->mutex_twi_bricklet);
		}

		if(BC->period_coordinates_counter != 0) {
			BC->period_coordinates_counter--;
		}

		if(BC->period_status_counter != 0) {
			BC->period_status_counter--;
		}

		if(BC->period_altitude_counter != 0) {
			BC->period_altitude_counter--;
		}

		if(BC->period_motion_counter != 0) {
			BC->period_motion_counter--;
		}

		if(BC->period_date_time_counter != 0) {
			BC->period_date_time_counter--;
		}
	}

	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		if(BC->period_coordinates != 0 &&
		   BC->period_coordinates_new &&
		   BC->period_coordinates_counter == 0) {
			Coordinates coordinates;
			BA->com_make_default_header(&coordinates, BS->uid, sizeof(Coordinates), FID_COORDINATES);

			coordinates.latitude  = BC->buffer.latitude;
			coordinates.ns        = BC->buffer.ns == 1 ? 'N' : 'S';
			coordinates.longitude = BC->buffer.longitude;
			coordinates.ew        = BC->buffer.ew == 1 ? 'E' : 'W';
			coordinates.pdop      = BC->buffer.pdop;
			coordinates.hdop      = BC->buffer.hdop;
			coordinates.vdop      = BC->buffer.vdop;
			coordinates.epe       = BC->buffer.epe;

			BA->send_blocking_with_timeout(&coordinates,
			                               sizeof(Coordinates),
			                               *BA->com_current);

			BC->period_coordinates_counter = BC->period_coordinates;
			BC->period_coordinates_new = false;
		}

		if(BC->period_status != 0 &&
		   BC->period_status_new &&
		   BC->period_status_counter == 0) {
			Status status;
			BA->com_make_default_header(&status, BS->uid, sizeof(Status), FID_STATUS);

			status.fix             = BC->buffer.fix_type;
			status.satellites_view = BC->buffer.satellites_view;
			status.satellites_used = BC->buffer.satellites_used;

			BA->send_blocking_with_timeout(&status,
			                               sizeof(Status),
			                               *BA->com_current);

			BC->period_status_counter = BC->period_status;
			BC->period_status_new = false;
		}

		if(BC->period_altitude != 0 &&
		   BC->period_altitude_new &&
		   BC->period_altitude_counter == 0) {
			Altitude altitude;
			BA->com_make_default_header(&altitude, BS->uid, sizeof(Altitude), FID_ALTITUDE);

			altitude.altitude           = BC->buffer.altitude;
			altitude.geoidal_separation = BC->buffer.geoidal_separation;

			BA->send_blocking_with_timeout(&altitude,
			                               sizeof(Altitude),
			                               *BA->com_current);

			BC->period_altitude_counter = BC->period_altitude;
			BC->period_altitude_new = false;
		}

		if(BC->period_motion != 0 &&
		   BC->period_motion_new &&
		   BC->period_motion_counter == 0) {
			Motion motion;
			BA->com_make_default_header(&motion, BS->uid, sizeof(Motion), FID_MOTION);

			motion.course = BC->buffer.course;
			motion.speed  = BC->buffer.speed;

			BA->send_blocking_with_timeout(&motion,
			                               sizeof(Motion),
			                               *BA->com_current);

			BC->period_motion_counter = BC->period_motion;
			BC->period_motion_new = false;
		}

		if(BC->period_date_time != 0 &&
		   BC->period_date_time_new &&
		   BC->period_date_time_counter == 0) {
			DateTime date_time;
			BA->com_make_default_header(&date_time, BS->uid, sizeof(DateTime), FID_DATE_TIME);

			date_time.date = BC->buffer.date;
			date_time.time = BC->buffer.time;

			BA->send_blocking_with_timeout(&date_time,
			                               sizeof(DateTime),
			                               *BA->com_current);

			BC->period_date_time_counter = BC->period_date_time;
			BC->period_date_time_new = false;
		}
	}
}

void invocation(const ComType com, const uint8_t *data) {
	switch(((StandardMessage*)data)->header.fid) {
		case FID_GET_COORDINATES:                 get_coordinates(com, (GetCoordinates*)data); break;
		case FID_GET_STATUS:                      get_status(com, (GetStatus*)data); break;
		case FID_GET_ALTITUDE:                    get_altitude(com, (GetAltitude*)data); break;
		case FID_GET_MOTION:                      get_motion(com, (GetMotion*)data); break;
		case FID_GET_DATE_TIME:                   get_date_time(com, (GetDateTime*)data); break;
		case FID_RESTART:                         restart(com, (Restart*)data); break;
		case FID_SET_COORDINATES_CALLBACK_PERIOD: set_coordinates_callback_period(com, (SetCoordinatesCallbackPeriod*)data); break;
		case FID_GET_COORDINATES_CALLBACK_PERIOD: get_coordinates_callback_period(com, (GetCoordinatesCallbackPeriod*)data); break;
		case FID_SET_STATUS_CALLBACK_PERIOD:      set_status_callback_period(com, (SetStatusCallbackPeriod*)data); break;
		case FID_GET_STATUS_CALLBACK_PERIOD:      get_status_callback_period(com, (GetStatusCallbackPeriod*)data); break;
		case FID_SET_ALTITUDE_CALLBACK_PERIOD:    set_altitude_callback_period(com, (SetAltitudeCallbackPeriod*)data); break;
		case FID_GET_ALTITUDE_CALLBACK_PERIOD:    get_altitude_callback_period(com, (GetAltitudeCallbackPeriod*)data); break;
		case FID_SET_MOTION_CALLBACK_PERIOD:      set_motion_callback_period(com, (SetMotionCallbackPeriod*)data); break;
		case FID_GET_MOTION_CALLBACK_PERIOD:      get_motion_callback_period(com, (GetMotionCallbackPeriod*)data); break;
		case FID_SET_DATE_TIME_CALLBACK_PERIOD:   set_date_time_callback_period(com, (SetDateTimeCallbackPeriod*)data); break;
		case FID_GET_DATE_TIME_CALLBACK_PERIOD:   get_date_time_callback_period(com, (GetDateTimeCallbackPeriod*)data); break;
		default: BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_NOT_SUPPORTED, com); break;
	}
}

void get_coordinates(const ComType com, const GetCoordinates *data) {
	GetCoordinatesReturn gcr;

	gcr.header         = data->header;
	gcr.header.length  = sizeof(GetCoordinatesReturn);
	gcr.latitude       = BC->buffer.latitude;
	gcr.ns             = BC->buffer.ns == 1 ? 'N' : 'S';
	gcr.longitude      = BC->buffer.longitude;
	gcr.ew             = BC->buffer.ew == 1 ? 'E' : 'W';
	gcr.pdop           = BC->buffer.pdop;
	gcr.hdop           = BC->buffer.hdop;
	gcr.vdop           = BC->buffer.vdop;
	gcr.epe            = BC->buffer.epe;

	BA->send_blocking_with_timeout(&gcr, sizeof(GetCoordinatesReturn), com);
}

void get_status(const ComType com, const GetStatus *data) {
	GetStatusReturn gsr;

	gsr.header             = data->header;
	gsr.header.length      = sizeof(GetStatusReturn);
	gsr.fix                = BC->buffer.fix_type;
	gsr.satellites_view    = BC->buffer.satellites_view;
	gsr.satellites_used    = BC->buffer.satellites_used;

	BA->send_blocking_with_timeout(&gsr, sizeof(GetStatusReturn), com);
}

void get_altitude(const ComType com, const GetAltitude *data) {
	GetAltitudeReturn gar;

	gar.header             = data->header;
	gar.header.length      = sizeof(GetAltitudeReturn);
	gar.altitude           = BC->buffer.altitude;
	gar.geoidal_separation = BC->buffer.geoidal_separation;

	BA->send_blocking_with_timeout(&gar, sizeof(GetAltitudeReturn), com);
}

void get_motion(const ComType com, const GetMotion *data) {
	GetMotionReturn gmr;

	gmr.header             = data->header;
	gmr.header.length      = sizeof(GetMotionReturn);
	gmr.course             = BC->buffer.course;
	gmr.speed              = BC->buffer.speed;

	BA->send_blocking_with_timeout(&gmr, sizeof(GetMotionReturn), com);
}

void get_date_time(const ComType com, const GetDateTime *data) {
	GetDateTimeReturn gdtr;

	gdtr.header             = data->header;
	gdtr.header.length      = sizeof(GetDateTimeReturn);
	gdtr.date               = BC->buffer.date;
	gdtr.time               = BC->buffer.time;

	BA->send_blocking_with_timeout(&gdtr, sizeof(GetDateTimeReturn), com);
}

void restart(const ComType com, const Restart *data) {
	if(data->restart_type > 3) {
		return;
	}

	if(BA->mutex_take(*BA->mutex_twi_bricklet, 10)) {
		BA->bricklet_select(BS->port - 'a');

		const char last_digit[4] = {'2', '1', '0', '7'};
		char *str = "$PMTK101*30\r\n";
		str[8] += data->restart_type; // FIXME: is this safe, as in does this do a copy?
		str[10] = last_digit[data->restart_type];

		while(*str != '\0') {
			const uint8_t lsr = sc16is740_read_register(I2C_INTERNAL_ADDRESS_LSR);
			if(lsr & SC16IS740_LSR_THR_IS_EMPTY) {
				sc16is740_write_register(I2C_INTERNAL_ADDRESS_THR, *str);
				str++;
			}
		}

		BA->bricklet_deselect(BS->port - 'a');
		BA->mutex_give(*BA->mutex_twi_bricklet);
	}
}

void set_coordinates_callback_period(const ComType com, const SetCoordinatesCallbackPeriod *data) {
	BC->period_coordinates = data->period;

	BA->com_return_setter(com, data);
}

void get_coordinates_callback_period(const ComType com, const GetCoordinatesCallbackPeriod *data) {
	GetCoordinatesCallbackPeriodReturn gccpr;

	gccpr.header         = data->header;
	gccpr.header.length  = sizeof(GetCoordinatesCallbackPeriodReturn);
	gccpr.period         = BC->period_coordinates;

	BA->send_blocking_with_timeout(&gccpr,
	                               sizeof(GetCoordinatesCallbackPeriodReturn),
	                               com);
}

void set_status_callback_period(const ComType com, const SetStatusCallbackPeriod *data) {
	BC->period_status = data->period;

	BA->com_return_setter(com, data);
}

void get_status_callback_period(const ComType com, const GetStatusCallbackPeriod *data) {
	GetStatusCallbackPeriodReturn gscpr;

	gscpr.header         = data->header;
	gscpr.header.length  = sizeof(GetStatusCallbackPeriodReturn);
	gscpr.period         = BC->period_status;

	BA->send_blocking_with_timeout(&gscpr,
	                               sizeof(GetStatusCallbackPeriodReturn),
	                               com);
}

void set_altitude_callback_period(const ComType com, const SetAltitudeCallbackPeriod *data) {
	BC->period_altitude = data->period;

	BA->com_return_setter(com, data);
}

void get_altitude_callback_period(const ComType com, const GetAltitudeCallbackPeriod *data) {
	GetAltitudeCallbackPeriodReturn gacpr;

	gacpr.header         = data->header;
	gacpr.header.length  = sizeof(GetAltitudeCallbackPeriodReturn);
	gacpr.period         = BC->period_altitude;

	BA->send_blocking_with_timeout(&gacpr,
	                               sizeof(GetAltitudeCallbackPeriodReturn),
	                               com);
}

void set_motion_callback_period(const ComType com, const SetMotionCallbackPeriod *data) {
	BC->period_motion = data->period;

	BA->com_return_setter(com, data);
}

void get_motion_callback_period(const ComType com, const GetMotionCallbackPeriod *data) {
	GetMotionCallbackPeriodReturn gmcpr;

	gmcpr.header         = data->header;
	gmcpr.header.length  = sizeof(GetMotionCallbackPeriodReturn);
	gmcpr.period         = BC->period_motion;

	BA->send_blocking_with_timeout(&gmcpr,
	                               sizeof(GetMotionCallbackPeriodReturn),
	                               com);
}

void set_date_time_callback_period(const ComType com, const SetDateTimeCallbackPeriod *data) {
	BC->period_date_time = data->period;

	BA->com_return_setter(com, data);
}

void get_date_time_callback_period(const ComType com, const GetDateTimeCallbackPeriod *data) {
	GetDateTimeCallbackPeriodReturn gdtcpr;

	gdtcpr.header         = data->header;
	gdtcpr.header.length  = sizeof(GetDateTimeCallbackPeriodReturn);
	gdtcpr.period         = BC->period_date_time;

	BA->send_blocking_with_timeout(&gdtcpr,
	                               sizeof(GetDateTimeCallbackPeriodReturn),
	                               com);
}

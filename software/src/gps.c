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
#include "bricklib/drivers/adc/adc.h"
#include "config.h"

#define MAX_ADC_VALUE ((1 << 12) - 1)
#define MAX_VOLTAGE 3300

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

bool parse_packed_sentence(void) {
	// Preamble and end
	if(BC->packed_sentence.preamble[0] != 0x04 ||
	   BC->packed_sentence.preamble[1] != '$' ||
	   BC->packed_sentence.asterisk != '*' ||
	   BC->packed_sentence.end[0] != '\r' ||
	   BC->packed_sentence.end[1] != '\n') {
		return false;
	}

	// Checksum
	uint8_t xor = 0;
	uint8_t *p = ((uint8_t *)&BC->packed_sentence) + offsetof(PackedBinarySentence, time);
	uint8_t *e = ((uint8_t *)&BC->packed_sentence) + offsetof(PackedBinarySentence, asterisk);

	while(p < e) {
		xor ^= *p++;
	}

	if(xor != BC->packed_sentence.checksum) {
		return false;
	}

	// Unpack to avoid packed access and convert big to little endian
	BC->unpacked_sentence.time               = swap_uint32(BC->packed_sentence.time);
	BC->unpacked_sentence.date               = swap_uint32(BC->packed_sentence.date);
	BC->unpacked_sentence.latitude           = swap_uint32(BC->packed_sentence.latitude);
	BC->unpacked_sentence.ns                 = BC->packed_sentence.ns;
	BC->unpacked_sentence.longitude          = swap_uint32(BC->packed_sentence.longitude);
	BC->unpacked_sentence.ew                 = BC->packed_sentence.ew;
	BC->unpacked_sentence.fix_type           = BC->packed_sentence.fix_type;
	BC->unpacked_sentence.fix_mode           = BC->packed_sentence.fix_mode;
	BC->unpacked_sentence.altitude           = swap_uint32(BC->packed_sentence.altitude);
	BC->unpacked_sentence.geoidal_separation = swap_uint32(BC->packed_sentence.geoidal_separation);
	BC->unpacked_sentence.course             = swap_uint32(BC->packed_sentence.course);
	BC->unpacked_sentence.speed              = swap_uint32(BC->packed_sentence.speed);
	BC->unpacked_sentence.satellites_view    = BC->packed_sentence.satellites_view;
	BC->unpacked_sentence.satellites_used    = BC->packed_sentence.satellites_used;
	BC->unpacked_sentence.pdop               = swap_uint16(BC->packed_sentence.pdop);
	BC->unpacked_sentence.hdop               = swap_uint16(BC->packed_sentence.hdop);
	BC->unpacked_sentence.vdop               = swap_uint16(BC->packed_sentence.vdop);
	BC->unpacked_sentence.epe                = swap_uint16(BC->packed_sentence.epe);

	// Validate
	if(BC->unpacked_sentence.ns != 1 && BC->unpacked_sentence.ns != 2) {
		return false;
	}

	if(BC->unpacked_sentence.ew != 1 && BC->unpacked_sentence.ew != 2) {
		return false;
	}

	if(BC->unpacked_sentence.fix_type != 1 &&
	   BC->unpacked_sentence.fix_type != 2 &&
	   BC->unpacked_sentence.fix_type != 3) {
		return false;
	}

	if(BC->unpacked_sentence.fix_mode != 0 &&
	   BC->unpacked_sentence.fix_mode != 1 &&
	   BC->unpacked_sentence.fix_mode != 2) {
		return false;
	}

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

	BC->counter = 0;

	adc_channel_enable(BS->adc_channel);

	BC->voltage_avg_sum = 0;
	BC->voltage_avg = 0;
	BC->voltage_tick = 0;

	mt3339_disable();
	sc16is740_reset();

	BC->in_sync = false;
	BC->packed_sentence_used = 0;

	uint8_t *p = (uint8_t *)&BC->unpacked_sentence;
	uint8_t *e = p + sizeof(UnpackedBinarySentence);

	while(p < e) {
		*p++ = 0;
	}

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
	//adc_channel_disable(BS->adc_channel); // FIXME: save some byte plugin size
}

void tick(const uint8_t tick_type) {
	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
		if(BC->counter != 0 && BC->counter != 255) {
			BC->counter--;

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
					if(BC->in_sync) {
						// read and parse data
						uint8_t *p = (uint8_t *)&BC->packed_sentence + BC->packed_sentence_used;

						*p = sc16is740_read_register(I2C_INTERNAL_ADDRESS_RHR);
						BC->packed_sentence_used += 1;

						if(BC->packed_sentence_used == sizeof(PackedBinarySentence)) {
							BC->packed_sentence_used = 0;

							bool result = parse_packed_sentence();

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
						// drop data
						sc16is740_read_register(I2C_INTERNAL_ADDRESS_RHR);
					}
				} else if(!BC->in_sync) {
					BC->in_sync = true;
					BC->packed_sentence_used = 0;
				}

				BA->bricklet_deselect(BS->port - 'a');
				BA->mutex_give(*BA->mutex_twi_bricklet);
			}
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

		if(BC->counter == 0) {
			// wait a tick between I2C communication and ADC readout
			BC->counter = 255;
		} else if(BC->counter == 255) {
			// update battery voltage
			BC->counter = sizeof(PackedBinarySentence);

			BC->voltage_avg_sum += BA->adc_channel_get_data(BS->adc_channel);
			BC->voltage_tick = (BC->voltage_tick + 1) % VOLTAGE_AVERAGE;

			if(BC->voltage_tick % VOLTAGE_AVERAGE == 0) {
				BC->voltage_avg_sum += VOLTAGE_AVERAGE * (MAX_ADC_VALUE / MAX_VOLTAGE) / 2;

				if(BC->voltage_avg_sum > MAX_ADC_VALUE * VOLTAGE_AVERAGE) {
					BC->voltage_avg_sum = MAX_ADC_VALUE * VOLTAGE_AVERAGE;
				}

				BC->voltage_avg = SCALE(BC->voltage_avg_sum / VOLTAGE_AVERAGE,
				                        0, MAX_ADC_VALUE,
				                        0, MAX_VOLTAGE);
				BC->voltage_avg_sum = 0;
			}
		}
	}

	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		if(BC->period_coordinates != 0 &&
		   BC->period_coordinates_new &&
		   BC->period_coordinates_counter == 0) {
			Coordinates coordinates;
			BA->com_make_default_header(&coordinates, BS->uid, sizeof(Coordinates), FID_COORDINATES);

			coordinates.latitude  = BC->unpacked_sentence.latitude;
			coordinates.ns        = BC->unpacked_sentence.ns == 1 ? 'N' : 'S';
			coordinates.longitude = BC->unpacked_sentence.longitude;
			coordinates.ew        = BC->unpacked_sentence.ew == 1 ? 'E' : 'W';
			coordinates.pdop      = BC->unpacked_sentence.pdop;
			coordinates.hdop      = BC->unpacked_sentence.hdop;
			coordinates.vdop      = BC->unpacked_sentence.vdop;
			coordinates.epe       = BC->unpacked_sentence.epe;

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

			status.fix             = BC->unpacked_sentence.fix_type;
			status.satellites_view = BC->unpacked_sentence.satellites_view;
			status.satellites_used = BC->unpacked_sentence.satellites_used;

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

			altitude.altitude           = BC->unpacked_sentence.altitude;
			altitude.geoidal_separation = BC->unpacked_sentence.geoidal_separation;

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

			motion.course = BC->unpacked_sentence.course;
			motion.speed  = BC->unpacked_sentence.speed;

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

			date_time.date = BC->unpacked_sentence.date;
			date_time.time = BC->unpacked_sentence.time;

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
		case FID_GET_BATTERY_VOLTAGE:             get_battery_voltage(com, (GetBatteryVoltage*)data); break;
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
	gcr.latitude       = BC->unpacked_sentence.latitude;
	gcr.ns             = BC->unpacked_sentence.ns == 1 ? 'N' : 'S';
	gcr.longitude      = BC->unpacked_sentence.longitude;
	gcr.ew             = BC->unpacked_sentence.ew == 1 ? 'E' : 'W';
	gcr.pdop           = BC->unpacked_sentence.pdop;
	gcr.hdop           = BC->unpacked_sentence.hdop;
	gcr.vdop           = BC->unpacked_sentence.vdop;
	gcr.epe            = BC->unpacked_sentence.epe;

	BA->send_blocking_with_timeout(&gcr, sizeof(GetCoordinatesReturn), com);
}

void get_status(const ComType com, const GetStatus *data) {
	GetStatusReturn gsr;

	gsr.header             = data->header;
	gsr.header.length      = sizeof(GetStatusReturn);
	gsr.fix                = BC->unpacked_sentence.fix_type;
	gsr.satellites_view    = BC->unpacked_sentence.satellites_view;
	gsr.satellites_used    = BC->unpacked_sentence.satellites_used;

	BA->send_blocking_with_timeout(&gsr, sizeof(GetStatusReturn), com);
}

void get_altitude(const ComType com, const GetAltitude *data) {
	GetAltitudeReturn gar;

	gar.header             = data->header;
	gar.header.length      = sizeof(GetAltitudeReturn);
	gar.altitude           = BC->unpacked_sentence.altitude;
	gar.geoidal_separation = BC->unpacked_sentence.geoidal_separation;

	BA->send_blocking_with_timeout(&gar, sizeof(GetAltitudeReturn), com);
}

void get_motion(const ComType com, const GetMotion *data) {
	GetMotionReturn gmr;

	gmr.header             = data->header;
	gmr.header.length      = sizeof(GetMotionReturn);
	gmr.course             = BC->unpacked_sentence.course;
	gmr.speed              = BC->unpacked_sentence.speed;

	BA->send_blocking_with_timeout(&gmr, sizeof(GetMotionReturn), com);
}

void get_date_time(const ComType com, const GetDateTime *data) {
	GetDateTimeReturn gdtr;

	gdtr.header             = data->header;
	gdtr.header.length      = sizeof(GetDateTimeReturn);
	gdtr.date               = BC->unpacked_sentence.date;
	gdtr.time               = BC->unpacked_sentence.time;

	BA->send_blocking_with_timeout(&gdtr, sizeof(GetDateTimeReturn), com);
}

void get_battery_voltage(const ComType com, const GetBatteryVoltage *data) {
	GetBatteryVoltageReturn gbvr;

	gbvr.header             = data->header;
	gbvr.header.length      = sizeof(GetBatteryVoltageReturn);
	gbvr.voltage            = BC->voltage_avg;

	BA->send_blocking_with_timeout(&gbvr, sizeof(GetBatteryVoltageReturn), com);
}

void restart(const ComType com, const Restart *data) {
	if(data->restart_type > 3) {
		BA->com_return_error(data, sizeof(MessageHeader), MESSAGE_ERROR_CODE_INVALID_PARAMETER, com);
		return;
	}

	if(BA->mutex_take(*BA->mutex_twi_bricklet, 10)) {
		BA->bricklet_select(BS->port - 'a');

		const char last_digit[4] = {'2', '1', '0', '7'};
		char *str = "$PMTK10x*3y\r\n";

		while(*str != '\0') {
			const uint8_t lsr = sc16is740_read_register(I2C_INTERNAL_ADDRESS_LSR);
			if(lsr & SC16IS740_LSR_THR_IS_EMPTY) {
				char c = *str;
				if(c == 'x') {
					c = '1' + data->restart_type;
				} else if (c == 'y') {
					c = last_digit[data->restart_type];
				}

				sc16is740_write_register(I2C_INTERNAL_ADDRESS_THR, c);
				str++;
			}
		}

		BC->in_sync = false;
		BC->unpacked_sentence.fix_type = 1;

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

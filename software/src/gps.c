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

uint8_t sc16is750_get_address(void) {
	if(BS->address == I2C_EEPROM_ADDRESS_HIGH) {
		return I2C_ADDRESS_HIGH;
	} else {
		return I2C_ADDRESS_LOW;
	}
}

uint8_t sc16is750_read_register(uint8_t address) {
	uint8_t value = 0;
	BA->TWID_Read(BA->twid,
	              sc16is750_get_address(),
	              address << 3,
	              1,
	              &value,
	              1,
	              NULL);

	return value;
}

void sc16is750_write_register(uint8_t address, uint8_t value) {
	BA->TWID_Write(BA->twid,
	               sc16is750_get_address(),
	               address << 3,
	               1,
	               &value,
	               1,
	               NULL);
}

uint8_t hex_to_int(char hex) {
	if(hex >= '0' && hex <= '9') {
		return hex - '0';
	}

	if(hex >= 'A' && hex <= 'F') {
		return 10 + (hex - 'A');
	}

	return 255;
}

bool streq(const char *s1, const char *s2) {
	if(s1 == s2) {
		return true;
	}

	while(*s1 == *s2 && *s1 != '\0' && *s2 != '\0') {
		++s1;
		++s2;
	}

	return *s1 == *s2;
}

char *strtoi(char *s, int32_t *i) {
	int32_t sign = 1;

	if(*s == '-') {
		sign = -1;
		++s;
	} else if(*s == '+') {
		++s;
	}

	if(*s < '0' || *s > '9') {
		return NULL;
	}

	char *p = s;

	while(*(p + 1) >= '0' && *(p + 1) <= '9') {
		++p;
	}

	char *e = p + 1;
	int32_t base = 1;
	int32_t temp = 0;

	while(p >= s) {
		int32_t d = (*p - '0') * base;

		if(INT32_MAX - d < temp) {
			// overflow
			return NULL;
		}

		temp += d;
		base *= 10;
		--p;
	}

	*i = sign * temp;

	return e;
}

static uint8_t strsplit(char *s, uint8_t *fields, uint8_t count) {
	uint8_t i = 0;
	char *p = s;

	if(count == 0) {
		return 0;
	}

	fields[i++] = 0;

	while(*p != '\0' && i < count) {
		if(*p == ',') {
			*p = '\0';
			fields[i++] = p - s + 1;
		}

		++p;
	}

	return i;
}

int32_t parse_simple_int32(char *str) {
	int32_t value = 0;
	if(strtoi(str, &value) == 0) {
		return 0;
	}

	return value;
}

void parse_simple_int32dot(char *str, int32_t *value1, int32_t *value2) {
	char *e = strtoi(str, value1);
	if(e != NULL) {
		if(*e == '.') {
			*value2 = parse_simple_int32(e+1);
		} else {
			*value2 = 0;
		}
	} else {
		*value1 = 0;
		*value2 = 0;
	}
}

uint8_t parse_simple_uint8(char *str) {
	int32_t value = 1;
	if(strtoi(str, &value) == 0) {
		return 0;
	}

	if(value > UINT8_MAX || value < 0) {
		return 0;
	}

	return value;
}

bool parse_buffer(void) {
	if(BC->buffer[0] != '$' ||
	   BC->buffer[BC->buffer_used - 5] != '*') {
	   BC->buffer[BC->buffer_used - 2] = '\0';
		return false;
	}

	const uint8_t c1 = hex_to_int(BC->buffer[BC->buffer_used - 4]);
	const uint8_t c2 = hex_to_int(BC->buffer[BC->buffer_used - 3]);
	uint8_t xor = 0;

	if(c1 > 15 || c2 > 15) {
		return false;
	}
	
	for(uint8_t i = 1; i < BC->buffer_used - 5; i++) {
		xor ^= BC->buffer[i];
	}
	
	if(xor != c1 * 16 + c2) {
		return false;
	}

	//BC->buffer[BC->buffer_used - 2] = '\0';
	//BA->printf("%s\n\r", BC->buffer);

	BC->buffer[BC->buffer_used - 5] = '\0';

	uint8_t fields[30];
	char *p = BC->buffer + 1;
	uint8_t n = strsplit(p, fields, 30);
	
	if(n > 0) {
		if(streq(p, "GPGGA")) {
			BC->satellites_used = parse_simple_uint8(p + fields[7]);

			int32_t value1 = 0;
			int32_t value2 = 0;
			parse_simple_int32dot(p + fields[9], &value1, &value2);
			BC->altitude = value1*10 + value2;
			parse_simple_int32dot(p + fields[11], &value1, &value2);
			BC->geoidal_separation = value1*10 + value2;
			BC->period_status_new = true;
		} else if(streq(p, "GPGSA")) {
			BC->fix = parse_simple_uint8(p + fields[2]);
			int32_t value1 = 0;
			int32_t value2 = 0;
			parse_simple_int32dot(p +  fields[15], &value1, &value2);
			BC->pdop = value1*100 + value2;
			parse_simple_int32dot(p +  fields[16], &value1, &value2);
			BC->hdop = value1*100 + value2;
			parse_simple_int32dot(p +  fields[17], &value1, &value2);
			BC->vdop = value1*100 + value2;

			BC->period_status_new = true;
			BC->period_coordinates_new = true;
		} else if(streq(p, "GPRMC")) {
			
			const char status = p[fields[2]];
			if(status == 'A') {
				BC->time = parse_simple_int32(p + fields[1]);
				BC->date = parse_simple_int32(p + fields[9]);

				int32_t value1 = 0;
				int32_t value2 = 0;
				parse_simple_int32dot(p +  fields[3],
				                      &value1,
				                      &value2);
				BC->latitude = value1*10000 + value2;
				BC->ns = p[fields[4]];
				if(BC->ns != 'N' && BC->ns != 'S') {
					BC->ns = 'U';
				}

				parse_simple_int32dot(p +  fields[5],
				                      &value1,
				                      &value2);
				BC->longitude = value1*10000 + value2;
				BC->ew = p[fields[6]];
				if(BC->ew != 'E' && BC->ew != 'W') {
					BC->ew = 'U';
				}

			} else {
				BC->latitude = 0;
				BC->ns = 'U';
				BC->longitude = 0;
				BC->ew = 'U';
			}
			BC->period_coordinates_new = true;
		} else if(streq(p, "GPGSV")) {
			const uint8_t message_num = parse_simple_uint8(p+fields[2]);
			if(message_num == 1) {
				BC->satellites_view = parse_simple_uint8(p+fields[3]);
				BC->period_status_new = true;
			}
		} else if(streq(p, "GPVTG")) {
			int32_t value1 = 0;
			int32_t value2 = 0;
			parse_simple_int32dot(p +  fields[1], &value1, &value2);
			BC->course = value1*100 + value2;

			parse_simple_int32dot(p +  fields[7], &value1, &value2);
			BC->speed = value1*100 + value2;
			BC->period_status_new = true;
		} else if(streq(p, "PMTK010")) {
			if(!BC->is_configured_to_57600) {
				const char *str_57600baud = "$PMTK251,57600*2C\r\n";
				mt3392_write_str(str_57600baud);
				BC->is_configured_to_57600 = true;
			}
		}
	}
	
	return true;
}

void mt3392_write_str(const char *str) {
	while(*str != '\0') {
		const uint8_t lsr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_LSR);
		if(lsr & (1 << 5)) {
			sc16is750_write_register(I2C_INTERNAL_ADDRESS_THR, *str);
			str++;
		}
	}
}

void mt3329_disable(void) {
	BS->pin2_da.pio->PIO_CODR = BS->pin2_da.mask;
	SLEEP_MS(1);
}

void mt3329_enable(void) {
	BS->pin2_da.pio->PIO_SODR = BS->pin2_da.mask;
}

void sc16is750_reset(void) {
	BS->pin3_pwm.pio->PIO_CODR = BS->pin3_pwm.mask;
	SLEEP_MS(5);
	BS->pin3_pwm.pio->PIO_SODR = BS->pin3_pwm.mask;
	SLEEP_MS(500); // Wait for SC16IS750 initialization and clock stabilization
}

void sc16is750_init(uint8_t baud) {
	if(BA->mutex_take(*BA->mutex_twi_bricklet, 10)) {
		BA->bricklet_select(BS->port - 'a');

		// Configure UART
		uint8_t lcr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_LCR);

		lcr |= (1 << 1) | (1 << 0); // Word length 8 bit: LCR[1], LCR[0] = 1, 1
		lcr &= ~(1 << 2); // 1 stop bit: LCR[2] = 0
		lcr &= ~(1 << 3); // No parity: LCR[3] = 0
		sc16is750_write_register(I2C_INTERNAL_ADDRESS_LCR, lcr);

		// Configure baudrate: ((14.7456 MHz * 1000000) / 1) /
		//                     (9600 baud * 16) = 96
		lcr |= 1 << 7; // Divisor latch enable: LCR[7] = 1
		sc16is750_write_register(I2C_INTERNAL_ADDRESS_LCR, lcr);

		sc16is750_write_register(I2C_INTERNAL_ADDRESS_DLL, baud);
		sc16is750_write_register(I2C_INTERNAL_ADDRESS_DLH, 0);

		lcr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_LCR);
		lcr &= ~(1 << 7); // Divisor latch disable: LCR[7] = 0
		sc16is750_write_register(I2C_INTERNAL_ADDRESS_LCR, lcr);

		// Configure FIFOs
		uint8_t fcr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_FCR);

		fcr |= (1 << 0); // Enable FIFOs: FCR[0] = 1
		sc16is750_write_register(I2C_INTERNAL_ADDRESS_FCR, fcr);

		BA->bricklet_deselect(BS->port - 'a');
		BA->mutex_give(*BA->mutex_twi_bricklet);
	}
}

void constructor(void) {
	_Static_assert(sizeof(BrickContext) <= BRICKLET_CONTEXT_MAX_SIZE,
	               "BrickContext too big");

	BS->pin2_da.type = PIO_OUTPUT_1;
	BA->PIO_Configure(&BS->pin2_da, 1);

	BS->pin3_pwm.type = PIO_OUTPUT_1;
	BA->PIO_Configure(&BS->pin3_pwm, 1);

	BC->period_coordinates = 0;
	BC->period_coordinates_counter = 0;
	BC->period_coordinates_new = false;
	BC->period_status = 0;
	BC->period_status_counter = 0;
	BC->period_status_new = false;

	BC->buffer_used = 0;
	BC->timeout_counter = 5000;
	BC->is_configured_to_57600 = false;

	// GPS disable
	mt3329_disable();

	// Reset SC16IS750
	sc16is750_reset();

	sc16is750_init(96); // 9600 baud
	// GPS enable
	mt3329_enable();
}

void destructor(void) {

}

void tick(uint8_t tick_type) {
	bool reset = false;
	
	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
		if(BA->mutex_take(*BA->mutex_twi_bricklet, 10)) {
			BA->bricklet_select(BS->port - 'a');

			uint8_t lsr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_LSR);

			if((lsr & 0x1E) != 0) {
				reset = true;
			} else if(lsr & SC16IS750_LSR_DATA_IN_RECEIVER) {
				uint8_t rxlvl = sc16is750_read_register(I2C_INTERNAL_ADDRESS_RXLVL);

				for(uint8_t i = 0; i < rxlvl; i++) {
					uint8_t rhr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_RHR);

					if(BC->buffer_used == 0) {
						if(rhr == '$') {
							BC->buffer[BC->buffer_used++] = rhr;
						}
					} else if(BC->buffer_used < BUFFER_SIZE) {
						BC->buffer[BC->buffer_used++] = rhr;
					} else {
						BC->buffer_used = 0;
					}

					if(BC->buffer_used >= 6 &&
					    BC->buffer[BC->buffer_used - 2] == '\r' &&
					    BC->buffer[BC->buffer_used - 1] == '\n') {
						if(parse_buffer()) {
							BC->timeout_counter = 5000;
						}

						BC->buffer_used = 0;
					}
				}
			}

			BA->bricklet_deselect(BS->port - 'a');
			BA->mutex_give(*BA->mutex_twi_bricklet);
		}

		if(BC->timeout_counter != 0) {
			--BC->timeout_counter;
			
			if(BC->timeout_counter == 0) {
				reset = true;
			}
		}

		if(reset) {
			BC->timeout_counter = 5000;
			sc16is750_init(16); // 57600 baud
			const char *str_10hz = "$PMTK220,100*2F\r\n";
			mt3392_write_str(str_10hz);
			BC->is_configured_to_57600 = false;
		}

		if(BC->period_coordinates_counter > 0) {
			BC->period_coordinates_counter--;
		}
		if(BC->period_status_counter > 0) {
			BC->period_status_counter--;
		}
	}
	if(tick_type & TICK_TASK_TYPE_MESSAGE) {
		if(BC->period_coordinates != 0 &&
		   BC->period_coordinates_new &&
		   BC->period_coordinates_counter == 0) {
			Coordinates coordinates = {
				BS->stack_id,
				TYPE_COORDINATES,
				sizeof(Coordinates),
				BC->ns,
				BC->latitude,
				BC->ew,
				BC->longitude,
				BC->pdop,
				BC->hdop,
				BC->vdop
			};

			BA->send_blocking_with_timeout(&coordinates,
			                               sizeof(Coordinates),
			                               *BA->com_current);
			BC->period_coordinates_counter = BC->period_coordinates;
		}

		if(BC->period_status != 0 &&
		   BC->period_status_new &&
		   BC->period_status_counter == 0) {
			Status status = {
				BS->stack_id,
				TYPE_STATUS,
				sizeof(Status),
				BC->fix,
				BC->satellites_view,
				BC->satellites_used,
				BC->speed,
				BC->course,
				BC->date,
				BC->time,
				BC->altitude,
				BC->geoidal_separation
			};

			BA->send_blocking_with_timeout(&status,
			                               sizeof(Status),
			                               *BA->com_current);

			BC->period_status_counter = BC->period_status;
		}
	}
}

void invocation(uint8_t com, uint8_t *data) {
	switch(((StandardMessage*)data)->type) {
		case 1: get_coordinates(com, (GetCoordinates*)data); break;
		case 2: get_status(com, (GetStatus*)data); break;
		case 3: restart(com, (Restart*)data); break;
		case 4: set_coordinates_callback_period(com, (SetCoordinatesCallbackPeriod*)data); break;
		case 5: get_coordinates_callback_period(com, (GetCoordinatesCallbackPeriod*)data); break;
		case 6: set_status_callback_period(com, (SetStatusCallbackPeriod*)data); break;
		case 7: get_status_callback_period(com, (GetStatusCallbackPeriod*)data); break;
	}
}

void get_coordinates(uint8_t com, const GetCoordinates *data) {
	GetCoordinatesReturn gcr;

	gcr.stack_id       = data->stack_id;
	gcr.type           = data->type;
	gcr.length         = sizeof(GetCoordinatesReturn);
	gcr.ns             = BC->ns;
	gcr.latitude       = BC->latitude;
	gcr.ew             = BC->ns;
	gcr.longitude      = BC->longitude;
	gcr.pdop           = BC->pdop;
	gcr.hdop           = BC->hdop;
	gcr.vdop           = BC->vdop;

	BA->send_blocking_with_timeout(&gcr, sizeof(GetCoordinatesReturn), com);
}

void get_status(uint8_t com, const GetStatus *data) {
	GetStatusReturn gsr;

	gsr.stack_id           = data->stack_id;
	gsr.type               = data->type;
	gsr.length             = sizeof(GetStatusReturn);
	gsr.fix                = BC->fix;
	gsr.satellites_view    = BC->satellites_view;
	gsr.satellites_used    = BC->satellites_used;
	gsr.speed              = BC->speed;
	gsr.course             = BC->course;
	gsr.date               = BC->date;
	gsr.time               = BC->time;
	gsr.altitude           = BC->altitude;
	gsr.geoidal_separation = BC->geoidal_separation;

	BA->send_blocking_with_timeout(&gsr, sizeof(GetStatusReturn), com);
}

void restart(uint8_t com, const Restart *data) {
	if(data->restart_type > 3) {
		return;
	}

	if(BA->mutex_take(*BA->mutex_twi_bricklet, 10)) {
		BA->bricklet_select(BS->port - 'a');

		const char last_digit[4] = {'2', '1', '0', '7'};
		char *str = "$PMTK101*30\r\n";
		str[8] += data->restart_type;
		str[10] = last_digit[data->restart_type];

		while(*str != '\0') {
			const uint8_t lsr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_LSR);
			if(lsr & (1 << 5)) {
				sc16is750_write_register(I2C_INTERNAL_ADDRESS_THR, *str);
				str++;
			}
		}

		BA->bricklet_deselect(BS->port - 'a');
		BA->mutex_give(*BA->mutex_twi_bricklet);
	}
}

void set_coordinates_callback_period(uint8_t com, const SetCoordinatesCallbackPeriod *data) {
	BC->period_coordinates = data->period;
}

void get_coordinates_callback_period(uint8_t com, const GetCoordinatesCallbackPeriod *data) {
	GetCoordinatesCallbackPeriodReturn gccpr;

	gccpr.stack_id       = data->stack_id;
	gccpr.type           = data->type;
	gccpr.length         = sizeof(GetCoordinatesCallbackPeriodReturn);
	gccpr.period         = BC->period_coordinates;

	BA->send_blocking_with_timeout(&gccpr,
	                               sizeof(GetCoordinatesCallbackPeriodReturn),
	                               com);
}

void set_status_callback_period(uint8_t com, const SetStatusCallbackPeriod *data) {
	BC->period_status = data->period;
}

void get_status_callback_period(uint8_t com, const GetStatusCallbackPeriod *data) {
	GetStatusCallbackPeriodReturn gscpr;

	gscpr.stack_id       = data->stack_id;
	gscpr.type           = data->type;
	gscpr.length         = sizeof(GetStatusCallbackPeriodReturn);
	gscpr.period         = BC->period_status;

	BA->send_blocking_with_timeout(&gscpr,
	                               sizeof(GetStatusCallbackPeriodReturn),
	                               com);
}

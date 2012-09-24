/* gps-bricklet
 * Copyright (C) 2012 Matthias Bolte <matthias@tinkerforge.com>
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

	BA->TWID_Read(BA->twid, sc16is750_get_address(), address << 3, 1, &value, 1, NULL);

	return value;
}

void sc16is750_write_register(uint8_t address, uint8_t value) {
	BA->TWID_Write(BA->twid, sc16is750_get_address(), address << 3, 1, &value, 1, NULL);
}

uint8_t hex_to_int(char hex) {
	if (hex >= '0' && hex <= '9') {
		return hex - '0';
	}

	if (hex >= 'A' && hex <= 'F') {
		return 10 + (hex - 'A');
	}

	return 255;
}

bool streq(const char *s1, const char *s2) {
	if (s1 == s2) {
		return true;
	}

	while (*s1 == *s2 && *s1 != '\0' && *s2 != '\0') {
		++s1;
		++s2;
	}

	return *s1 == *s2;
}

char *strtoi(char *s, int32_t *i)
{
	int32_t sign = 1;

	if (*s == '-') {
		sign = -1;
		++s;
	} else if (*s == '+') {
		++s;
	}

	if (*s < '0' || *s > '9') {
		return NULL;
	}

	char *p = s;

	while (*(p + 1) >= '0' && *(p + 1) <= '9') {
		++p;
	}

	char *e = p + 1;
	int32_t base = 1;
	int32_t temp = 0;

	while (p >= s) {
		int32_t d = (*p - '0') * base;

		if (INT32_MAX - d < temp) {
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

static uint8_t strsplit(char *s, uint8_t *fields, uint8_t count)
{
	uint8_t i = 0;
	char *p = s;

	if (count == 0) {
		return 0;
	}

	fields[i++] = 0;

	while (*p != '\0' && i < count) {
		if (*p == ',') {
			*p = '\0';
			fields[i++] = p - s + 1;
		}

		++p;
	}

	return i;
}

bool parse_buffer(void) {
	if (BC->buffer[0] != '$' ||
		BC->buffer[BC->buffer_used - 5] != '*') {
		BC->buffer[BC->buffer_used - 2] = '\0';
		BA->printf("IB '%s'\n\r", BC->buffer);
		return false;
	}

	uint8_t c1 = hex_to_int(BC->buffer[BC->buffer_used - 4]);
	uint8_t c2 = hex_to_int(BC->buffer[BC->buffer_used - 3]);
	uint8_t xor = 0;

	if (c1 > 15 || c2 > 15) {
		BC->buffer[BC->buffer_used - 2] = '\0';
		BA->printf("ICF '%s'\n\r", BC->buffer);
		return false;
	}
	
	for (uint8_t i = 1; i < BC->buffer_used - 5; i++) {
		xor ^= BC->buffer[i];
	}
	
	if (xor != c1 * 16 + c2) {
		BC->buffer[BC->buffer_used - 2] = '\0';
		BA->printf("CM %d != %d (%d, %d) '%s'\n\r", xor, c1 * 16 + c2, c1, c2, BC->buffer);
		return false;
	}

	BC->buffer[BC->buffer_used - 2] = '\0';
	BA->printf("BUFFER '%s'\n\r", BC->buffer);
	
	
	if (streq(BC->buffer, "$PMTK011,MTKGPS*08")) {
		/*if(BA->mutex_take(*BA->mutex_twi_bricklet, 10)) {
			BA->bricklet_select(BS->port - 'a');*/
			
			const char *p = "$PMTK605*31\r\n";
		
			while (*p != '\0') {
				uint8_t lsr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_LSR);
			
				if (lsr & (1 << 5)) {
			
					sc16is750_write_register(I2C_INTERNAL_ADDRESS_THR, *p);
					
					++p;
				}
			}
			
			BA->printf("PMTK_Q_RELEASE send\r\n");



		/*	BA->bricklet_deselect(BS->port - 'a');
			BA->mutex_give(*BA->mutex_twi_bricklet);
		}*/
	}
	
	
	BC->buffer[BC->buffer_used - 5] = '\0';
	
	
	uint8_t fields[30];
	char *p = BC->buffer + 1;
	uint8_t n = strsplit(p, fields, 30);
	
	if (n > 0) {
		if (streq(p, "GPGGA")) {
			int32_t utc = 0, msec = 0;
			char *e = strtoi(p + fields[1], &utc);
			
			if (*e == '.') {
				strtoi(e + 1, &msec); // FIXME: error handling
			}
			
			BA->printf("UTC: %d.%d\r\n", utc, msec);
			
			int32_t fix = 0;
			strtoi(p + fields[6], &fix); // FIXME: error handling
			BA->printf("fix: %d\r\n", fix);
			
			for (uint8_t i = 0; i < n; ++i) {
				BA->printf("F%d: %u, '%s'\r\n", i, fields[i], p + fields[i]);
			}
		}
	}

	/*split_buffer()

	if (streq(p, "GPGSA")) {
		p += 5;

		if (*p != ',') return false;
		++p;

		if (*p != 'A' && *p != 'M') return false;
		++p;

		if (*p != ',') return false;
		++p;
	}*/
	
	return true;
}

void invocation(uint8_t com, uint8_t *data) {
	
}


static void mt3329_disable(void) {
	BS->pin2_da.type = PIO_OUTPUT_0;
	BA->PIO_Configure(&BS->pin2_da, 1);
	SLEEP_MS(1);
}

static void mt3329_enable(void) {
	BS->pin2_da.type = PIO_OUTPUT_1;
	BA->PIO_Configure(&BS->pin2_da, 1);
}

static void sc16is750_reset(void) {
	BS->pin3_pwm.type = PIO_OUTPUT_0;
	BA->PIO_Configure(&BS->pin3_pwm, 1);
	SLEEP_MS(5);
	BS->pin3_pwm.type = PIO_OUTPUT_1;
	BA->PIO_Configure(&BS->pin3_pwm, 1);
	SLEEP_MS(500); // Wait for SC16IS750 initialization and clock stabilization
}

static void sc16is750_init(void) {
	if(BA->mutex_take(*BA->mutex_twi_bricklet, 10)) {
		BA->bricklet_select(BS->port - 'a');

		// Configure UART
		uint8_t lcr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_LCR);

		lcr |= (1 << 1) | (1 << 0); // Word length 8 bit: LCR[1], LCR[0] = 1, 1
		lcr &= ~(1 << 2); // 1 stop bit: LCR[2] = 0
		lcr &= ~(1 << 3); // No parity: LCR[3] = 0

		sc16is750_write_register(I2C_INTERNAL_ADDRESS_LCR, lcr);

		// Configure baudrate: ((14.7456 MHz * 1000000) / 1) / (9600 baud * 16) = 96
		lcr |= 1 << 7; // Divisor latch enable: LCR[7] = 1

		sc16is750_write_register(I2C_INTERNAL_ADDRESS_LCR, lcr);

		lcr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_LCR);

		sc16is750_write_register(I2C_INTERNAL_ADDRESS_DLL, 96);
		sc16is750_write_register(I2C_INTERNAL_ADDRESS_DLH, 0);

		lcr &= ~(1 << 7); // Divisor latch disable: LCR[7] = 0

		sc16is750_write_register(I2C_INTERNAL_ADDRESS_LCR, lcr);

		// Configure FIFOs
		uint8_t fcr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_FCR);

		fcr |= (1 << 0); // Enable FIFOs: FCR[0] = 1

		sc16is750_write_register(I2C_INTERNAL_ADDRESS_FCR, fcr);

		fcr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_FCR);
		BA->printf("FCR %d\n\r", fcr);

#if 0
		fcr &= ~((1 << 7) | (1 << 6)); // Set RX trigger level to 8 chars: FCR[7], FCR[6] = 0, 0

		sc16is750_write_register(I2C_INTERNAL_ADDRESS_FCR, fcr);

		fcr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_FCR);
		BA->printf("FCR %d\n\r", fcr);

		// Configure interrupts
		uint8_t ier = sc16is750_read_register(I2C_INTERNAL_ADDRESS_IER);

		ier |= (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0); // Enable all interrupts: IER[3] .. IER[0] = 1 .. 1

		sc16is750_write_register(I2C_INTERNAL_ADDRESS_IER, ier);
#endif

		BA->bricklet_deselect(BS->port - 'a');
		BA->mutex_give(*BA->mutex_twi_bricklet);
	}
}

void constructor(void) {
	BC->buffer_used = 0;
	BC->timeout_counter = 5000;

	// GPS disable
	mt3329_disable();

	// Reset SC16IS750
	sc16is750_reset();

	BC->last_irq_value = BS->pin4_io.pio->PIO_PDSR & BS->pin4_io.mask;

	sc16is750_init();

	// GPS enable
	mt3329_enable();
}

void destructor(void) {

}

void tick(uint8_t tick_type) {
	bool reset = false;
	
	if(tick_type & TICK_TASK_TYPE_CALCULATION) {
#if 0
		uint32_t irq_value = BS->pin4_io.pio->PIO_PDSR & BS->pin4_io.mask;

		if(irq_value != BC->last_irq_value) {
			if(irq_value < BC->last_irq_value &&
			   BA->mutex_take(*BA->mutex_twi_bricklet, 10)) {
				BA->bricklet_select(BS->port - 'a');

				uint8_t iir = sc16is750_read_register(I2C_INTERNAL_ADDRESS_IIR);

				BA->printf("IIR %d\n\r", iir);

				//if ((iir & 0x3F) == 0x06) {
					uint8_t lsr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_LSR);

					BA->printf("LSR %d\n\r", lsr);
				//}
				
				if (lsr & (1 << 1)) {
					BA->printf("RX overrun\n\r");
				}

				if (lsr & SC16IS750_LSR_DATA_IN_RECEIVER) {
					uint8_t rxlvl = sc16is750_read_register(I2C_INTERNAL_ADDRESS_RXLVL);

					BA->printf("RXLVL %d\n\r", rxlvl);

					for (uint8_t i = 0; i < rxlvl; i++) {
						uint8_t rhr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_RHR);

						BA->printf("RHR %d, '%c'\n\r", rhr, rhr);
					}
				}
				
				
				uint8_t ier = sc16is750_read_register(I2C_INTERNAL_ADDRESS_IER);
				uint8_t fcr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_FCR);
				BA->printf("IER %d, FCR %d\n\r", ier, fcr);

				BA->bricklet_deselect(BS->port - 'a');
				BA->mutex_give(*BA->mutex_twi_bricklet);
			}

			BC->last_irq_value = irq_value;
		}
#endif
		//BA->printf("T1\n\r");

		if(BA->mutex_take(*BA->mutex_twi_bricklet, 10)) {
			BA->bricklet_select(BS->port - 'a');

			uint8_t lsr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_LSR);

			if ((lsr & 0x9E) != 0) {
				BA->printf("LSR %d\n\r", lsr);
				reset = true;
			} else if (lsr & SC16IS750_LSR_DATA_IN_RECEIVER) {
				uint8_t rxlvl = sc16is750_read_register(I2C_INTERNAL_ADDRESS_RXLVL);

				if (rxlvl > 30) {
					BA->printf("RXLVL %d\n\r", rxlvl);
				}

				for (uint8_t i = 0; i < rxlvl; i++) {
					uint8_t rhr = sc16is750_read_register(I2C_INTERNAL_ADDRESS_RHR);

					/*if (rhr == '\n') {
						BA->printf("RHR %d, '\\n'\n\r", rhr);
					} else if (rhr == '\r') {
						BA->printf("RHR %d, '\\r'\n\r", rhr);
					} else {
						BA->printf("RHR %d, '%c'\n\r", rhr, rhr);
					}*/

					if (BC->buffer_used == 0) {
						if (rhr == '$') {
							BC->buffer[BC->buffer_used++] = rhr;
						} else {
							if (rhr == '\n') {
								BA->printf("RHR %d, '\\n' ignored\n\r", rhr);
							} else if (rhr == '\r') {
								BA->printf("RHR %d, '\\r' ignored\n\r", rhr);
							} else {
								BA->printf("RHR %d, '%c' ignored\n\r", rhr, rhr);
							}
						}
					} else if (BC->buffer_used < BUFFER_SIZE) {
						BC->buffer[BC->buffer_used++] = rhr;
					} else {
						BA->printf("BUFFER OVERFLOW\n\r");
						BC->buffer_used = 0;
					}

					if (BC->buffer_used >= 6 &&
					    BC->buffer[BC->buffer_used - 2] == '\r' &&
					    BC->buffer[BC->buffer_used - 1] == '\n') {
						if (parse_buffer()) {
							BC->timeout_counter = 5000;
						}

						BC->buffer_used = 0;
					}
				}
			}

			BA->bricklet_deselect(BS->port - 'a');
			BA->mutex_give(*BA->mutex_twi_bricklet);
		}
		//BA->printf("T2\n\r");

		if (BC->timeout_counter != 0) {
			--BC->timeout_counter;
			
			if (BC->timeout_counter == 0) {
				BA->printf("TIMEOUT\n\r");
				reset = true;
			}
		}
		//BA->printf("T3\n\r");

		if (reset) {
			BA->printf("RESET\n\r");
			BC->buffer_used = 0;

			// GPS disable
			mt3329_disable();

			// Reset SC16IS750
			sc16is750_reset();

			BC->last_irq_value = BS->pin4_io.pio->PIO_PDSR & BS->pin4_io.mask;

			sc16is750_init();

			// GPS enable
			mt3329_enable();
		}
		//BA->printf("T4\n\r");
	}
}

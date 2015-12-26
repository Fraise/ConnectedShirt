#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#include "serial.h"
#include "analog.h"
#include "slip.h"
#include "ethernet.h"
#include "broadcast.h"


static unsigned char led_freq = 100; // 0: static, 1: blinking


void task_send_samples(void) { /* LED PIND7: Green */
	unsigned char input[DATA_LENGTH], old_input[DATA_LENGTH], tmp;
	unsigned char packet[29 + DATA_LENGTH];

	int i, samples = 0, diff;

	_delay_cs(50); /* We get first values for initialization */
	for (i = 0; i < 3; i++) {
		ad_init(i);
		old_input[i] = ad_sample();
	}

	while (1) {
		diff = 0;
		for (i = 0; i < 3; i++) { /* Calculates squared distance of acceleration difference */
			ad_init(i);
			input[i] = ad_sample();
			tmp = (input[i] < old_input[i]) ? (old_input[i] - input[i]) : (input[i] - old_input[i]);
			diff += ((int)tmp * (int)tmp);
		}

		if (samples > 30) { /* Sending data every X seconds */
			samples = 0;
			ad_init(3);
			input[3] = ad_sample();
			build_packet(input, packet);
			PORTD ^= 0x80;
			slip_send_packet(packet, 29 + DATA_LENGTH);
			PORTD ^= 0x80;
		} else if (diff > 550) {
			for (i = 0; i < 4; i++) input[i] = 0xFF;
			build_packet(input, packet);
			PORTD ^= 0x80;
			slip_send_packet(packet, 29 + DATA_LENGTH);
			PORTD ^= 0x80;
		}

		for (i = 0; i < 3; i++) old_input[i] = input[i]; /* Saving old data */
		samples++;
		_delay_ms(30);
	}
}

void task_get_packet(void) { /* LED PIND6: Blue */
	unsigned char packet[29 + DATA_LENGTH], new_id, old_id, freq;

	while (1) {
		slip_recv_packet(packet, 29 + DATA_LENGTH);
		old_id = (packet[28] & 0xF0) >> 4;
		new_id = (packet[29] & 0x0F);
		freq = packet[32];
		if (old_id == get_id()) {
			if (freq == 0) set_id(new_id);
			else led_freq = freq;
		}
		PORTD ^= 0x40;
		_delay_ms(40);
		PORTD ^= 0x40;
	}
}

void task_blink_led(void) { /* LED PIND5: Red */
	while (1) {
		PORTD ^= 0x20;
		_delay_cs(led_freq);
	}
}

void init_led(void) {
	/* We use port D pins 5-6-7 aka connections 5-6-7 */
	DDRD = 0xE0;
	PORTD = 0xE0;
}

void _delay_cs(int t) {
	int i;
	for (i = 0; i < t; i++) _delay_ms(10);
}

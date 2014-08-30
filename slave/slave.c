#define PIN_RED 2
#define PIN_BLUE 0
#define PIN_GREEN 4
#define PIN_STATUS 6
#define F_CPU 3686400
#define BAUD 9600

#include <avr/io.h>
#include <util/delay.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>
#include "proto.h"

#define bit_set(byte, bit) do{ byte |= (uint8_t)(1 << bit); }while(0)
#define bit_clear(byte, bit) do{ byte &= (uint8_t)~(1 << bit); }while(0)

static uint8_t pwm_counter;
static uint8_t pwm_red = 0x10;
static uint8_t pwm_blue = 0x10;
static uint8_t pwm_green = 0x10;

static uint8_t recv_counter = 0;

ISR(TIMER0_COMPA_vect)
{
	if (pwm_counter < pwm_red)
		bit_set(PORTD, PIN_RED);
	else
		bit_clear(PORTD, PIN_RED);

	if (pwm_counter < pwm_blue)
		bit_set(PORTD, PIN_BLUE);
	else
		bit_clear(PORTD, PIN_BLUE);

	if (pwm_counter < pwm_green)
		bit_set(PORTD, PIN_GREEN);
	else
		bit_clear(PORTD, PIN_GREEN);

	pwm_counter++;
}

static void handle_byte(uint8_t byte) {
	PORTD ^= 1 << PIN_STATUS;

	switch (recv_counter) {
		case 0:
			pwm_red = byte;
			break;
		case 1:
			pwm_green = byte;
			break;
		case 2:
			pwm_blue = byte;
			break;
	}

	if (++recv_counter > 2)
		recv_counter = 0;
}

ISR(USART_RX_vect) {
	uint8_t byte = UDR;
	handle_escaping(byte, handle_byte);
}

int main(void) {
	// Configure output pins
	DDRD = (1 << PIN_STATUS) | (1 << PIN_RED) | (1 << PIN_BLUE) | (1 << PIN_GREEN);

	// Set up timer for PWM
	TCCR0A = (1<<WGM01); // ctc mode
	TCCR0B = (1<<CS00); // no prescaler
	OCR0A = 240; // set interval (200Hz)
	TIMSK = (1<<OCIE0A); // enable interrupts
	TIFR = (1<<OCF0A); // clear pending interrupts

	// Set Baudrate
	// see: http://www.nongnu.org/avr-libc/user-manual/group__util__setbaud.html
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
#if USE_2X
	UCSRA |= (1 << U2X);
#else
	UCSRA &= ~(1 << U2X);
#endif

	// Configure UART
	UCSRB = (1 << RXEN) | (1 << RXCIE);

	// Enable Interrupts
	sei();

	// Stupid main loop
	// This basically tells us if we need a lot of time in one of the ISRs
	// (probably useless...)
	for (;;) {
		//bit_set(PORTD, PIN_STATUS);
		//_delay_ms(100);
		//bit_clear(PORTD, PIN_STATUS);
		//_delay_ms(100);
	}
}

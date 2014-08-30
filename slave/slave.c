#define DDR_RGB DDRB
#define PORT_RGB PORTB
#define PIN_RED 2
#define PIN_BLUE 0
#define PIN_GREEN 4
#define DDR_STATUS DDRD
#define PORT_STATUS PORTD
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
#define bit_flip(byte, bit) do{ byte ^= (uint8_t)(1 << bit); }while(0)

static volatile uint8_t pwm_counter;
static volatile uint8_t pwm_red = 0xff;
static volatile uint8_t pwm_blue = 0x88;
static volatile uint8_t pwm_green = 0x00;

static uint8_t recv_counter = 0;

ISR(TIMER0_COMPA_vect)
{
	if (pwm_counter < pwm_red)
		bit_set(PORT_RGB, PIN_RED);
	else
		bit_clear(PORT_RGB, PIN_RED);

	if (pwm_counter < pwm_blue)
		bit_set(PORT_RGB, PIN_BLUE);
	else
		bit_clear(PORT_RGB, PIN_BLUE);

	if (pwm_counter < pwm_green)
		bit_set(PORT_RGB, PIN_GREEN);
	else
		bit_clear(PORT_RGB, PIN_GREEN);

	pwm_counter++;
}

static void handle_byte(uint8_t byte) {
	bit_flip(PORT_STATUS, PIN_STATUS);

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
	//uint8_t byte = UDR;
	//handle_escaping(byte, handle_byte);
	handle_byte(UDR);
}

int main(void) {
	// Configure output pins
	bit_set(DDR_STATUS, PIN_STATUS);
	bit_set(DDR_RGB, PIN_RED);
	bit_set(DDR_RGB, PIN_BLUE);
	bit_set(DDR_RGB, PIN_GREEN);

	// Set up timer for PWM
	TCCR0A = (1<<WGM01); // ctc mode
	TCCR0B = (1<<CS00); // no prescaler
	OCR0A = 240; // set interval (200Hz)
	TIMSK = (1<<OCIE0A); // enable interrupts
	TIFR = (1<<OCF0A); // clear pending interrupts

	/*
	// Set Baudrate
	// see: http://www.nongnu.org/avr-libc/user-manual/group__util__setbaud.html
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
#if USE_2X
	UCSRA |= (1 << U2X);
#else
	UCSRA &= ~(1 << U2X);
#endif
	*/

	int baud = F_CPU / (BAUD * 16L) - 1;
	UBRRH = baud >> 8;
	UBRRL = baud;


	// Configure UART
	UCSRB = (1 << RXEN) | (1 << RXCIE);

	// Enable Interrupts
	sei();

	while(1);
}

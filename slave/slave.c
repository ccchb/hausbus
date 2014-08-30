#define F_CPU 3686400
#define UART_BAUDRATE 9600
#include <avr/io.h>
#include <avr/interrupt.h>
#include "proto.h"

static void handle_byte(uint8_t byte) {
	(void) byte;
	// toggle the status led on any payload
	PORTD ^= 1 << 6;
}

ISR(USART_RX_vect) {
	uint8_t byte = UDR;
	handle_escaping(byte, handle_byte);
}

int main(void) {
	// status LED
	DDRD = 1 << 6;

	// pd0: rx
	//// set baudrate
	int baud = F_CPU / (UART_BAUDRATE * 16L) - 1;
	UBRRH = baud >> 8;
	UBRRL = baud;
	//// set interrupt
	UCSRB = (1 << RXEN) | (1 << RXCIE);
	sei();

	while(1) {
	}
}

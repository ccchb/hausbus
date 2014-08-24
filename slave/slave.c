#define F_CPU 3686400
#define UART_BAUDRATE 9600
#define PROTO_VERSION 1
#define PROTO_ADDR 0x01
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

static enum {
	STATE_ESCAPE_MESSAGE,
	STATE_ESCAPE_ESCAPE
} state_escape = STATE_ESCAPE_MESSAGE;

static enum {
	STATE_PROTO_ADDRESS,
	STATE_PROTO_IGNORE
} state_proto = STATE_PROTO_IGNORE;

static enum {
	STATE_ADDRESS_PAYLOAD,
	STATE_ADDRESS_IGNORE
} state_address = STATE_ADDRESS_IGNORE;

void handle_byte_payload(uint8_t byte) {

}

void handle_byte_address(uint8_t byte) {
	switch(state_address) {
		case STATE_ADDRESS_IGNORE:
			state_address = (byte == PROTO_ADDR) ? STATE_ADDRESS_PAYLOAD : STATE_ADDRESS_IGNORE;
			break;
		case STATE_ADDRESS_PAYLOAD:
			handle_byte_payload(byte);
			break;
	}
}

void handle_byte_proto(uint8_t byte) {
	switch(state_proto) {
		case STATE_PROTO_IGNORE:
			state_proto = (byte == PROTO_VERSION) ? STATE_PROTO_ADDRESS : STATE_PROTO_IGNORE;
			break;
		case STATE_PROTO_ADDRESS:
			handle_byte_address(byte);
			break;

	}
}

ISR(USART_RX_vect) {
	uint8_t val = UDR;

	// protocol: 1b proto | 1b addr | 1b eom
	// escaping: 0x00 0xff -> EOM, 0x00 0x00 -> 0x00
	switch(state_escape) {
		case STATE_ESCAPE_ESCAPE:
			switch(val) {
				case 0xff:
					// end of message
					state_proto = STATE_PROTO_IGNORE;
					break;
				case 0x00:
					handle_byte_proto(0x00);
					break;
			}
			state_escape = STATE_ESCAPE_MESSAGE;
			break;

		case STATE_ESCAPE_MESSAGE:
			switch(val) {
				case 0x00:
					state_escape = STATE_ESCAPE_ESCAPE;
					break;
				default:
					handle_byte_proto(val);
					break;
			}
			break;
	}
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

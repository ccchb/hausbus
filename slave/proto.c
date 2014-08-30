#include <stdint.h>
#include "proto.h"

/* Embassy One-Wire-Bus
 *
 *      1B         1B                      1B
 * ├──────────┼──────────┼─────────    ┼────────┤
 * ┏━━━━━━━━━━┱──────────┬─────────    ┲━━━━━━━━┓
 * ┃ PROTOCOL ┃  ADDRESS │ PAYLOAD     ┃  EOM   ┃
 * ┗━━━━━━━━━━┹──────────┴─────────    ┺━━━━━━━━┛
 * 
 * Only bytes with big lines are required by the protocol.
 */

static enum {
	STATE_ESCAPE_MESSAGE,
	STATE_ESCAPE_ESCAPE
} state_escape = STATE_ESCAPE_MESSAGE;

static enum {
	STATE_PROTO,
	STATE_ADDRESS,
	STATE_IGNORE
} state = STATE_IGNORE;

/* PROTOCOL and ADDRESS handling
 * 
 * PROTOCOL is used to differentiate between incompatible protocols on the bus
 * ADDRESS is used to talk to a specific device on the bus or broadcast using address 0x01
 */
static void handle_byte(uint8_t byte, proto_handle handle) {
	switch(state) {
		case STATE_IGNORE:
			state = (byte == PROTO_VERSION) ? STATE_PROTO : STATE_IGNORE;
			break;
		case STATE_PROTO:
			state = (byte == PROTO_ADDR || byte == PROTO_BROADCAST) ? STATE_ADDRESS : STATE_IGNORE;
			break;
		case STATE_ADDRESS:
			handle(byte);
			break;
	}
}

static void handle_EOM() {
	state = STATE_IGNORE;
}

/* Escaping
 *
 * 0x00 0x00 → 0x00
 * 0x00 0xff → EOM
 */
void handle_escaping(uint8_t byte, proto_handle handle) {
	switch(state_escape) {
		case STATE_ESCAPE_ESCAPE:
			switch(byte) {
				case 0xff:
					handle_EOM();
					break;
				case 0x00:
					handle_byte(0x00, handle);
					break;
			}
			state_escape = STATE_ESCAPE_MESSAGE;
			break;

		case STATE_ESCAPE_MESSAGE:
			switch(byte) {
				case 0x00:
					state_escape = STATE_ESCAPE_ESCAPE;
					break;
				default:
					handle_byte(byte, handle);
					break;
			}
			break;
	}
}
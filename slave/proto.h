#define PROTO_VERSION 0x01
#define PROTO_ADDR 0x02
#define PROTO_BROADCAST 0x01

typedef void (*proto_handle)(uint8_t byte);

void handle_escaping(uint8_t byte, proto_handle handle);
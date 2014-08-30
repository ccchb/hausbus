CC = avr-gcc
MCU = attiny2313
CFLAGS = -Wall -Wextra -g -Os -mmcu=$(MCU)

slave.elf: proto.o slave.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm *.o
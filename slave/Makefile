CC = avr-gcc
MCU = attiny2313
CFLAGS = -std=c11 -flto -Wall -Wextra -g -Os -mmcu=$(MCU)

default: slave.elf
	avr-size -C --mcu=$(MCU) slave.elf

slave.elf: proto.o slave.o
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm *.o slave.elf

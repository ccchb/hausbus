#!/bin/bash

CCOPTS="-Wall -Wextra -Werror"

avr-gcc $CCOPTS -std=c11 -Os -g -mmcu=attiny2313 -o "$1.elf" "$1.c"
sudo avrdude -p t2313 -c usbasp -U lfuse:w:0xfd:m -U hfuse:w:0xdf:m -U efuse:w:0xff:m -U "$1.elf"
#!/bin/bash

CCOPTS="-Wall -Wextra -Werror"

#avr-gcc $CCOPTS -std=c11 -Os -g -mmcu=attiny2313 -o "$1.elf" "$1.c"
avrdude -p t2313 -c usbasp -U "$1.elf"

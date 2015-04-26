# Name: Makefile
# Author: <insert your name here>
# Copyright: <insert your copyright message here>
# License: <insert your license reference here>

CC=gcc
CFLAGS=-Wall -std=gnu99 -O3
CPPFLAGS=$(EEPROM_DEFINES)
LDFLAGS=
OBJECTS=main.o eeprom.o
SOURCESS=$(OBJECTS:.o=.c)
EXECUTABLE=main

include eeprom.mk

all: $(SOURCES) $(EXECUTABLE)

clean:
	rm -rf $(EXECUTABLE) $(OBJECTS)

$(EXECUTABLE): $(OBJECTS)
	$(LINK.c) $(OBJECTS) -o $@ $(LDFLAGS)

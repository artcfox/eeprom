/*

  main.c

  Copyright 2015 Matthew T. Pandina. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:

  1. Redistributions of source code must retain the above copyright
  notice, this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY MATTHEW T. PANDINA "AS IS" AND ANY
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MATTHEW T. PANDINA OR
  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
  OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
  SUCH DAMAGE.

*/

#include <stdint.h>

uint8_t volume = 0x40;

struct __attribute__ ((__packed__)) settings_t {
  uint16_t score;
  uint8_t level;
};
struct settings_t settings = {0x00FD, 0x01};

// EEPROM parameter offsets (EE_EPROM_END should be defined before including eeprom.h)
#define EE_VOLUME     0
#define EE_SETTINGS   (EE_VOLUME + sizeof(volume) * EEPROM_WEAR_LEVEL_FACTOR * 2)
#define EE_EEPROM_END (EE_SETTINGS + sizeof(struct settings_t) * EEPROM_WEAR_LEVEL_FACTOR * 2)
#include "eeprom.h"

int main(void) {
  /*
    The initial state of the EEPROM:
    -----------------------------------------------
    FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
    FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
    FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
    FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
    ...
  */

  EEPROM_InitWearLeveledByte(EE_VOLUME, volume);
  /*
    The initial byte (0x40) is now stored, and the
    wear-leveling metadata for that byte has been
    initialized:
    -----------------------------------------------
    40 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
    FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
    FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
    ...
  */

  EEPROM_InitWearLeveledBlock(EE_SETTINGS, &settings, sizeof(settings));
  /*
    The contents of the struct settings_t
    ({0x00FD, 0x01}) have been stored, and each
    byte of data in the struct has had its wear-
    leveling metadata initialized:
    -----------------------------------------------
    40 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    FD FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    00 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    01 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    ...
  */

  volume = EEPROM_ReadWearLeveledByte(EE_VOLUME);
  EEPROM_ReadWearLeveledBlock(EE_SETTINGS, &settings, sizeof(settings));
  /*
    Reading wear-leveled bytes and/or blocks does
    not modify the contents of the EEPROM:
    -----------------------------------------------
    40 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    FD FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    00 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    01 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    ...
  */

  volume++;
  EEPROM_WriteWearLeveledByte(EE_VOLUME, volume);
  /*
    A new value (0x41) for the EE_VOLUME parameter
    has been written, and its metadata has been
    updated to reflect the location of this new
    value:
    -----------------------------------------------
    40 41 FF FF FF FF FF FF 07 08 01 02 03 04 05 06
    FD FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    00 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    01 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    ...
  */

  settings.score++;
  EEPROM_WriteWearLeveledBlock(EE_SETTINGS, &settings, sizeof(settings));
  /*
    The new contents of the EE_SETTINGS parameter
    ({0x00FE, 0x01}) have been stored, but since
    only a single byte of data in the structure
    actually changed, only that byte has been
    written to EEPROM. The metadata for the byte
    that changed has been updated to reflect the
    location of its new value:
    -----------------------------------------------
    40 41 FF FF FF FF FF FF 07 08 01 02 03 04 05 06
    FD FE FF FF FF FF FF FF 07 08 01 02 03 04 05 06
    00 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    01 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    ...
  */

  settings.score++;
  EEPROM_WriteWearLeveledBlock(EE_SETTINGS, &settings, sizeof(settings));
  /*
    The new contents of the EE_SETTINGS parameter
    ({0x00FF, 0x01}) have been stored, and only a
    single byte of data in the structure has
    changed (0xFF), but since that location in the
    EEPROM already contains 0xFF, only the metadata
    is updated to reflect the location of the new
    value:

    -----------------------------------------------
    40 41 FF FF FF FF FF FF 07 08 01 02 03 04 05 06
    FD FE FF FF FF FF FF FF 07 08 09 02 03 04 05 06
    00 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    01 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    ...
  */

  settings.score++;
  EEPROM_WriteWearLeveledBlock(EE_SETTINGS, &settings, sizeof(settings));
  /*
    The new contents of the EE_SETTINGS parameter
    ({0x0100, 0x01}) have been stored, and since
    two out of three bytes in the struct have
    changed, those two values (0x01) and (0x00)
    have been written to EEPROM. The metadata for
    those two bytes has also been updated to
    reflect the locations of each new value:
    -----------------------------------------------
    40 41 FF FF FF FF FF FF 07 08 01 02 03 04 05 06
    FD FE FF 00 FF FF FF FF 07 08 09 0A 03 04 05 06
    00 01 FF FF FF FF FF FF 07 08 01 02 03 04 05 06
    01 FF FF FF FF FF FF FF 07 00 01 02 03 04 05 06
    ...
  */

  return 0;
}

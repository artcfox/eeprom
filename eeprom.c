/*

  eeprom.c

  Based on AVR101 "High endurance EEPROM storage" by jllassen

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

#include "eeprom.h"

// Define the number of levels in the buffer (8 levels will guarantee 800k writes)
#define EE_PARAM_BUFFER_SIZE  EEPROM_WEAR_LEVEL_FACTOR
#define EE_STATUS_BUFFER_SIZE  EE_PARAM_BUFFER_SIZE

#ifdef F_CPU
#include <avr/eeprom.h>
#define EEPROM_Read(address) eeprom_read_byte((uint8_t *)(uint16_t)(address))
#define EEPROM_Write(address, data) eeprom_update_byte((uint8_t *)(uint16_t)(address), (data))
#else // F_CPU
// Only used for simulation when compiled on a computer
uint8_t eeprom[EEPROM_SIMULATED_SIZE] = { [0 ... EEPROM_SIMULATED_SIZE - 1] = 0xFF };
#define EEPROM_Read(address) eeprom[(address)]
#define EEPROM_Write(address, data) eeprom[(address)] = (data)
#include <stdio.h>
void EEPROM_Print(const uint16_t begin, const uint16_t end) {
  printf("-----------------------------------------------\n");
  for (uint16_t i = begin; i < end; ++i)
    printf("%02X %s", eeprom[i], (i + 1) % 16 ? "" : "\n");
  printf("-----------------------------------------------\n");
}
#endif // F_CPU

static uint16_t EEPROM_FindCurrentAddress(const uint16_t param) {
  uint16_t EeBufPtr = param + EE_PARAM_BUFFER_SIZE; // point to the status buffer
  uint16_t EeBufEnd = EeBufPtr + EE_STATUS_BUFFER_SIZE; // the first address outside the buffer

  // Identify the last written element of the status buffer
  uint8_t tmp;
  do {
    tmp = EEPROM_Read(EeBufPtr);
    if (++EeBufPtr == EeBufEnd) // avoid comparing out-of-bounds
      break;
  } while (EEPROM_Read(EeBufPtr) == (uint8_t)(tmp + 1));

  // Return the last used element of the param buffer
  return EeBufPtr - (EE_PARAM_BUFFER_SIZE + 1);
}

#if (EEPROM_INCLUDE_BYTE_FUNCS == 0)
static
#endif // EEPROM_INCLUDE_BYTE_FUNCS
uint8_t EEPROM_InitWearLeveledByte(const uint16_t param, const uint8_t data) {
  EEPROM_Write(param + EE_PARAM_BUFFER_SIZE, EE_STATUS_BUFFER_SIZE - 1);

  for (uint8_t i = 1; i < EE_STATUS_BUFFER_SIZE; ++i)
    EEPROM_Write(i + param + EE_PARAM_BUFFER_SIZE, i - 1);

  EEPROM_Write(param, data);
  return data;
}

#if (EEPROM_INCLUDE_BYTE_FUNCS == 0)
static
#endif // EEPROM_INCLUDE_BYTE_FUNCS
uint8_t EEPROM_ReadWearLeveledByte(const uint16_t param) {
  return EEPROM_Read(EEPROM_FindCurrentAddress(param));
}

#if (EEPROM_INCLUDE_BYTE_FUNCS == 0)
static
#endif // EEPROM_INCLUDE_BYTE_FUNCS
void EEPROM_WriteWearLeveledByte(const uint16_t param, const uint8_t data) {
  uint16_t address = EEPROM_FindCurrentAddress(param);

  // Only perform the write if the new value is different from what's currently stored
  if (EEPROM_Read(address) == data)
    return;

  // Store the old status value
  uint8_t oldStatusValue = EEPROM_Read(address + EE_PARAM_BUFFER_SIZE);

  // Move pointer to the next element in the buffer, wrapping around if necessary
  if (++address == param + EE_PARAM_BUFFER_SIZE)
    address = param;

  // If self-programming is used in the application, insert code here
  // to wait for any self-programming operations to finish before
  // writing to the EEPROM.

  // Update the param buffer in the EEPROM
  EEPROM_Write(address, data);

  // Update the status buffer in the EEPROM
  EEPROM_Write(address + EE_PARAM_BUFFER_SIZE, oldStatusValue + 1);
}

#if (EEPROM_INCLUDE_BLOCK_FUNCS)
void EEPROM_InitWearLeveledBlock(const uint16_t param, const void *data, const uint16_t len) {
  for (uint16_t i = 0; i < len; ++i)
    EEPROM_InitWearLeveledByte(param + i * (EE_PARAM_BUFFER_SIZE + EE_STATUS_BUFFER_SIZE),
                               *(((uint8_t *)data) + i));
}
/*
EEPROM_ReadWearLeveledBlock() is typically only called once per
parameter, usually when the device is first powered on, to retrieve
the latest stored value. */
void EEPROM_ReadWearLeveledBlock(const uint16_t param, void *data, const uint16_t len) {
  for (uint16_t i = 0; i < len; ++i)
    *(((uint8_t *)data) + i) = EEPROM_ReadWearLeveledByte(param + i * (EE_PARAM_BUFFER_SIZE
								       + EE_STATUS_BUFFER_SIZE));
}
/*
EEPROM_WriteWearLeveledBlock() is called to store the value of a
parameter in EEPROM. Internally, it checks to see if each byte being
stored is different than the one currently present in EEPROM, and
writes only occur for bytes that have changed. */
void EEPROM_WriteWearLeveledBlock(const uint16_t param, const void *data, const uint16_t len) {
  for (uint16_t i = 0; i < len; ++i)
    EEPROM_WriteWearLeveledByte(param + i * (EE_PARAM_BUFFER_SIZE + EE_STATUS_BUFFER_SIZE),
                                *(((uint8_t *)data) + i));
}

#endif // EEPROM_INCLUDE_BLOCK_FUNCS

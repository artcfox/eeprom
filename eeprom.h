/*

  eeprom.h

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

#pragma once

#include <stdint.h>
/*
 * This library is intended to be compiled for an AVR microcontroller,
 * but for debugging purposes, it may also be compiled on a computer,
 * in which case the EEPROM will be simulated using an array, and a
 * function called EEPROM_Print will become available, which prints
 * the contents of the simulated EEPROM to stdout.
 *
 * In order to differentiate between the two platforms, it is assumed
 * that F_CPU is only defined when being compiled on an AVR.
 *
 * You probably want to program the FUSES on your AVR to preserve the
 * contents of the EEPROM (so the EEPROM doesn't get wiped clean each
 * time you flash a new program), and enable BOD (Brown Out Detection)
 * to avoid EEPROM corruption if the supply voltage falls too low.
 */
#ifdef F_CPU
#include <avr/io.h>
/*
 * If EE_EEPROM_END is defined before including this header file, then
 * a compile-time assert will be used to ensure that all wear-leveled
 * parameters and associated metadata fit into the available EEPROM.
 *
 * EE_EEPROM_END should be defined to point to the first address after
 * the last used parameter.
 */
#ifdef EE_EEPROM_END
_Static_assert((EE_EEPROM_END <= E2END + 1), "Available EEPROM memory exceeded. Consider setting EEPROM_WEAR_LEVEL_FACTOR to a lower value.");
#endif // EE_EEPROM_END
#else // F_CPU
// Only used for simulation when compiled on a computer
extern uint8_t eeprom[EEPROM_SIMULATED_SIZE];
#ifdef EE_EEPROM_END
_Static_assert((EE_EEPROM_END <= sizeof(eeprom)), "Available EEPROM memory exceeded. Consider setting EEPROM_WEAR_LEVEL_FACTOR to a lower value.");
#endif // EE_EEPROM_END
/*
 * EEPROM_Print
 *
 * Prints the contents of the simulated EEPROM, between two locations,
 * to stdout.
 *
 * begin [in]
 *   The first location of the simulated EEPROM to be printed.
 *
 * end [in]
 *   The location one after the last location of the simulated EEPROM
 *   to be printed.
 */
void EEPROM_Print(const uint16_t begin, const uint16_t end);
#endif // F_CPU

#if (EEPROM_INCLUDE_BYTE_FUNCS)
/*
 * EEPROM_InitWearLeveledByte
 *
 * Initializes a segment of EEPROM to use for wear-leveled storage of
 * a byte of memory, and writes the initial value to EEPROM. The total
 * length of this segment, including metadata, occupies
 * (EEPROM_WEAR_LEVEL_FACTOR * 2) bytes of EEPROM.
 * 
 * param [in]
 *   The offset into EEPROM where the wear-leveled segment begins.
 *
 * data [in]
 *   A byte containing the initial data to store in EEPROM.
 *
 * Returns:
 *   A copy of data, for convenience.
 * 
 * This function only needs to be invoked once per offset in order to
 * initialize the metadata necessary for wear-leveling, after which
 * the function EEPROM_ReadWearLeveledByte may be used to read the
 * byte from EEPROM into memory, and EEPROM_WriteWearLeveledByte to
 * write a new byte from memory into EEPROM.
 */
uint8_t EEPROM_InitWearLeveledByte(const uint16_t param, const uint8_t data);

/*
 * EEPROM_ReadWearLeveledByte
 *
 * Reads, and returns the byte of data currently stored in a
 * wear-leveled segment of EEPROM.
 * 
 * param [in]
 *   The offset into EEPROM where the wear-leveled segment begins.
 *
 * Returns:
 *   The byte of data stored in that segment of EEPROM.
 * 
 * This function may only be invoked if EEPROM_InitWearLeveledByte has
 * previously been invoked on the same segment of EEPROM.
 */
uint8_t EEPROM_ReadWearLeveledByte(const uint16_t param);

/*
 * EEPROM_WriteWearLeveledByte
 *
 * Writes a byte of data into a wear-leveled segment of EEPROM.
 * 
 * param [in]
 *   The offset into EEPROM where the wear-leveled segment begins.
 *
 * data [in]
 *   The byte of data to store in EEPROM.
 *
 * This function may only be invoked if EEPROM_InitWearLeveledByte
 * has previously been invoked on the same segment of EEPROM.
 */
void EEPROM_WriteWearLeveledByte(const uint16_t param, const uint8_t data);
#endif // EEPROM_INCLUDE_BYTE_FUNCS

#if (EEPROM_INCLUDE_BLOCK_FUNCS)
/*
 * EEPROM_InitWearLeveledBlock
 *
 * Initializes a segment of EEPROM to use for wear-leveled storage of
 * a block of memory, and writes the contents of the buffer to
 * EEPROM. The total length of this segment, including metadata,
 * occupies (len * EEPROM_WEAR_LEVEL_FACTOR * 2) bytes of EEPROM.
 * 
 * param [in]
 *   The offset into EEPROM where the wear-leveled segment begins.
 *
 * data [in]
 *   A pointer to the buffer containing the initial data to store in
 *   EEPROM.
 *
 * len [in]
 *   The size of the buffer, in bytes.
 *
 * This function only needs to be invoked once per offset in order to
 * initialize the metadata necessary for wear-leveling, after which
 * the function EEPROM_ReadWearLeveledBlock may be used to read the
 * block from EEPROM into memory, and EEPROM_WriteWearLeveledBlock to
 * write a new block from memory into EEPROM.
 */
void EEPROM_InitWearLeveledBlock(const uint16_t param, const void *data, const uint16_t len);

/*
 * EEPROM_ReadWearLeveledBlock
 *
 * Reads, and copies the block of data currently stored in a
 * wear-leveled segment of EEPROM into the supplied buffer.
 * 
 * param [in]
 *   The offset into EEPROM where the wear-leveled segment begins.
 *
 * data [out]
 *   A pointer to a buffer large enough to contain the block of data
 *   stored in EEPROM.
 *
 * len [in]
 *   The size of the buffer, in bytes.
 *
 * This function may only be invoked if EEPROM_InitWearLeveledBlock
 * has previously been invoked on the same segment of EEPROM.
 */
void EEPROM_ReadWearLeveledBlock(const uint16_t param, void *data, const uint16_t len);

/*
 * EEPROM_WriteWearLeveledBlock
 *
 * Writes the contents of the supplied buffer into a wear-leveled
 * segment of EEPROM.
 * 
 * param [in]
 *   The offset into EEPROM where the wear-leveled segment begins.
 *
 * data [in]
 *   A pointer to the buffer containing the data to store in EEPROM.
 *
 * len [in]
 *   The size of the buffer, in bytes.
 *
 * This function may only be invoked if EEPROM_InitWearLeveledBlock
 * has previously been invoked on the same segment of EEPROM.
 */
void EEPROM_WriteWearLeveledBlock(const uint16_t param, const void *data, const uint16_t len);
#endif // EEPROM_INCLUDE_BLOCK_FUNCS

# ---------- Begin EEPROM Configuration Section ----------

# EEPROM_WEAR_LEVEL_FACTOR determines the number of bytes that writes
# to a single wear-leveled byte are distributed evenly across.
# Including metadata, the actual number of EEPROM bytes used to store
# each wear-leveled byte is EEPROM_WEAR_LEVEL_FACTOR * 2.
#
# Note: Eight levels will guarantee 800k writes.
EEPROM_WEAR_LEVEL_FACTOR = 8

# Flag for including functions for wear-leveling arbitrarily sized
# blocks of memory, rather than just single bytes.
#  0 = Do not include functions for operating on blocks of memory
#  1 = Include functions for operating on blocks of memory
EEPROM_INCLUDE_BLOCK_FUNCS = 1

# Flag for including functions for wear-leveling single bytes of
# memory.
#  0 = Do not include functions for operating on blocks of memory
#  1 = Include functions for operating on blocks of memory
EEPROM_INCLUDE_BYTE_FUNCS = 1

# For simulation purposes, this library will also compile and run on a
# computer. When running on a computer, reads and writes to the EEPROM
# will be simulated using an array named "eeprom" and a function named
# EEPROM_Print() will be defined, which prints the entire contents of
# the simulated EEPROM to the screen. To configure the size of the
# simulated EEPROM, modify EEPROM_SIMULATED_SIZE below.
EEPROM_SIMULATED_SIZE = 512

# ---------- End EEPROM Configuration Section ----------

# ---------- DO NOT MODIFY BELOW THIS LINE ----------

ifeq ($(EEPROM_INCLUDE_BLOCK_FUNCS), 0)
ifeq ($(EEPROM_INCLUDE_BYTE_FUNCS), 0)
$(warning @@@@@@@@@@@@@@@@@@@ EEPROM_INCLUDE_BYTE_FUNCS WARNING @@@@@@@@@@@@@@@@@@@)
$(warning @ Your configuration of EEPROM_INCLUDE_BYTE_FUNCS = 0 has been overridden)
$(warning @ and will be changed to EEPROM_INCLUDE_BYTE_FUNCS = 1, because at least)
$(warning @ one set of EEPROM functions must be included.)
$(warning @)
$(warning @ To prevent this warning from being displayed, you should explicitly set)
$(warning @ EEPROM_INCLUDE_BYTE_FUNCS = 1 or EEPROM_INCLUDE_BLOCK_FUNCS = 1)
$(warning @@@@@@@@@@@@@@@@@@@ EEPROM_INCLUDE_BYTE_FUNCS WARNING @@@@@@@@@@@@@@@@@@@)
EEPROM_INCLUDE_BYTE_FUNCS = 1
endif
endif

# This line integrates all EEPROM-related defines into a single flag called:
#     $(EEPROM_DEFINES)
# which should be appended to the definition of COMPILE in the Makefile
EEPROM_DEFINES = -DEEPROM_WEAR_LEVEL_FACTOR=$(EEPROM_WEAR_LEVEL_FACTOR) \
                 -DEEPROM_INCLUDE_BLOCK_FUNCS=$(EEPROM_INCLUDE_BLOCK_FUNCS) \
                 -DEEPROM_INCLUDE_BYTE_FUNCS=$(EEPROM_INCLUDE_BYTE_FUNCS) \
                 -DEEPROM_SIMULATED_SIZE=$(EEPROM_SIMULATED_SIZE)

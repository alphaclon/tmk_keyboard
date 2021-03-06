#----------------------------------------------------------------------------
# On command line:
#
# make all = Make software.
#
# make clean = Clean out built project files.
#
# make coff = Convert ELF to AVR COFF.
#
# make extcoff = Convert ELF to AVR Extended COFF.
#
# make program = Download the hex file to the device.
#                Please customize your programmer settings(PROGRAM_CMD)
#
# make teensy = Download the hex file to the device, using teensy_loader_cli.
#               (must have teensy_loader_cli installed).
#
# make dfu = Download the hex file to the device, using dfu-programmer (must
#            have dfu-programmer installed).
#
# make flip = Download the hex file to the device, using Atmel FLIP (must
#             have Atmel FLIP installed).
#
# make dfu-ee = Download the eeprom file to the device, using dfu-programmer
#               (must have dfu-programmer installed).
#
# make flip-ee = Download the eeprom file to the device, using Atmel FLIP
#                (must have Atmel FLIP installed).
#
# make dfu-start = Restart using dfu-programmer
#                  (must have dfu-programmer installed).
#
# make debug = Start either simulavr or avarice as specified for debugging, 
#              with avr-gdb or avr-insight as the front end for debugging.
#
# make filename.s = Just compile filename.c into the assembler code only.
#
# make filename.i = Create a preprocessed source file for use in submitting
#                   bug reports to the GCC project.
#
# To rebuild project do "make clean" then "make all".
#
# avrdude -v -c avr109 -p atmega32U4 -P /dev/ttyACM0 -U flash:w:anorak_kiibohd_lufa.hex
# 
# Fuses for nanoBoot:
# avrdude -v -c stk500v2 -p m32u4 -P /dev/ttySTK500 -B0.1 -U lfuse:w:0xff:m -U hfuse:w:0xd6:m -U efuse:w:0xf8:m
#
# Flash nanoBoot
# avrdude -v -c stk500v2 -p m32u4 -P /dev/ttySTK500 -B0.1 -U flash:w:nanoBoot.hex:a
# 
#
#----------------------------------------------------------------------------

# Target file name (without extension).
TARGET = anorak_kiibohd_lufa

# Directory common source files exist
TMK_DIR = ../../tmk_core

# Directory keyboard dependent files exist
TARGET_DIR = .

# List C source files here. (C dependencies are automatically generated.)
SRC += \
	led.c \
	hooks.c \
	utils.c \
	matrix.c \
	command.c \
	keymap_common.c \
	uart/uart.c \
	led_backlight/key_led_map.cpp \
	led_backlight/avr315/TWI_Master.c \
	led_backlight/avr315/twi_transmit_queue.c \
	led_backlight/i2cmaster/twimaster.c \
	led_backlight/gfx/new.cpp \
	led_backlight/gfx/glcdfont.c \
	led_backlight/gfx/Adafruit_GFX.cpp \
	led_backlight/gfx/WString.cpp \
	led_backlight/gfx/Print.cpp \
	led_backlight/IS31FL3731/IS31FL3731.cpp \
	led_backlight/IS31FL3731/IS31FL3731_buffered.cpp \
	led_backlight/backlight_kiibohd.c \
	led_backlight/control.cpp \
	led_backlight/led_control.cpp \
	led_backlight/pwm_control.cpp \
	led_backlight/led_masks.cpp \
	led_backlight/sleep_led.cpp \
	led_backlight/eeconfig_backlight.c \
	led_backlight/animations/animation.cpp \
	led_backlight/animations/animation_utils.cpp \
	led_backlight/animations/sweep.cpp \
	led_backlight/animations/breathing.cpp \
	led_backlight/animations/type_o_matic.cpp \
	led_backlight/animations/type_o_circles.cpp
	

#	led_backlight/twi/twi_master.c \
#	led_backlight/i2cmaster/twimaster.c \
#	led_backlight/avr315/TWI_Master.c \

#PROGRAM_CMD = avrdude -v -c avr109 -p atmega32u4 -P /dev/ttyACM0 -U flash:w:anorak_kiibohd_lufa.hex
PROGRAM_CMD = /home/wenk/bin/hid_bootloader_cli -v -mmcu=atmega32u4 anorak_kiibohd_lufa.hex

CONFIG_H = config.h


# MCU name
# PJRC Teensy++ 2.0
# MCU = at90usb1286
# TMK Alt Controller or PJRC Teensy 2.0
MCU = atmega32u4

# Processor frequency.
#     This will define a symbol, F_CPU, in all source code files equal to the
#     processor frequency in Hz. You can then use this symbol in your source code to
#     calculate timings. Do NOT tack on a 'UL' at the end, this will be done
#     automatically to create a 32-bit value in your source code.
#
#     This will be an integer division of F_USB below, as it is sourced by
#     F_USB after it has run through any CPU prescalers. Note that this value
#     does not *change* the processor frequency - it should merely be updated to
#     reflect the processor speed set externally so that the code can use accurate
#     software delays.
F_CPU = 16000000

#
# LUFA specific
#
# Target architecture (see library "Board Types" documentation).
ARCH = AVR8

# Input clock frequency.
#     This will define a symbol, F_USB, in all source code files equal to the
#     input clock frequency (before any prescaling is performed) in Hz. This value may
#     differ from F_CPU if prescaling is used on the latter, and is required as the
#     raw input clock is fed directly to the PLL sections of the AVR for high speed
#     clock generation for the USB and other AVR subsections. Do NOT tack on a 'UL'
#     at the end, this will be done automatically to create a 32-bit value in your
#     source code.
#
#     If no clock division is performed on the input clock inside the AVR (via the
#     CPU clock adjust registers or the clock division fuses), this will be equal to F_CPU.
F_USB = $(F_CPU)

# Interrupt driven control endpoint task(+60)
OPT_DEFS += -DINTERRUPT_CONTROL_ENDPOINT

# Boot Section Size in *bytes*
#   Teensy halfKay   512
#   Teensy++ halfKay 1024
#   Atmel DFU loader 4096	(TMK Alt Controller)
#   LUFA bootloader  4096
#   USBaspLoader     2048
OPT_DEFS += -DBOOTLOADER_SIZE=512


# Build Options
#   comment out to disable the options.
#
#MOUSEKEY_ENABLE = yes		# Mouse keys
BOOTMAGIC_ENABLE = yes		# Virtual DIP switch configuration
EXTRAKEY_ENABLE = yes		# Audio control and System control
CONSOLE_ENABLE = yes		# Console for debug
COMMAND_ENABLE = yes    	# Commands for debug and configuration
BACKLIGHT_ENABLE = yes      # Enable keyboard backlight functionality
#NKRO_ENABLE = yes		    # USB Nkey Rollover
SLEEP_LED_ENABLE = yes      # Breathing sleep LED during USB suspend
SLEEP_LED_USE_COMMON = yes


#OPT_DEFS += -DNO_ACTION_TAPPING
#OPT_DEFS += -DNO_ACTION_LAYER
#OPT_DEFS += -DNO_ACTION_MACRO

#
# Keymap file
#
ifdef KEYMAP
    SRC := keymap_$(KEYMAP).c $(SRC)
else
    SRC := keymap_kiibohd.c $(SRC)
endif


# Search Path
VPATH += $(TARGET_DIR)
VPATH += $(TMK_DIR)

include $(TMK_DIR)/protocol/lufa.mk
include $(TMK_DIR)/common.mk
include $(TMK_DIR)/rules.mk

debug-on: EXTRAFLAGS += -DDEBUG
#debug-on: EXTRAFLAGS += -DDEBUG_OUTPUT_ENABLE
#debug-on: EXTRAFLAGS += -DDEBUG_LATE_TEST
#debug-on: EXTRAFLAGS += -DDEBUG_BACKLIGHT
#debug-on: EXTRAFLAGS += -DDEBUG_BACKLIGHT_EXTENDED
#debug-on: EXTRAFLAGS += -DDEBUG_ISSI
#debug-on: EXTRAFLAGS += -DDEBUG_ISSI_PERFORMANCE
#debug-on: EXTRAFLAGS += -DDEBUG_I2C
#debug-on: EXTRAFLAGS += -DDEBUG_TX_QUEUE
#debug-on: EXTRAFLAGS += -DDEBUG_ANIMATION
#debug-on: EXTRAFLAGS += -DLUFA_DEBUG_UART
#debug-on: EXTRAFLAGS += -DLUFA_DEBUG
#debug-on: EXTRAFLAGS += -DDEBUG_SHOW_SCAN_LED
#debug-on: EXTRAFLAGS += -DDEBUG_ACTION
debug-on: all

debug-off: EXTRAFLAGS += -DNO_DEBUG
debug-off: all

debug-off-noprint: EXTRAFLAGS += -DNO_DEBUG -DNO_PRINT
debug-off-noprint: OPT_DEFS := $(filter-out -DCONSOLE_ENABLE,$(OPT_DEFS))
debug-off-noprint: all


boot: 
	@echo teensy_loader_cli: Boot only, do not program
	teensy_loader_cli --mcu $(MCU) -v -b

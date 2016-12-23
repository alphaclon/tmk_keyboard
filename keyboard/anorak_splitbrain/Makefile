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
# Default fuses (factory) : E:F3, H:99, L:5E
# Used fuses              : E:F3, H:D9, L:DE
#
# Flashing the bootloader:
#  avrdude -v -c stk500v2 -p usb1286 -P /dev/ttySTK500 -B0.1 -D -U flash:w:BootloaderDFU.hex:a
#
#----------------------------------------------------------------------------

# Target file name (without extension).
TARGET = anorak_splitbrain_lufa

# Directory common source filess exist
TMK_DIR = ../../tmk_core

# Directory keyboard dependent files exist
TARGET_DIR = .

PROGRAM_CMD = avrdude -v -c stk500v2 -p usb1286 -P /dev/ttySTK500 -B 0.1 -U flash:w:anorak_splitbrain_lufa.hex

# List C source files here. (C dependencies are automatically generated.)
SRC += \
	led.c \
	crc8.c \
	matrix.c \
	keymap_splitbrain.c \
	splitbrain.c \
	infodisplay.c \
	hooks.c \
	command.c \
	uart/uart.c \
	twi/twi/twi_master.c \
	backlight/gfx/new.cpp \
	backlight/gfx/glcdfont.c \
	backlight/gfx/Adafruit_GFX.cpp \
	backlight/gfx/WString.cpp \
	backlight/gfx/Print.cpp \
	backlight/IS31FL3731/IS31FL3731.cpp \
	backlight/IS31FL3731/IS31FL3731_buffered.cpp \
	backlight/backlight_kiibohd.c \
	backlight/control.cpp \
	backlight/led_control.cpp \
	backlight/pwm_control.cpp \
	backlight/led_masks_leftside.cpp \
	backlight/led_masks_rightside.cpp \
	backlight/sleep_led.cpp \
	backlight/key_led_map.cpp \
	backlight/animations/animation.cpp \
	backlight/animations/animation_utils.cpp \
	backlight/animations/sweep.cpp \
	backlight/animations/breathing.cpp \
	backlight/animations/type_o_matic.cpp \
	backlight/animations/type_o_circles.cpp	

#	twi/twi/twi_master.c \
#	twi/i2cmaster/twimaster.c \
#	twi/avr315/TWI_Master.c \

CONFIG_H = config.h


# MCU name
# PJRC Teensy++ 2.0
# MCU = at90usb1286
# TMK Alt Controller or PJRC Teensy 2.0
MCU = at90usb1286

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
OPT_DEFS += -DBOOTLOADER_SIZE=4096


# Build Options
#   comment out to disable the options.
#
BOOTMAGIC_ENABLE = yes      # Virtual DIP switch configuration(+1000)
#MOUSEKEY_ENABLE = yes       # Mouse keys(+4700)
EXTRAKEY_ENABLE = yes       # Audio control and System control(+450)
CONSOLE_ENABLE = yes        # Console for debug(+400)
COMMAND_ENABLE = yes        # Commands for debug and configuration
NKRO_ENABLE = yes          # USB Nkey Rollover - not yet supported in LUFA
BACKLIGHT_ENABLE = yes     # Enable keyboard backlight functionality
SLEEP_LED_ENABLE = yes      # Breathing sleep LED during USB suspend
VIRTUAL_SERIAL_ENABLE = no

#OPT_DEFS += -DNO_ACTION_TAPPING
#OPT_DEFS += -DNO_ACTION_LAYER
#OPT_DEFS += -DNO_ACTION_MACRO

# Do not wait for a connection to an USB host during startup
OPT_DEFS += -DLUFA_NO_WAIT_FOR_USB

# Search Path
VPATH += $(TARGET_DIR)
VPATH += $(TMK_DIR)

include $(TMK_DIR)/protocol/lufa.mk
include $(TMK_DIR)/common.mk
include $(TMK_DIR)/rules.mk


debug-on: EXTRAFLAGS += -DDEBUG
debug-on: EXTRAFLAGS += -DDEBUG_BACKLIGHT
debug-on: EXTRAFLAGS += -DDEBUG_SPLITBRAIN
#debug-on: EXTRAFLAGS += -DLUFA_DEBUG_UART
#debug-on: EXTRAFLAGS += -DLUFA_DEBUG
#debug-on: EXTRAFLAGS += -DDEBUG_ACTION
debug-on: all

debug-off: EXTRAFLAGS += -DNO_DEBUG -DNO_PRINT
debug-off: OPT_DEFS := $(filter-out -DCONSOLE_ENABLE,$(OPT_DEFS))
debug-off: all

dfu-hex: $(TARGET).hex
	@echo -n dfu-programmer: waiting
	@until dfu-programmer $(MCU) get bootloader-version > /dev/null 2>&1; do \
		echo  -n "."; \
		sleep 1; \
	done
	@echo
	dfu-programmer $(MCU) erase --force
	dfu-programmer $(MCU) flash $(TARGET).hex
	dfu-programmer $(MCU) launch --no-reset
	
dfu-launch:
	dfu-programmer $(MCU) launch --no-reset

dfu-eep: $(TARGET).hex $(TARGET).eep
	dfu-programmer $(MCU) flash --eeprom $(TARGET).eep
	dfu-programmer $(MCU) launch --no-reset

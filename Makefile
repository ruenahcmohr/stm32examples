#this makefile is helped in part by https://github.com/libopencm3/libopencm3-examples/blob/master/examples/Makefile.rules

#what is the main file 
BINARY = program
#OBJS = $(BINARY).o $(HL_LIBS:.c=.o)

CFILES = $(wildcard *.c)
HFILES = $(wildcard *.h)
OBJS = $(CFILES:%.c=%.o)

#LibopenCM3 location
libopenCM3Path =/usr/lib/arm

SRCLIBDIR = $(libopenCM3Path)
INCLUDE_DIR = $(libopenCM3Path)/include
LIB_DIR = $(libopenCM3Path)/lib


OPENOCD_SCRIPTS_DIR = /usr/share/openocd/scripts

######Device specific things
DEVICE = stm32f103c8t6
#DEVICE = stm32f103x8

toolchainPath ?= /usr/bin/

PREFIX		?= $(toolchainPath)arm-none-eabi

CC		:= $(PREFIX)-gcc
LD		:= $(PREFIX)-gcc
AR		:= $(PREFIX)-ar
AS		:= $(PREFIX)-as
OBJCOPY		:= $(PREFIX)-objcopy
OBJDUMP		:= $(PREFIX)-objdump
GDB		:= $(PREFIX)-gdb


CFLAGS = 
#with thanks to libopencm3 makefile
CFLAGS += -g
CFLAGS += -Wextra -Wshadow -Wimplicit-function-declaration -Wredundant-decls 
CFLAGS += -fno-common -ffunction-sections -fdata-sections
CFLAGS += -MD
CFLAGS += -Wall -Wundef
CFLAGS += -mcpu=cortex-m3 -mthumb
CFLAGS += -T $(LDSCRIPT)

#Includes
CFLAGS += -I $(INCLUDE_DIR)


#######  LD Script #################
# libopencm3 makefile that does the ldscript
#uses libopencm3 to make a .ld file for the specified device
LDFLAGS		?= 
LDFLAGS		+= --static -nostartfiles
LDFLAGS		+= -L$(LIB_DIR)
LDFLAGS		+= -T$(LDSCRIPT)
LDFLAGS		+= -Wl,-Map=$(*).map
LDFLAGS		+= -Wl,--gc-sections


LDLIBS		+= -lopencm3_stm32f1
LDLIBS		+= -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group 
#include <math.h>
LDLIBS          += -lm

	
.SUFFIXES: .elf .bin .hex .srec .list .map .images
.SECONDEXPANSION:
.SECONDARY:

all: elf bin

include $(libopenCM3Path)/ld/Makefile.linker

# new version
#  include $(libopenCM3Path)/mk/genlink-config.mk

elf: $(BINARY).elf
bin: $(BINARY).bin
hex: $(BINARY).hex
srec: $(BINARY).srec
list: $(BINARY).list

images: $(BINARY).images
flash: $(BINARY).flash

%.images: %.bin %.hex %.srec %.list %.map
	@#printf "*** $* images generated ***\n"

%.bin: %.elf
	@#printf "  OBJCOPY $(*).bin\n"
	$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.hex: %.elf
	@#printf "  OBJCOPY $(*).hex\n"
	$(OBJCOPY) -Oihex $(*).elf $(*).hex

%.srec: %.elf
	@#printf "  OBJCOPY $(*).srec\n"
	$(OBJCOPY) -Osrec $(*).elf $(*).srec

%.list: %.elf
	@#printf "  OBJDUMP $(*).list\n"
	$(OBJDUMP) -S $(*).elf > $(*).list

%.elf %.map: $(OBJS) $(HFILES) $(LDSCRIPT)
	@#printf "  LD      $(*).elf\n"
	$(LD) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(BINARY).elf

%.o: %.c
	@#printf "  CC      $(*).c\n"
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).c

clean:
	@#printf "  CLEAN\n"
	$(RM) $(OBJS)
	$(RM) *.o *.d *.elf *.bin *.hex *.srec *.list *.map


install: program.elf
#	openocd -f $(OPENOCD_SCRIPTS_DIR)/interface/stlink-v2.cfg -f $(OPENOCD_SCRIPTS_DIR)/target/stm32f1x_stlink.cfg -c init -c targets -c "halt" -c "flash write_image erase ./$(BINARY).elf" -c "verify_image ./$(BINARY).elf" -c "reset run" -c shutdown
#	openocd -f $(OPENOCD_SCRIPTS_DIR)/interface/stlink-v2.cfg -f $(OPENOCD_SCRIPTS_DIR)/target/stm32f1x.cfg -c init -c targets -c "halt" -c "flash write_image erase ./$(BINARY).elf" -c "verify_image ./$(BINARY).elf" -c "reset run" -c shutdown
	openocd -f $(OPENOCD_SCRIPTS_DIR)/interface/stlink-v2.cfg -f $(OPENOCD_SCRIPTS_DIR)/target/stm32f1x.cfg -c init -c "reset halt" -c "flash write_image erase ./$(BINARY).elf" -c "verify_image ./$(BINARY).elf" -c "reset run" -c shutdown

debug: main.elf main.bin;
	$(GDB) --eval-command="target ext:3335"  main.elf

telnet:;
	telnet localhost 4446
            
testecho:;
	echo $(libopenCM3Path)
	echo $(LD_PARAMS)


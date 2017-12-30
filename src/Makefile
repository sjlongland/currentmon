# gcc Makefile for LPC810
# Kamal Mostafa <kamal@whence.com>
#
# License: Public Domain

CROSS ?= arm-none-eabi-
CPU = -mthumb -mcpu=cortex-m0plus
FPU = -mfloat-abi=soft

FILENAME = currentmon
LINKERSCRIPT = currentmon.ld

CFLAGS += -Werror -pedantic -Os -ffunction-sections -fdata-sections

CC = ${CROSS}gcc
LD = ${CROSS}ld
OBJCOPY = ${CROSS}objcopy
SIZE = ${CROSS}size

CFLAGS += $(CPU) $(FPU)
LDFLAGS += -Os \
	   -nostartfiles \
	   -nodefaultlibs \
	   -nostdlib \
	   -specs=nosys.specs \
	   -specs=nano.specs \
	   -Wl,--gc-sections \
	   -Wl,-Map=$(FILENAME).map \
	   -T$(LINKERSCRIPT) \

LIBS += -lc -lnosys -lgcc

INC += -I../cmsis -I../third_party/lpcopen/lpc_chip_8xx/inc
CPPFLAGS += $(INC) -D__USE_CMSIS -DINLINE=inline

CURRENTMON_OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
uart.o : CFLAGS += -fno-builtin

all: $(FILENAME).bin

$(FILENAME).axf: $(LINKERSCRIPT) $(CURRENTMON_OBJS)
	$(CC) $(LDFLAGS) -o $@ \
		-Wl,--start-group $(CURRENTMON_OBJS) \
		-Wl,--end-group $(LIBS)
	-@echo ""
	@$(SIZE) $(FILENAME).axf

clean:
	@rm -f *.o *.d
	@rm -f $(FILENAME).axf $(FILENAME).hex $(FILENAME).bin

%.bin: %.axf
	-@echo ""
	-@echo "Generating $(FILENAME).hex (FlashMagic, etc.)"
	@$(OBJCOPY) --strip-unneeded -O ihex $(FILENAME).axf $(FILENAME).hex
	-@echo "Generating $(FILENAME).bin"
	@$(OBJCOPY) --strip-unneeded -O binary $(FILENAME).axf $(FILENAME).bin

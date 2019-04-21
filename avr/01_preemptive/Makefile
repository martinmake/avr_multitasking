# PROJECT DIRS
IDIRS:=inc
SDIR :=src
ODIR :=obj
DDIR :=dep
BDIR :=bin

# PROJECT FILES
ELF  :=$(BDIR)/firmware.elf
HEX  :=$(BDIR)/firmware.hex
BIN  :=$(BDIR)/firmware.bin
ASM  :=$(BDIR)/firmware.asm

# LIB DIRS
LDIR :=$(HOME)/Documents/libs/AVR
LIDIR:=$(LDIR)/inc
LADIR:=$(LDIR)/arc

# LIBS AND DEFS
LIBS:=usart
DEFS:=

# TARGET
MCU     :=atmega328p
F_CPU   :=16000000
TIO_BAUD:=9600
PROG    :=arduino
PORT    :=/dev/ttyUSB0

# TOOLCHAIN
TOOLCHAIN:=avr-
CC     :=$(TOOLCHAIN)gcc
CXX    :=$(TOOLCHAIN)g++
ASMC   :=$(TOOLCHAIN)gcc
LD     :=$(TOOLCHAIN)g++
OBJCOPY:=$(TOOLCHAIN)objcopy
OBJDUMP:=$(TOOLCHAIN)objdump
SIZE   :=$(TOOLCHAIN)size
STRIP  :=$(TOOLCHAIN)strip

# COMPILER FLAGS
WARNING     :=-Wall -Wextra -pedantic -Wno-write-strings
OPTIMIZATION:=-O3 -Os -fdata-sections -ffunction-sections -fno-exceptions

######END#OF#CONFIGURATION#VARIABLES######

LIBS+=standard
DEFS+=$(EDEFS) F_CPU=$(F_CPU) TIO_BAUD=$(TIO_BAUD)

LIBS  :=$(addprefix -l,$(LIBS))
DEFS  :=$(addprefix -D,$(DEFS))

C_SRCS  :=$(shell find $(SDIR) -name '*.c'  )
CXX_SRCS:=$(shell find $(SDIR) -name '*.cpp')
ASM_SRCS:=$(shell find $(SDIR) -name '*.S'  )

C_OBJS  :=$(patsubst $(SDIR)/%.c,  $(ODIR)/%.o,$(C_SRCS)  )
CXX_OBJS:=$(patsubst $(SDIR)/%.cpp,$(ODIR)/%.o,$(CXX_SRCS))
ASM_OBJS:=$(patsubst $(SDIR)/%.S,  $(ODIR)/%.o,$(ASM_SRCS))
OBJS    :=$(C_OBJS) $(CXX_OBJS) $(ASM_OBJS)

INC:=$(addprefix -I,$(IDIRS) $(LIDIR))

CCFLAGS :=$(WARNING) $(OPTIMIZATION) -s -mmcu=$(MCU) $(INC)  $(DEFS) -std=c99
CXXFLAGS:=$(WARNING) $(OPTIMIZATION) -s -mmcu=$(MCU) $(INC)  $(DEFS) -std=c++14 -fno-rtti
ASMFLAGS:=$(WARNING) $(OPTIMIZATION) -s -mmcu=$(MCU) $(INC)  $(DEFS)
LDFLAGS :=$(WARNING) $(OPTIMIZATION)    -mmcu=$(MCU) -L$(LADIR)/$(MCU) -Wl,--gc-sections

.SECONDARY: $(OBJS)

.PHONY: all
all: $(ELF)
	@echo "> BULDING"
	$(OBJCOPY) -j .data -j .text -O ihex   $(ELF)   $(HEX)
	$(OBJCOPY) -j .data -j .text -O binary $(ELF)   $(BIN)
	$(OBJDUMP) -DCz                        $(ELF) > $(ASM)
	$(SIZE) $(HEX) | sed -e 's/^/* /'
	@echo "< BULDING DONE"

$(ELF): $(OBJS)
	@echo "> LINKING"
	$(LD) $(LDFLAGS) $^ $(LIBS) -o $@
	@echo "< LINKING DONE"

$(ODIR)/%.o: $(SDIR)/%.c
	@echo "> COMPILING $<"
	$(CC)   $(CCFLAGS) -c $< -o $@
	@echo "< COMPILING $< DONE"
$(ODIR)/%.o: $(SDIR)/%.cpp
	@echo "> COMPILING $<"
	$(CXX)  $(CXXFLAGS) -c $< -o $@
	@echo "< COMPILING $< DONE"
$(ODIR)/%.o: $(SDIR)/%.S
	@echo "> COMPILING $<"
	$(ASMC) $(ASMFLAGS) -c $< -o $@
	@echo "< COMPILING $< DONE"

$(DDIR)/%.d: $(SDIR)/%.c
	@echo "> BUILDING DEPENDENCIES $<"
	$(CC)   $(INC) -MM $< -MT '$(patsubst $(DDIR)/%.d,$(ODIR)/%.o,$@)' > $@
	@echo "< BUILDING DEPENDENCIES $< DONE"
$(DDIR)/%.d: $(SDIR)/%.cpp
	@echo "> BUILDING DEPENDENCIES $<"
	$(CXX)  $(INC) -MM $< -MT '$(patsubst $(DDIR)/%.d,$(ODIR)/%.o,$@)' > $@
	@echo "< BUILDING DEPENDENCIES $< DONE"
$(DDIR)/%.d: $(SDIR)/%.S
	@echo "> BUILDING DEPENDENCIES $<"
	$(ASMC) $(INC) -MM $< -MT '$(patsubst $(DDIR)/%.d,$(ODIR)/%.o,$@)' > $@
	@echo "< BUILDING DEPENDENCIES $< DONE"

.PHONY: flash
flash:
	sudo avrdude -p $(MCU) -c $(PROG) -P $(PORT) -U flash:w:$(HEX):i

.PHONY: serial
serial:
	sudo tio -b $(TIO_BAUD) -d 8 -s 2 -e -mINLCRNL $(PORT)

.PHONY: setup
setup:
	@echo "> MAKING DIRECTORIES"
	mkdir -p $(BDIR) $(ODIR) $(DDIR) $(IDIRS) $(SDIR)
	touch $(BDIR)/.empty $(ODIR)/.empty $(DDIR)/.empty
	@echo "< MAKING DIRECTORIES DONE"

.PHONY: clean
clean:
	@echo "> CLEANING"
	-rm -vfr ./$(ODIR)/* ./$(DDIR)/* $(ELF) $(HEX) $(BIN) $(ASM) | sed -e 's/^/* /'
	@echo "< CLEANING DONE"

ifeq ($(NODEP),)
-include $(patsubst $(SDIR)/%.c,  $(DDIR)/%.d,$(C_SRCS)  )
-include $(patsubst $(SDIR)/%.cpp,$(DDIR)/%.d,$(CXX_SRCS))
-include $(patsubst $(SDIR)/%.S,  $(DDIR)/%.d,$(ASM_SRCS))
endif

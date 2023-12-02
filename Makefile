MKDIR = mkdir -p
CP = cp -f

# assembler
ASM = /usr/bin/nasm

# compiler

# !!! DO NOT CHANGE THIS !!!
# i don't care if you can't use clang, i don't care if you hate clang.
# this project targets clang and clang only. you will not be helped if it fails.
# gcc broke this project. i am not fixing it again. - irix
CC = /usr/bin/clang

# linker
LD = /usr/bin/ld

# grub iso creator
GRUB = /usr/bin/grub-mkrescue

# sources
SRC = src
ASM_SRC = $(SRC)/asm

# objects
OBJ = obj
ASM_OBJ = $(OBJ)/asm

CONFIG = ./config
OUT = out
INC = ./include

INCLUDE =-I$(INC)

DEFINES =

# assembler flags
ASM_FLAGS = -f elf32
# compiler flags
CC_FLAGS = $(INCLUDE) $(DEFINES) --target=i686-pc-none-elf -march=i686 -std=gnu99 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -Werror=implicit-function-declaration -Wall -Wextra -ferror-limit=9999

ifeq ($(DEBUG),1)
CC_FLAGS += -g
endif

# linker flags, for linker add linker.ld file too
LD_FLAGS = -m elf_i386 -T $(CONFIG)/linker.ld -nostdlib

# target file to create in linking
TARGET = $(OUT)/catkernel.bin

# iso file target to create
TARGET_ISO = $(OUT)/catkernel.iso
ISO_DIR = $(OUT)/isodir

$(shell $(MKDIR) $(OBJ) $(OUT))

# automatically find all C source files in src/ and its subdirectories
C_SOURCES := $(wildcard $(SRC)/**/*.c $(SRC)/*.c)

# generate object file names from source file names
OBJECTS := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(ASM_OBJ)/entry.o

all: $(TARGET_ISO)

$(TARGET_ISO): $(TARGET)
	@printf "[ building ISO... ]\n"
	$(MKDIR) $(ISO_DIR)/boot/grub
	$(CP) $(TARGET) $(ISO_DIR)/boot/
	$(CP) $(CONFIG)/grub.cfg $(ISO_DIR)/boot/grub/
	$(GRUB) -o $(TARGET_ISO) $(ISO_DIR)
	rm -f $(TARGET)

$(TARGET): $(ASM_OBJECTS) $(OBJECTS)
	@printf "[ linking... ]\n"
	$(LD) $(LD_FLAGS) -o $(TARGET) $^
	grub-file --is-x86-multiboot $(TARGET)
	@printf "\n"

$(ASM_OBJ)/entry.o: $(ASM_SRC)/entry.asm
	@printf "[ $(ASM_SRC)/entry.asm ]\n"
	$(MKDIR) $(dir $@)
	$(ASM) $(ASM_FLAGS) $< -o $@
	@printf "\n"

$(OBJ)/%.o: $(SRC)/%.c
	@printf "[ $< ]\n"
	$(MKDIR) $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@
	@printf "\n"

clean:
	rm -rf $(OBJ)
	rm -rf $(OUT)

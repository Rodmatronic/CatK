# sources
SRC = $(CATK_ROOT)/src/kernel
ASM_SRC = $(SRC)/asm
INC = $(SRC)/include
LIBK_INC = $(SRC)/libk/include

# objects
OBJ := $(OBJ)/kernel
ASM_OBJ = $(OBJ)/asm

INCLUDE =-I$(INC) -I$(LIBK_INC)

ASM_FLAGS = -f elf32
CC_FLAGS = $(INCLUDE) --target=i686-pc-none-elf -march=i686 -std=gnu99 -ffreestanding -fno-exceptions -fno-rtti -fno-stack-protector -Werror=implicit-function-declaration -Wall -Wextra -ferror-limit=9999

ifeq ($(DEBUG), 1)
CC_FLAGS += -g
endif

ifeq ($(OS), Windows_NT)
override SKIP_ISO = 1
override SKIP_MB_CHECK = 1
endif

# linker flags, for linker add linker.ld file too
LD_FLAGS = -m elf_i386 -T $(CONFIG)/linker.ld -nostdlib

# target file to create in linking
TARGET = $(OUT)/catkernel.bin

# iso file target to create
TARGET_ISO = $(OUT)/catkernel.iso
ISO_DIR = $(OUT)/isodir

# automatically find all C source files in src/ and its subdirectories
C_SOURCES := $(shell find $(SRC)/ -type f -name "*.c" -maxdepth 25)
ASM_SOURCES := $(shell find $(ASM_SRC)/ -type f -name "*.asm" -maxdepth 25)

# generate object file names from source file names
OBJECTS := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst $(ASM_SRC)/%.asm,$(ASM_OBJ)/%.o,$(ASM_SOURCES))

all: $(TARGET_ISO)

$(TARGET_ISO): $(TARGET)
	@printf "[ building ISO... ]\n"
ifneq ($(SKIP_ISO), 1)
	$(MKDIR) $(ISO_DIR)/boot/grub
	$(CP) $(TARGET) $(ISO_DIR)/boot/
	$(CP) $(CONFIG)/grub.cfg $(ISO_DIR)/boot/grub/
	grub-mkrescue -o $(TARGET_ISO) $(ISO_DIR)
	$(RM) $(TARGET)
else
	@printf "[ iso build skipped ]\n"
endif

$(TARGET): $(ASM_OBJECTS) $(OBJECTS)
	@printf "[ linking... ]\n"
	$(LD) $(LD_FLAGS) -o $(TARGET) $^
	@printf "[ checking multiboot signature... ]\n"
ifneq ($(SKIP_MB_CHECK), 1)
	grub-file --is-x86-multiboot $(TARGET)
else
	@printf "[ multiboot check skipped ]\n"
endif

$(ASM_OBJ)/%.o: $(ASM_SRC)/%.asm
	@printf "[ $< ]\n"
	$(MKDIR) $(dir $@)
	$(ASM) $(ASM_FLAGS) $< -o $@
	@printf "\n"

$(OBJ)/%.o: $(SRC)/%.c
	@printf "[ $< ]\n"
	$(MKDIR) $(dir $@)
	$(CC) $(CC_FLAGS) -c $< -o $@
	@printf "\n"

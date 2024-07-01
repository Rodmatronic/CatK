# Target files to create
TARGET = catkernel.elf
TARGET_ISO = catkernel.iso

ISO_DIR = $(OUT)/isodir

# Source directory

SRC = $(CATK_ROOT)/src

# Compiler flags and other stuff

INCLUDE_PATH = $(SRC)/include
INCLUDE = -I $(INCLUDE_PATH)

DEBUG := 1

CC_FLAGS = \
	$(INCLUDE)  \
	-ffreestanding \
	--target=i686-pc-none-elf \
	-march=i686 \
	-fno-exceptions \
	-Werror=implicit-function-declaration \
	-nodefaultlibs \
	-nostdlib \
	-O3 \

ifeq ($(DEBUG), 1)
CC_FLAGS += -g
ASM_FLAGS += -g # NASM also supports debug symbols! :)
endif

ASM_FLAGS = -f elf32
LD_FLAGS = -m elf_i386 -T $(CONFIG)/link.ld -znoexecstack

# source files
C_SRC := $(shell find . -type f -name "*.c")
ASM_SRC := $(shell find . -type f -name "*.asm")
# object files
C_OBJ := $(patsubst %.c, %.o, $(C_SRC))
ASM_OBJ := $(patsubst %.asm, %.o, $(ASM_SRC))

all: $(TARGET_ISO)

$(TARGET_ISO): $(TARGET)
ifneq ($(SKIP_ISO), 1)
	@printf "[ Building ISO.. ]\n"
	@$(MKDIR) $(ISO_DIR)/boot/grub
	@$(CP) $(TARGET) $(ISO_DIR)/boot/
	@$(CP) $(CONFIG)/grub.cfg $(ISO_DIR)/boot/grub/
	@grub-mkrescue -o $(OUT)/$(TARGET_ISO) $(ISO_DIR)
	@$(RM) $(TARGET)
else
	@printf "[ Build ISO skipped ]\n"
endif


$(TARGET): $(ASM_OBJ) $(C_OBJ)
	@printf "[ Linking kernel.. ]\n"
	@$(LD) $(LD_FLAGS) -o $(TARGET) $^
	@printf "[ Validating Multiboot2 signature... ]\n"
ifneq ($(SKIP_MB_CHECK), 1)
	@grub-file --is-x86-multiboot2 $(TARGET)
else
	@printf "[ Multiboot2 check skipped ]\n"
endif

%.o: %.asm
	@printf "[ $< ]\n"
	@$(ASM) $(ASM_FLAGS) $< -o $@

%.o: %.c
	@printf "[ $< ]\n"
	@$(CC) -c $< -o $@ $(CC_FLAGS)

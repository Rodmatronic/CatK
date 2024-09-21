# Target files to create
TARGET = catkernel.elf
TARGET_TMP = catkernel.tmp.o
TARGET_ISO = catkernel.iso

ISO_DIR = $(OUT)/isodir

# Source directory

SRC = $(CATK_ROOT)/src

# Compiler flags and other stuff

C_INCLUDE_PATH = $(SRC)/include/
C_INCLUDE = -I $(C_INCLUDE_PATH)

ASM_INCLUDE = $(C_INCLUDE)

DEBUG = 1

CC_FLAGS = \
	$(C_INCLUDE)  \
	-fno-exceptions \
	-ffreestanding \
	--target=i686-pc-none-elf \
	-march=i686 \
	-Werror=implicit-function-declaration \
	-nodefaultlibs \
	-nostdlib \
	-fstack-protector-strong \
	-fno-pie \
	-O3

ASM_FLAGS = \
	$(ASM_INCLUDE)  \
	-ffreestanding \
	--target=i686-pc-none-elf \
	-march=i686 \
	-nodefaultlibs \
	-nostdlib \
	-fno-pie

ifeq ($(DEBUG), 1)
CC_FLAGS += -g
ASM_FLAGS += -g
endif

LD_FLAGS = -m elf_i386 -T $(CONFIG)/link.ld -z noexecstack -nostdlib -static

# source files
C_SRC := $(shell find . -type f -name "*.c")
ASM_SRC := $(shell find . -type f -name "*.S")
# object files
C_OBJ := $(patsubst %.c, %.o, $(C_SRC))
ASM_OBJ := $(patsubst %.S, %.o, $(ASM_SRC))

all: $(TARGET_ISO)

$(TARGET_ISO): $(TARGET)
ifneq ($(SKIP_ISO), 1)
	@printf "[ Building ISO.. ]\n"
	@$(MKDIR) $(ISO_DIR)/boot/grub
	@$(CP) $(TARGET) $(ISO_DIR)/boot/
	@$(CP) $(CONFIG)/grub.cfg $(ISO_DIR)/boot/grub/
	@grub-mkrescue -o $(OUT)/$(TARGET_ISO) $(ISO_DIR)
else
	@printf "[ Build ISO skipped ]\n"
endif

$(TARGET): $(ASM_OBJ) $(C_OBJ)
	@printf "[ Linking kernel.. ]\n"
	@$(LD) $(LD_FLAGS) $^ -o $(TARGET)
	@printf "[ Validating Multiboot2 signature... ]\n"
ifneq ($(SKIP_MB_CHECK), 1)
	@grub-file --is-x86-multiboot2 $(TARGET)
else
	@printf "[ Multiboot2 check skipped ]\n"
endif

%.o: %.S
	@printf "[ $< ]\n"
	@$(ASM) $(ASM_FLAGS) -c $< -o $@

%.o: %.c
	@printf "[ $< ]\n"
	@$(CC) -c $< -o $@ $(CC_FLAGS)

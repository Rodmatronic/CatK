# Target files to create
TARGET = catk
TARGET_TMP = catk.tmp.o
TARGET_ISO = catkernel.iso

ISO_DIR = $(OUT)/isodir

# Source directory

SRC = $(CATK_ROOT)/src

# Compiler flags and other stuff

C_INCLUDE_PATH = $(SRC)/include/
C_INCLUDE = -I $(C_INCLUDE_PATH)

NASM_INCLUDE = -I $(SRC)/include/asm/

DEBUG = 1

# I stole most of this from Linux because we're evil >:)
CC_FLAGS = \
	$(C_INCLUDE)  \
	-fno-exceptions \
	-ffreestanding \
	--target=i686-pc-none-elf \
	-march=i686 \
	-Wall \
	-Wextra \
	-Werror \
	-nodefaultlibs \
	-nostdlib \
	-fstack-protector-strong \
	-fno-common \
	-O2 \
	-pipe \
	-Wp,-D_FORTIFY_SOURCE=2 \
	-Wp,-D_REENTRANT \
	-fexceptions \
	--param=ssp-buffer-size=32 \
	-Wformat \
	-Wformat-security \
	-fasynchronous-unwind-tables \
	-fno-semantic-interposition \
	-ffat-lto-objects \
	-msse4

ifeq ($(DEBUG), 1)
	CC_FLAGS += -feliminate-unused-debug-types
endif

ASM_FLAGS = -f elf32 $(NASM_INCLUDE)

ifeq ($(DEBUG), 1)
CC_FLAGS += -g
ASM_FLAGS += -g
endif

LD_FLAGS = \
	-m elf_i386 \
	-z noexecstack \
	-nostdlib \
	-static \
	--no-dynamic-linker \
	-z text \
	-z max-page-size=0x1000 \
	-T $(CONFIG)/link.ld

# source files
C_SRC := $(shell find . -type f -name "*.c" ! -name "liballoc.c -printf "%P\n"")
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
	@$(CP) $(CONFIG)/background.png $(ISO_DIR)/boot/grub
	@$(CP) $(CATK_ROOT)/initrd.tar $(ISO_DIR)/boot/initrd.img
	@grub-mkrescue -o $(OUT)/$(TARGET_ISO) $(ISO_DIR)
else
	@printf "[ Build ISO skipped ]\n"
endif

$(TARGET_TMP): $(ASM_OBJ) $(C_OBJ)
	@$(LD) $(LD_FLAGS) -o $(TARGET_TMP) -r $^
	@printf "[ Generating kernel symbols... ]\n"
	@bash $(TOOLS)/gen_syms.sh $(TARGET_TMP) $(SRC)/symlist.c
	@$(CC) -c symlist.c -o symlist.o $(CC_FLAGS)

$(TARGET): $(TARGET_TMP)
	@printf "[ Linking kernel.. ]\n"
	@$(LD) $(LD_FLAGS) -o $(TARGET) $(TARGET_TMP) symlist.o
	@printf "[ Validating Multiboot2 signature... ]\n"
ifneq ($(SKIP_MB_CHECK), 1)
	@grub-file --is-x86-multiboot2 $(TARGET)
else
	@printf "[ Multiboot2 check skipped ]\n"
endif

#mem/liballoc.o: mem/liballoc.c
#	@$(CC) -c $< -o $@ $(CC_FLAGS) -DDEBUG=1

%.o: %.c
	@printf "[ $< ]\n"
	@$(CC) -c $< -o $@ $(CC_FLAGS)

%.o: %.asm
	@printf "[ $< ]\n"
	@$(ASM) $(ASM_FLAGS) $< -o $@


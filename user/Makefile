TARGET = init

SRC = $(CATK_ROOT)/user

C_INCLUDE_PATH := $(SRC)/include

CC_FLAGS = \
	-I $(C_INCLUDE_PATH)	\
	-ffreestanding \
	-m32 \
	-Werror=implicit-function-declaration \
	-nodefaultlibs \
	-fPIE \
	-nostdlib \
	-fno-stack-protector

ASM_FLAGS = -f elf32
LD_FLAGS = -m elf_i386 -T $(SRC)/link.ld -znoexecstack

# source files
C_SRC := $(shell find . -type f -name "*.c")
ASM_SRC := $(shell find . -type f -name "*.asm")
# object files
C_OBJ := $(patsubst %.c, %.o, $(C_SRC))
ASM_OBJ := $(patsubst %.asm, %.o, $(ASM_SRC))

all: $(TARGET)

$(TARGET): $(C_OBJ) $(ASM_OBJ)
	@$(LD) $(LD_FLAGS) -o $(TARGET) $^

%.o: %.asm
	@printf "[ $< ]\n"
	@$(ASM) $(ASM_FLAGS) $< -o $@

%.o: %.c
	@printf "[ $< ]\n"
	@gcc -c $< -o $@ $(CC_FLAGS)

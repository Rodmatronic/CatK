TARGET = init

SRC = $(CATK_ROOT)/user

CC_FLAGS = \
	-ffreestanding \
	--target=i686-pc-none-elf \
	-march=i686 \
	-Werror=implicit-function-declaration \
	-nodefaultlibs \
	-nostdlib \

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
	@printf "[ Building Init.. ]\n"
	@$(LD) $(LD_FLAGS) -o $(TARGET) $^

%.o: %.asm
	@printf "[ $< ]\n"
	@$(ASM) $(ASM_FLAGS) $< -o $@

%.o: %.c
	@printf "[ $< ]\n"
	@$(CC) -c $< -o $@ $(CC_FLAGS)
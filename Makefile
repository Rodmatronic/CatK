export MKDIR = mkdir -p
export CP = cp -f
export RM = rm
export RM_FORCE = rm -rf

# assembler
export ASM = nasm
export CC = clang # We Only Support Clang.
export LD = ld.lld # We Only Support GNU ld or LLVM ld.lld.

export CATK_ROOT = $(CURDIR)
export CONFIG = $(CATK_ROOT)/config
export OUT = $(CATK_ROOT)/out
export OBJ = $(CATK_ROOT)/obj
.PHONY: all

$(shell $(MKDIR) $(OBJ) $(OUT))

all:
	@$(MAKE) -C src/kernel || { echo "Kernel build failed"; exit 1; }
	@$(MAKE) -C src/utils || { echo "Utils build failed"; exit 1; }
	@echo "Build successful"

clean:
	$(RM_FORCE) $(OBJ)
	$(RM_FORCE) $(OUT)

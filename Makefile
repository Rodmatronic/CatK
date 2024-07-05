export MKDIR = mkdir -p
export CP = cp -f
export RM = rm
export RM_FORCE = rm -rf

# assembler
export ASM = nasm
export CC = clang
export LD = ld

export CATK_ROOT = $(CURDIR)
export CONFIG = $(CATK_ROOT)/config
export OUT = $(CATK_ROOT)/out
export OBJ = $(CATK_ROOT)/obj
.PHONY: all

$(shell $(MKDIR) $(OBJ) $(OUT))

all:
	@$(MAKE) -C src || { echo "Build failed"; exit 1; }
	@echo "Build successful"
# 		-icount 4,align=on \
#       For debugging

run:
	@qemu-system-x86_64 \
		-cpu host \
		-enable-kvm \
		-drive format=raw,file=$(CATK_ROOT)/disk-ext2.img \
		-cdrom $(OUT)/catkernel.iso \
		-m 16G

clean:
	@$(RM_FORCE) $(OBJ)
	@$(RM_FORCE) $(OUT)
	@$(RM_FORCE) $(shell find . -type f -name "*.o")

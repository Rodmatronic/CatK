export MKDIR = mkdir -p
export CP = cp -f
export RM = rm
export RM_FORCE = rm -rf

# assembler
export ASM = nasm
export CC = clang
export LD = ld

export CATK_ROOT = $(CURDIR)
export GZ = $(shell which gzip)
export CONFIG = $(CATK_ROOT)/config
export OUT = $(CATK_ROOT)/out
export OBJ = $(CATK_ROOT)/obj

.PHONY: all

$(shell $(MKDIR) $(OBJ) $(OUT))

# The reason as to why the directories should be cleaned
# is because the configuration file could be changed any time..
# If an option in the configuration file were to change, all object files
# would not update with the new configuration. A very simple workaround
# is to recompile all files with the new configurations.
all: clean
	@$(MAKE) -C $(CATK_ROOT)/tools/gen_config || { echo "Build failed"; exit 1; }
# generate c header file
	@$(CATK_ROOT)/tools/gen_config/gen_config $(CONFIG)/config.catk | tee $(CATK_ROOT)/src/include/config.h
# generate nasm include file
	@$(CATK_ROOT)/tools/gen_config/gen_config $(CONFIG)/config.catk -n | tee $(CATK_ROOT)/src/include/asm/config.inc
	@$(MAKE) -C $(CATK_ROOT)/src 	|| { echo "Build failed"; exit 1; }
	@echo "Build successful"
# 		-icount 4,align=on \
#       For debugging

run:
	@qemu-system-x86_64 \
		-d int \
		-drive format=raw,file=$(CATK_ROOT)/disk-ext2.img \
		-cdrom $(OUT)/catkernel.iso \
		-m 2G

clean:
	@$(RM_FORCE) $(OBJ)
	@$(RM_FORCE) $(OUT)
	@$(RM_FORCE) $(shell find . -type f -name "*.o")

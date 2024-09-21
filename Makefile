VERSION = 0
PATCH_LEVEL = 1

export MKDIR = mkdir -p
export CP = cp -f
export RM = rm
export RM_FORCE = rm -rf
export MOVE = mv

# assembler
export ASM = clang
export CC = clang
export LD = ld

export CATK_ROOT = $(CURDIR)
export UTILS = $(CATK_ROOT)/utils
export USER = $(CATK_ROOT)/user
export GZ = $(shell which gzip)
export CONFIG = $(CATK_ROOT)/config
export OUT = $(CATK_ROOT)/target

$(shell $(MKDIR) $(OUT))

.PHONY: all config

# The reason as to why the directories should be cleaned
# is because the configuration file could be changed any time..
# If an option in the configuration file were to change, all object files
# would not update with the new configuration. A very simple workaround
# is to recompile all files with the new configurations.
all: clean
	@sed -i.bak '/CATK_BUILD_DATE/d' $(CONFIG)/config.catk
	@sed -i.bak '/CATK_COMPILED_WITH/d' $(CONFIG)/config.catk
	@sed -i.bak '/CATK_VERSION_STRING/d' $(CONFIG)/config.catk
	@echo 'CATK_COMPILED_WITH="$(CC)"' >> $(CONFIG)/config.catk
	@echo 'CATK_VERSION_STRING="$(VERSION).$(PATCH_LEVEL)"' >> $(CONFIG)/config.catk
	@echo -n 'CATK_BUILD_DATE="' >> $(CONFIG)/config.catk
	@date +"%a %d %b %Y %T %Z" | tr -d '\n' >> $(CONFIG)/config.catk
	@echo '"' >> $(CONFIG)/config.catk

	@$(MAKE) -C $(UTILS)/gen_config/ || { echo "Build failed"; exit 1; }
	@$(UTILS)/gen_config/gen_config $(CONFIG)/config.catk | tee $(CATK_ROOT)/src/include/config.h
	@$(MAKE) -C $(CATK_ROOT)/src     || { echo "Build failed"; exit 1; }
	@printf "Build successful"
#         -icount 6,align=on \
#       For debugging

init:
	@$(MAKE) -C $(USER)/ || { echo "Build failed"; exit 1; }

debug:
	@qemu-system-x86_64 \
		-d int \
		-cdrom $(OUT)/catkernel.iso \
		-m 2G \
		-no-reboot

disk:
	@bash $(UTILS)/make_ext2.sh $(CATK_ROOT)/skeleton disk-ext2.img

config:
	@bash $(UTILS)/config.sh

# use this for pulse-audio 	-audiodev pa,id=snd0 -machine pcspk-audiodev=snd0 \
# use this for alsa 				-audiodev alsa,id=snd0 -machine pcspk-audiodev=snd0 \

run:
	@qemu-system-x86_64 \
		-cpu host \
		-enable-kvm \
		-cdrom $(OUT)/catkernel.iso \
		-m 2G \
		-debugcon stdio

clean:
	@$(RM_FORCE) $(OUT)
	@$(RM_FORCE) $(CATK_ROOT)/src/symlist.c
	@$(RM_FORCE) $(shell find . -type f -name "*.o")

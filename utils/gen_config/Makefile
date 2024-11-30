objout = gen_config
cc_flags = -I include

# compilers / assemblers
export cc = g++

# source files
c_src := $(shell find . -type f -name "*.cpp")

all: $(c_src)
	@$(cc) $(cc_flags) -D IS_CATK -o $(objout) $^


#EE_ERL = libfiler.erl
EE_SHARED = libfiler.so

ERL_FLAGS = -nodefaultlibs -nostdlib -nostartfiles -fno-builtin -fvisibility=hidden

EE_CFLAGS += $(ERL_FLAGS)
EE_CXXFLAGS += $(ERL_FLAGS)
EE_LDFLAGS += $(ERL_FLAGS) -Wl,-soname,$(EE_SHARED)

all: $(EE_SHARED)
	$(EE_STRIP) --strip-unneeded $(EE_SHARED)
clean: 
	rm -rf $(EE_OBJ) $(EE_SHARED)

include ../Makefile.project
EE_BIN = runtime.elf

EE_INCS = -I$(PS2LUA_SRC)/dl/ee/include -I$(PS2LUA_SRC)/filer/ee/include
EE_LIBS = -L$(PS2LUA_SRC)/dl/ -ldl\
		-ldebug

EE_WARNFLAGS += -Wall 

all: $(EE_BIN)
#	$(EE_STRIP) $(EE_BIN)
clean: 
	rm -rf $(EE_OBJ) $(EE_BIN)

include ../Makefile.project
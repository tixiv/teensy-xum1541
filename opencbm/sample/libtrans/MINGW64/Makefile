RELATIVEPATH=../../
include ${RELATIVEPATH}MINGW64/config.make

CFLAGS     := $(subst ../,../../,$(CFLAGS))
LINK_FLAGS := -L$(RELATIVEPATH)/libtrans -ltrans $(subst ../,../../,$(LINK_FLAGS))

OBJS = main.o
PROG = samplelibtransf

include ${RELATIVEPATH}MINGW64/prgrules.make

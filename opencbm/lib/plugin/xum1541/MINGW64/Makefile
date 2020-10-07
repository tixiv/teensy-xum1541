RELATIVEPATH=../../../
include ${RELATIVEPATH}MINGW64/config.make

.PHONY: all clean mrproper install uninstall install-files

PLUGIN_NAME = xum1541
LIBNAME = opencbm-${PLUGIN_NAME}
SRCS    = archlib.c xum1541.c s1_s2_pp.c parburst.c
LIBS    = -L$(RELATIVEPATH)/libmisc -lmisc
LIBS   += $(LIBUSB_LIBS)

CFLAGS += $(LIBUSB_CFLAGS)
CFLAGS += -I$(RELATIVEPATH)/include/LINUX/ -I$(RELATIVEPATH)/include/ -I../../ -I $(XUM1541DIR) -I$(RELATIVEPATH)/libmisc -I$(RELATIVEPATH)/libmisc/MINGW64
#LDFLAGS =

all: build-lib

clean: clean-lib

mrproper: clean

install-files: install-plugin

install: install-files

uninstall: uninstall-plugin

include ../../../MINGW64/dllrules.make

### dependencies:

archlib.o archlib.lo: ../../archlib.h xum1541.h
xum1541.o xum1541.lo: xum1541.h

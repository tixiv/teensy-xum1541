# get package version (major.minor.release).

VERSIONH:=$(RELATIVEPATH)include/version.h

OPENCBM_MAJOR   :=$(shell grep "\#define OPENCBM_VERSION_MAJOR"      ${VERSIONH}|sed -e "s/\#define OPENCBM_VERSION_[^ ]*[ ]*//")
OPENCBM_MINOR   :=$(shell grep "\#define OPENCBM_VERSION_MINOR"      ${VERSIONH}|sed -e "s/\#define OPENCBM_VERSION_[^ ]*[ ]*//")
OPENCBM_RELEASE :=$(shell grep "\#define OPENCBM_VERSION_SUBMINOR"   ${VERSIONH}|sed -e "s/\#define OPENCBM_VERSION_[^ ]*[ ]*//")
OPENCBM_PATCHLVL:=$(shell grep "\#define OPENCBM_VERSION_PATCHLEVEL" ${VERSIONH}|sed -e "s/\#define OPENCBM_VERSION_[^ ]*[ ]*//")
OPENCBM_DEVEL   :=$(shell grep "\#define OPENCBM_VERSION_DEVEL"      ${VERSIONH}|sed -e "s/\#define OPENCBM_VERSION_[^ ]*[ ]*//")

MAJ:=${OPENCBM_MAJOR}
MIN:=${OPENCBM_MINOR}

ifeq ($(OPENCBM_DEVEL), 0)
	REL:=${OPENCBM_RELEASE}
else
	REL:=${OPENCBM_RELEASE}.${OPENCBM_DEVEL}
endif

# choose your crossassembler (if you have one).
# mandatory if you want to hack any of the 6502 sources.
#
#XASS        = xa
XASS        = cl65


#
# Default destination directories
#
PREFIX      = /usr/local
ETCDIR      = C:\\\\Windows\\\\System32
BINDIR      = $(PREFIX)/bin
LIBDIR      = $(PREFIX)/lib
MANDIR      = $(PREFIX)/man/man1
INFODIR     = $(PREFIX)/info
INCDIR      = $(PREFIX)/include
MODDIR      = ${shell for d in /lib/modules/`uname -r`/extra /lib/modules/`uname -r`/misc /lib/modules/`uname -r`/kernel/drivers/char; do test -d $$d && echo $$d; done | head -n 1}
PLUGINDIR   = $(PREFIX)/lib/opencbm/plugin/
UDEV_RULES  = /etc/udev/rules.d/

#
# Where to find the xum1541 and xu1541 firmware
#
XU1541DIR   = $(RELATIVEPATH)/../xu1541
XUM1541DIR  = $(RELATIVEPATH)/../xum1541

#
# Where to find libusb (libusb.sf.net)
#
#LIBUSB_CFLAGS  = -I/usr/include
#LIBUSB_LDFLAGS =
#LIBUSB_LIBS    = -L/usr/lib -lusb

HAVE_LIBUSB0 = ${shell pkg-config libusb && echo 1} 
HAVE_LIBUSB1 = ${shell pkg-config libusb-1.0 && echo 1} 

ifneq ($(strip $(HAVE_LIBUSB0)),)
  HAVE_LIBUSB=1
  LIBUSB_CFLAGS=-DHAVE_LIBUSB=1 -DHAVE_LIBUSB0=1 $(shell pkg-config --cflags libusb)
  LIBUSB_LDFLAGS=
  LIBUSB_LIBS=$(shell pkg-config --libs libusb)
endif

ifneq ($(strip $(HAVE_LIBUSB1)),)
  HAVE_LIBUSB=1
  LIBUSB_CFLAGS=-DHAVE_LIBUSB=1 -DHAVE_LIBUSB1=1 -DHAVE_LIBUSB_1_0=1 $(shell pkg-config --cflags libusb-1.0)
  LIBUSB_LDFLAGS=
  LIBUSB_LIBS=$(shell pkg-config --libs libusb-1.0)
endif


#
# define os name
#
OS = $(shell uname -s)

#
# compiler/linker flags. Should be ok.
#
OS_ARCH     = linux

CFLAGS       = -O2 -Wall -I../include -I../include/LINUX -DPREFIX=\"$(PREFIX)\" -DOPENCBM_CONFIG_FILE=\"$(OPENCBM_CONFIG_FILE)\"
CFLAGS      += -DMINGW64
CFLAGS      += $(USER_CFLAGS)

LIB_CFLAGS   = $(CFLAGS) -D_REENTRANT
SHLIB_CFLAGS = $(LIB_CFLAGS) -fPIC
SHLIB_EXT    = so
SHLIB_SWITCH = -shared
LINK_FLAGS   = -L../lib -L../arch/$(OS_ARCH) -L../libmisc -lopencbm -larch -lmisc
SONAME       = -Wl,-soname -Wl,
CC           = gcc
AR           = ar
LDCONFIG     = /sbin/ldconfig
OD_FLAGS     = -w8 -txC -v -An


OPENCBM_CONFIG_PATH = $(ETCDIR)

OPENCBM_CONFIG_FILE = $(OPENCBM_CONFIG_PATH)\\\\opencbm.conf

#
# common compile flags
#
.SUFFIXES: .a65 .o65 .inc .lo

.c.lo:
	$(CC) $(SHLIB_CFLAGS) -c -o $@ $<

.o65.inc:
	test -s $< && od $(OD_FLAGS) $< | \
	sed 's/\([0-9a-f]\{2\}\) */0x\1,/g; $$s/,$$//' > $@


#
# cross assembler definitions.
# patches for other assemblers are welcome.
#

# xa defs
XA          = xa

# cl65 defs, contributed by Ullrich von Bassewitz <uz(at)musoftware(dot)de>
# (cc65 >= 2.6 required)
CL65        = cl65
LD65        = ld65
CA65_FLAGS  = --feature labels_without_colons --feature pc_assignment --feature loose_char_term --asm-include-dir ..


#
# suffix rules
#
.a65.o65:
ifeq ($(XASS),xa)
	$(XA) $< -o $@
else
ifeq ($(XASS),cl65)
	$(CL65) -c $(CA65_FLAGS) -o $*.tmp $<
	$(LD65) -o $@ --target none $*.tmp && rm -f $*.tmp
else
	@echo "*** Error: No crossassembler defined. Check config.make" 2>&1
	exit 1
endif
endif

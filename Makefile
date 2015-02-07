SRCS=lense.c

TARGET=lense

# Set flags to the compiler and linker
CFLAGS += -O0 -g -Wall `$(PKG-CONFIG) --cflags gtk+-2.0 gstreamer-0.10` $(ARCH_CFLAGS)
LDFLAGS += `$(PKG-CONFIG) --libs gstreamer-0.10 gtk+-2.0` -lm

##############################################################################
# Setup your build environment
##############################################################################

# Set the path to the target libraries, i.e. the images rootfs
# Set the prefix for the cross compiler
OECORE_NATIVE_SYSROOT ?= /home/tej/oe-core-test/build/out-eglibc/sysroots/x86_64-linux/
OECORE_TARGET_SYSROOT ?= /home/tej/oe-core-test/build/out-eglibc/sysroots/colibri-t20/
CROSS_COMPILE ?= $(OECORE_NATIVE_SYSROOT)/usr/bin/armv7at2hf-vfp-angstrom-linux-gnueabi/arm-angstrom-linux-gnueabi-

ifneq ($(strip $(CROSS_COMPILE)),)
  LDFLAGS += -L$(OECORE_TARGET_SYSROOT)usr/lib -Wl,-rpath-link,$(OECORE_TARGET_SYSROOT)usr/lib -L$(OECORE_TARGET_SYSROOT)lib -Wl,-rpath-link,$(OECORE_TARGET_SYSROOT)lib
  #ARCH_CFLAGS = -march=armv7-a -fno-tree-vectorize -mthumb-interwork -mfloat-abi=hard -mtune=cortex-a9
  ARCH_FLAGS += -mfpu=neon -mtune=cortex-a5
  BIN_POSTFIX =
  PKG-CONFIG = export PKG_CONFIG_SYSROOT_DIR=$(OECORE_TARGET_SYSROOT); \
               export PKG_CONFIG_PATH=$(OECORE_TARGET_SYSROOT)/usr/lib/pkgconfig/; \
               $(OECORE_NATIVE_SYSROOT)usr/bin/pkg-config

  #LDFLAGS += -Wl,--allow-shlib-undefined
  # This configuration uses the buildsystems headers, don't emit warnings about that
  #ARCH_CFLAGS += -Wno-poison-system-directories
else
# Native compile
  PKG-CONFIG = pkg-config
  ARCH_CFLAGS =
# Append .x86 to the object files and binaries, so that native and cross builds can live side by side
  BIN_POSTFIX = .x86
endif

# Toolchain binaries
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)gcc
STRIP = $(CROSS_COMPILE)strip
RM = rm -f

OBJS = $(SRCS:.c=$(BIN_POSTFIX).o)
DEPS = $(OBJS:.o=.o.d)

# pull in dependency info for *existing* .o files
-include $(DEPS)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS) $(LDFLAGS)
	#$(STRIP) $@ 

%$(BIN_POSTFIX).o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<
	$(CC) -MM $(CFLAGS) $< > $@.d

clean:
	$(RM) $(OBJS) $(PROG) $(DEPS)

.PHONY: all clean


#!/bin/sh



OECORE_NATIVE_SYSROOT=/home/ghansham/tej/oe-core-test/build/out-eglibc/sysroots/x86_64-linux/
OECORE_TARGET_SYSROOT=/home/ghansham/tej/oe-core-test/build/out-eglibc/sysroots/colibri-t20/
export PATH=${OECORE_NATIVE_SYSROOT}usr/bin/armv7at2hf-vfp-angstrom-linux-gnueabi:$PATH
#LDFLAGS="-L${OECORE_TARGET_SYSROOT}usr/lib -Wl,-rpath-link,${OECORE_TARGET_SYSROOT}usr/lib -L${OECORE_TARGET_SYSROOT}lib -Wl,-rpath-link,${OECORE_TARGET_SYSROOT}lib -Wl,-rpath-link,${OECORE_TARGET_SYSROOT}usr/lib/gstreamer-0.10"
CROSS_COMPILE=arm-angstrom-linux-gnueabi
export LIBTOOL=${CROSS_COMPILE}-libtool
make distclean
./configure --prefix=/home/ghansham/defense/usr/ \
          --host=${CROSS_COMPILE} \
          --with-sysroot=${OECORE_TARGET_SYSROOT} \
          CC=${CROSS_COMPILE}-gcc \
          CFLAGS="-I${OECORE_TARGET_SYSROOT}usr/include \
          -I${OECORE_TARGET_SYSROOT}usr/lib/glib-2.0/include \
          -I${OECORE_TARGET_SYSROOT}usr/include/libxml2/ \
          -I${OECORE_TARGET_SYSROOT}usr/include/glib-2.0" \
          LDFLAGS="-Wl,-rpath-link,-L${OECORE_TARGET_SYSROOT}lib \
          -Wl,-rpath-link,-L${OECORE_TARGET_SYSROOT}usr/lib" \
          PKG_CONFIG_PATH=${OECORE_TARGET_SYSROOT}usr/lib/ \
          PKG_CONFIG_LIBDIR=${OECORE_TARGET_SYSROOT}usr/lib/ \
          GST_LIBS=-L${OECORE_TARGET_SYSROOT}usr/lib/gstreamer-0.10 \
          GST_CFLAGS=-I${OECORE_TARGET_SYSROOT}usr/include/gstreamer-0.10 \


make clean && make

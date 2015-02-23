#!/bin/sh

OECORE_NATIVE_SYSROOT=/home/ghansham/tej/oe-core-test/build/out-eglibc/sysroots/x86_64-linux/
OECORE_TARGET_SYSROOT=/home/ghansham/tej/oe-core-test/build/out-eglibc/sysroots/colibri-t20/
export PATH=${OECORE_NATIVE_SYSROOT}usr/bin/armv7at2hf-vfp-angstrom-linux-gnueabi:$PATH
CROSS_COMPILE=arm-angstrom-linux-gnueabi

make distclean
./configure --prefix=/home/ghansham/defense/usr/ \
          --host=${CROSS_COMPILE} \
          --with-sysroot=${OECORE_TARGET_SYSROOT} \
          PKG_CONFIG=${CROSS_COMPILE}-pkg-config-sysroot \

make clean && make

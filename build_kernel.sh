#!/bin/bash

###############################################################################
# To all DEV around the world :)                                              #
# to build this kernel you need to be ROOT and to have bash as script loader  #
###############################################################################

echo "Clear Folder"
make clean
echo "make config"
make trelte_00_defconfig
echo "build kernel"
make -j4

cp -f arch/arm/boot/zImage build_kernel/AIK-Linux/split_img/boot.img-zImage 

build_kernel/AIK-Linux/repackimg.sh

rm -f build_kernel/out/*.zip

cp -f build_kernel/AIK-Linux/image-new.img build_kernel/out/boot.img

cd build_kernel/out/

zip -r SpaceLemon.zip ./


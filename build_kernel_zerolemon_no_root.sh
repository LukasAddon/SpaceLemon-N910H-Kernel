#!/bin/bash

###############################################################################
# To all DEV around the world :)                                              #
# to build this kernel you need to be ROOT and to have bash as script loader  #
###############################################################################

DTS=arch/arm/boot/dts
BK=build_kernel

cp ./drivers/battery/max77843_fuelgauge_ZL.c ./drivers/battery/max77843_fuelgauge.c
echo "Clear Folder"
make clean
rm -rf  include/config/*
echo "make config"
make trelte_00_defconfig
echo "build kernel"
make exynos5433-tre_eur_open_16.dtb
make ARCH=arm -j4

GETVER=`grep 'SpaceLemon-Battery-Extended-v.*' arch/arm/configs/trelte_00_defconfig | sed 's/.*-.//g' | sed 's/".*//g'`
###################################### DT.IMG GENERATION #####################################
echo -n "Build dt.img......................................."

./tools/dtbtool -o ./dt.img -v -s 2048 -p ./scripts/dtc/ $DTS/ 
# get rid of the temps in dts directory
rm -rf $DTS/.*.tmp
rm -rf $DTS/.*.cmd
rm -rf $DTS/*.dtb

# Calculate DTS size for all images and display on terminal output
du -k "./dt.img" | cut -f1 >sizT
sizT=$(head -n 1 sizT)
rm -rf sizT
echo "$sizT Kb"

cp -f arch/arm/boot/zImage build_kernel/AIK-Linux-no-root/split_img/boot.img-zImage 
cp -f ./dt.img build_kernel/AIK-Linux-no-root/split_img/boot.img-dtb

build_kernel/AIK-Linux-no-root/repackimg.sh

rm -f build_kernel/out-no-root/*.zip

cp -f build_kernel/AIK-Linux-no-root/image-new.img build_kernel/out-no-root/boot.img

cd build_kernel/out-no-root/

mkdir system
mkdir data

zip -r SpaceLemon_v${GETVER}_zerolemon_no_root.zip ./


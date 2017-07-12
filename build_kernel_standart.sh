#!/bin/bash

###############################################################################
# To all DEV around the world :)                                              #
# to build this kernel you need to be ROOT and to have bash as script loader  #
###############################################################################

DTS=arch/arm/boot/dts
BK=build_kernel

cp ./drivers/battery/max77843_fuelgauge_ST.c ./drivers/battery/max77843_fuelgauge.c
echo "Clear Folder"
make clean
echo "make config"
make exynos5433-tre3caltelgt_defconfig
echo "build kernel"
make exynos5433-tre3calte_kor_open_05.dtb
make exynos5433-tre3calte_kor_open_14.dtb
make ARCH=arm -j4


GETVER=`grep 'SpaceLemon-Battery-Standart-v.*' arch/arm/configs/exynos5433-tre3caltelgt_defconfig | sed 's/.*-.//g' | sed 's/".*//g'`
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

cp -f arch/arm/boot/zImage build_kernel/AiK-N916S/split_img/boot.img-zImage
cp -f ./dt.img build_kernel/AiK-N916S/split_img/boot.img-dtb

build_kernel/AiK-N916S/repackimg.sh

rm -f build_kernel/out/*.zip

cp -f build_kernel/AiK-N916S/image-new.img build_kernel/out/boot.img

cd build_kernel/out/

mkdir system
mkdir data

zip -r N916S_K_L_SpaceLemon_v${GETVER}_standart_battery.zip ./


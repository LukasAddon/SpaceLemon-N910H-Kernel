#!/bin/bash

export ARCH=arm
#export CROSS_COMPILE=../PLATFORM/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8/bin/arm-eabi-

make exynos5433-tre3calteskt_defconfig

make -j1

################################################################################



1. How to Build

	- get Toolchain

		From android git server , codesourcery and etc ..

		 - arm-eabi-4.8

		

	- edit build_kernel.sh

		edit "CROSS_COMPILE" to set proper toolchain.

		  EX) export CROSS_COMPILE=$(android platform directory)/android/prebuilts/gcc/linux-x86/arm/arm-eabi-4.8/bin/arm-eabi-



	- run defconfig

	  $ make trelte_00_defconfig

	  $ make -j64



2. Output files

	- Kernel : arch/arm/boot/Image

	- module : modules/*.ko



3. How to Clean	

	  $ make clean



################################################################################

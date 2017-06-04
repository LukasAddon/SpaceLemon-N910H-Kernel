################################################################################



1. How to Build

	- get Toolchain

		From android git server , codesourcery and etc ..

		 - arm-eabi-5.2

		

	- edit MakeFile

        line 197
                    CROSS_COMPILE	?=../arm-eabi-5.2/bin/arm-eabi-


    - run build_kernel_***.sh
          N910H/C
                build_kernel_standart.sh  - standart battery with root
                build_kernel_standart_no_root.sh - standart battery
                build_kernel_zerolemon.sh  - zerolemon battery with root
                build_kernel_zerolemon_no_root.sh - zerolemon battery
          N910S
                build_kernel_standart_N910S.sh
                build_kernel_zerolemon_N910S.sh
          N910K
                build_kernel_standart_N910K.sh

2. Output files

	- Kernel zip for TWRP : build_kernel/out and build_kernel/out-no-root




3. How to Clean	

	  $ make clean



################################################################################

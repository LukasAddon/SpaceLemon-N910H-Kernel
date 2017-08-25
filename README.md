################################################################################



1. How to Build

	- get Toolchain

		From android git server , codesourcery and etc ..

		 - Linaro 5.2

    - install CCACHE to cache files when compiling
		

	- edit MakeFile

        line 197
	
                    CROSS_COMPILE	?=../arm-eabi-5.2/bin/arm-eabi-


    - run build_kernel_***.sh
    
          N910H/C
	  
                build_kernel_standart.sh  - standart battery
		
                build_kernel_zerolemon.sh  - zerolemon battery
		
          N910S/K/L
	  
                build_kernel_standart_N910S_K_L.sh
		
                build_kernel_zerolemon_N910S_K_L.sh

          N915S/K/L

                build_kernel_standart_N915S_K_L.sh

                build_kernel_zerolemon_N915S_K_L.sh

          N915U

                build_kernel_standart_N910U.sh

                build_kernel_zerolemon_N910U.sh

2. Output files

	- Kernel zip for TWRP : build_kernel/out and build_kernel/out-no-root




3. How to Clean	

	  $ make clean



################################################################################

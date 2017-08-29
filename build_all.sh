#!/bin/bash

#create catalog for release
mkdir build_kernel/release

#clear
./clear_script.sh

# start compile kernels for zerolemon

./build_kernel_zerolemon.sh
./clear_script.sh
./build_kernel_zerolemon_N910S_K_L.sh
./clear_script.sh
./build_kernel_zerolemon_N910U.sh
./clear_script.sh
./build_kernel_zerolemon_N915S-K-L.sh
./clear_script.sh

# build  standart battery kernels

./build_kernel_standart.sh
./clear_script.sh
./build_kernel_standart_N910S_K_L.sh
./clear_script.sh
./build_kernel_standart_N910U.sh
./clear_script.sh
./build_kernel_standart_N915S-K-L.sh

#clear
./clear_script.sh
make clean
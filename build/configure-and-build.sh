#!/bin/bash

export BLOG="build.out"
export FW="/opt/Xilinx/2024.2/Vivado/2024.2/data/ip/xilinx/cms_subsystem_v4_0/fw"

echo "./build.sh -j 4 -opt -nohip -disable-werror -driver -ertfw ${FW} -verbose"
./build.sh -j 4 -opt -nohip -disable-werror -driver -ertfw ${FW} -verbose > ${BLOG} 2>&1


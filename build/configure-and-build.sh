#!/bin/bash

export BLOG="build.out"
export FW=""

if [ "x${FW}" == "x" ] ; then
  echo "`basename $0`: Xilinx firmware directory is empty. Please set it to a valid path."
  exit 1
fi

echo "./build.sh -j 4 -opt -nohip -disable-werror -driver -ertfw ${FW} -verbose"
./build.sh -j 4 -opt -nohip -disable-werror -driver -ertfw ${FW} -verbose > ${BLOG} 2>&1


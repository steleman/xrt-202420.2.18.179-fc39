#!/bin/bash

if [ `id -u` -ne 0 ] ; then
  echo "You must be root to do this."
  exit 1
fi

for file in \
  xrt \
  xrt-container \
  xrt-xbflash \
  xrt-xbflash2
do
  echo "rpm erasing ${file} ..."
  rpm -e --nodeps ${file}
done


#!/bin/bash

if [ `id -u` -ne 0 ] ; then
  echo "You must be root to do this."
  exit 1
fi

for file in \
  xrt_202410.2.17.0_39-x86_64-xrt.rpm \
  xrt_202410.2.17.0_39-x86_64-container.rpm \
  xrt_202410.2.17.0_39-x86_64-xbflash.rpm \
  xrt_202410.2.17.0_39-x86_64-xbflash2.rpm
do
  dnf reinstall -y ./${file}
done


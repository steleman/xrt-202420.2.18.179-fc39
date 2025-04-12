AMD / Xilinx xrt-202420.2.18.179
=====================================

This is a fork and port of Xilinx XRT Version 202420.2.18.179 to Fedora 39 `x86_64` with Linux Kernel 6.11.9. It also builds and works correctly with Linux Kernel 6.13.9 also on Fedora 39. It is based on a Github fork of the AMD / Xilinx original at [XRT Github](https://github.com/Xilinx/XRT).

This port corrects a number of bugs in XRT. Most importantly, the file [src/include/1_2/CL/cl_ext.h](https://github.com/steleman/xrt-202420.2.18.179-fc39/src/include/1_2/CL/cl_ext.h) has been renamed to [src/include/1_2/CL/cl_ext_xocl.h](https://github.com/steleman/xrt-202420.2.18.179-fc39/src/include/1_2/CL/cl_ext_xocl.h). The original name created conflicts with normative public interfaces from the OpenCL Standard - namely the header file `cl_ext.h`. That, in turn, created a huge compile time mess. The mess has been cleaned up and everything compiles and builds cleanly.

There is a large number of other patches. Details are in the patch file - see below. The patch applies with `-p0` from the toplevel directory.

The original README file has been renamed to [README.XRT](https://github.com/steleman/xrt-202420.2.18.179-fc39/README.XRT.rst) here.

The full set of patches can be found in the [patches-fc39](https://github.com/steleman/xrt-202420.2.18.179-fc39/patches-fc39/) directory.

How to configure and build:
----------------------------------

1. Clone this repo.
2. cd `build`.
3. Run the script `./configure-and-build.sh` from that directory.

You do not need to clone this repo with `git submodule update --init --recursive`. Everything needed is already included.

You may need to install some XRT build-time dependencies. RapidJSON is one of them.

Other versions of Fedora, or other Linux Distros:
----------------------------------------------------------
This might very well work on other (higher than 39) versions of Fedora, and on other recent Linux Distros. I just don't have the infrastructure available to test it on other Linux distros.

I have not tested on Windows.


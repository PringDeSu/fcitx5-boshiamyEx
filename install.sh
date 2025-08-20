#!/bin/bash
clear
cd $(dirname $0)
mkdir -p build
cd build
if [ $# -eq 1 ]; then
    cmake .. -DCMAKE_INSTALL_PREFIX=/usr # use Debug for easy debugging with gdb
fi
make && sudo make install && fcitx5 -r


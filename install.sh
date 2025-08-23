#!/bin/bash
clear
cd $(dirname $0)
mkdir -p build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/usr # use Debug for easy debugging with gdb
make && sudo make install && fcitx5 -r


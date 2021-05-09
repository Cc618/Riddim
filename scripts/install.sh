#!/bin/bash

# Installs riddim once configured

set -e

source scripts/setup_env.sh

echo
echo Building...
cd build && make -j 4
cd ..

echo
echo Installing...

# Std lib
mkdir -p "$RID_STD_PATH"
cp -r "$PWD/std" "$RID_STD_PATH"

# Binaries
mkdir -p "$RID_BIN_PATH"
cp "$PWD/build/src/riddim" "$RID_BIN_PATH"

echo
echo Installed Riddim successfully at "$RID_BIN_PATH/riddim"

#!/bin/bash

# Setup the build environment and default environment variables
# Run this from the project's root directory

set -e

source scripts/setup_env.sh

if ! [ -z "$RID_DEBUG" ]
then
    export DEBUG_FLAGS="-DCMAKE_BUILD_TYPE=Debug"
    echo Debug mode
fi

echo Configuring...
mkdir -p build && cd build && cmake .. ${DEBUG_FLAGS}
cd ..

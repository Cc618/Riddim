#!/bin/bash

# Uninstalls riddim once configured

set -e

source scripts/setup_env.sh

# Std lib
rm -rf "$RID_STD_PATH"/*

# Binaries
rm "$RID_BIN_PATH"/riddim

echo Successfully uninstalled Riddim

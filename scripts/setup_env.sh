#!/bin/bash

# File to be sourced, sets default variables

# Default variables
DEFAULT_RID_BIN_PATH="/usr/bin"
DEFAULT_RID_STD_PATH="/usr/lib/riddim/std"

# Set default variables
export RID_BIN_PATH="${RID_BIN_PATH:-$DEFAULT_RID_BIN_PATH}"
export RID_STD_PATH="${RID_STD_PATH:-$DEFAULT_RID_STD_PATH}"

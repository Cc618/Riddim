#!/bin/bash

# Script that generates Riddim's Standard Library documentation
# Riddim must be built (or installed)

set -e

# Output directory (will be removed and recreated)
export OUT=docs/std

# Where to create the autodoc script
export SCRIPT=scripts/autodoc.rid

# Update this line to "riddim" if installed system-wide
export RIDDIM=build/src/riddim

rm -rf "$OUT"
mkdir "$OUT"

echo "autodoc_std('$OUT')" > "$SCRIPT"

"$RIDDIM" "$SCRIPT"

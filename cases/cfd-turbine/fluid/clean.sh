#!/usr/bin/env bash

# Source tutorial clean functions
. $WM_PROJECT_DIR/bin/tools/CleanFunctions

rm -rf 0 > /dev/null 2>&1

cleanCase
rm -r ./precice-profiling
rm -r ../precice-run
# ----------------------------------------------------------------- end-of-file

#!/usr/bin/env bash
set -e -u

# Source tutorial clean functions
. $WM_PROJECT_DIR/bin/tools/CleanFunctions

rm -rf 0 > /dev/null 2>&1

cleanCase
rm -rf ./precice-profiling
rm -rf ../precice-run
# ----------------------------------------------------------------- end-of-file

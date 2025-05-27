#! /bin/bash

# Source: https://mywiki.wooledge.org/BashFAQ/028 (last checked 19.04.25).
if [[ $BASH_SOURCE = */* ]]; then
  CDPATH= cd -- "${BASH_SOURCE%/*}/" || exit
fi

cd ../.. || exit
mkdir -p build
cd build || exit

if [ ! -f CMakeCache.txt ]; then
  cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" ..
fi

cmake --build . --config "Release"
cd ..

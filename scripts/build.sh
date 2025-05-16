#! /bin/bash
cd ~/cpp/monitor || exit
cd build || exit
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE="Release" ..
cmake --build . --config "Release"
cd ..

@echo off
REM Ensure we are in the script's directory
cd /d %~dp0

cd ../..
if not exist build (
    mkdir build
)
cd build

if not exist CMakeCache.txt (
    cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_BUILD_TYPE="Release" -DBUILD_EXAMPLES=ON ..
)

cmake --build . --config Release

cd ..

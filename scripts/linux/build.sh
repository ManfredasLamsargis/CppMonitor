#! /bin/bash

# Stop script on errors
set -euo pipefail

readonly SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)
readonly PROJECT_ROOT="$SCRIPT_DIR/../.."
readonly BUILD_DIR="$PROJECT_ROOT/build"

CLEAN_BUILD=false
BUILD_EXAMPLES=false
BUILD_TESTS=false
RUN_TESTS=false

while [ $# -gt 0 ]; do
  case $1 in
  --clean)
    CLEAN_BUILD=true
    ;;
  --build-examples)
    BUILD_EXAMPLES=true
    ;;
  --build-tests)
    BUILD_TESTS=true
    ;;
  --run-tests)
    RUN_TESTS=true
    ;;
  esac
  shift
done

mkdir -p build
cd "$BUILD_DIR" || exit

if $CLEAN_BUILD; then
  cd "$PROJECT_ROOT" && rm -rf build/*
fi

cd "$BUILD_DIR" || exit

cmake \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE="Release" \
  -DBUILD_EXAMPLES=$BUILD_EXAMPLES \
  -DBUILD_TESTS=$BUILD_TESTS \
  "$PROJECT_ROOT"

cmake --build "$BUILD_DIR" --config "Release"

if $RUN_TESTS; then
  cd "$BUILD_DIR" && ctest
fi

cd ..

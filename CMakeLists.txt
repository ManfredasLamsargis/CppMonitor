cmake_minimum_required(VERSION 3.5)

project(Monitor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/inc)

option(BUILD_EXAMPLES "Build example programs" OFF)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

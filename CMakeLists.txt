cmake_minimum_required(VERSION 3.5)

project(Monitor)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCE_FILES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

if(MSVC)
  add_compile_options(/W4)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic
                                                 -Werror)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}
                                                   ${CMAKE_SOURCE_DIR}/inc)

option(BUILD_EXAMPLES "Build example programs" OFF)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

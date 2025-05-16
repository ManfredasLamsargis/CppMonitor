cmake_minimum_required(VERSION 3.5)

project(
  monitor
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG 48bcf39a661a13be22666ac64db8a7f886f2637e # v1.15.2
)

FetchContent_MakeAvailable(spdlog)

file(GLOB SOURCE_FILES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/inc)

target_precompile_headers(${PROJECT_NAME} PRIVATE "<log.hpp>" "<pch.hpp>")

target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog
                                              $<$<BOOL:${MINGW}>:ws2_32>)
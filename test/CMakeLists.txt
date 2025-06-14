# see: https://google.github.io/googletest/quickstart-cmake.html (last checked
# on 06.14.25)
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG 52eb8108c5bdec04579160ae17225d66034bd723)

if(WIN32)
  set(gtest_force_shared_crt
      ON
      CACHE BOOL "" FORCE)
endif()

set(BUILD_GMOCK OFF) # Disable building gmock

FetchContent_MakeAvailable(googletest)

file(GLOB TEST_FILES *.cpp)

add_executable(monitor_test ${TEST_FILES})

target_link_libraries(monitor_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(monitor_test)

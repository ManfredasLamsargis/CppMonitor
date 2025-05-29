add_executable(flagged_sync
    main.cpp
)
target_include_directories(flagged_sync PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/inc
)
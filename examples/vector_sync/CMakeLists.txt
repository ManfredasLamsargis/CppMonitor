add_executable(vector_sync
    main.cpp
)
target_include_directories(vector_sync PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/inc
)
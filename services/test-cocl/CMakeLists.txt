cmake_minimum_required(VERSION 3.1)

project(test-cocl)

include_directories(
    ../../snode/fnode/include
    ../../snode/fcommon/include
)

add_executable(test-cocl main.c)
add_dependencies(test-cocl fnode)
target_link_libraries(test-cocl fnode)

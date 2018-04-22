cmake_minimum_required(VERSION 3.1)

project(test-tadc)

include_directories(
    ../../snode/fnode/include
    ../../snode/fcommon/include
)

add_executable(test-tadc main.c)
add_dependencies(test-tadc fnode)
target_link_libraries(test-tadc fnode m)

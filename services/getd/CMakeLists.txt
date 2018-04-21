cmake_minimum_required(VERSION 3.1)

project(getd)

include_directories(
    ../../snode/fnode/include
    ../../snode/fcommon/include
)

add_executable(getd main.cpp GPIOClass.cpp)
add_dependencies(getd fnode)
target_link_libraries(getd fnode)

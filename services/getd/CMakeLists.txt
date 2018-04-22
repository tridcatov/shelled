cmake_minimum_required(VERSION 3.1)

project(getd)

include_directories(
    ../../snode/fnode/include
    ../../snode/fcommon/include
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(getd main.cpp GPIOClass.cpp)
add_dependencies(getd fnode)
target_link_libraries(getd fnode Threads::Threads)

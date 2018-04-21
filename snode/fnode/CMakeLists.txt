cmake_minimum_required(VERSION 3.1)
project(fnode)

set(HEADERS
    src/service.h
    src/commands.h
)

set(SOURCES
    src/service.c
)

use_c99()

if (CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-pedantic -fPIC)
endif ()

include_directories(
    ../fcommon/include
    ../fnet/include
)

add_library(fnode STATIC ${HEADERS} ${SOURCES})
add_dependencies(fnode fnet)
target_link_libraries(fnode fnet)

add_subdirectory(test fnode-test)

# usage:
# mkdir build
# cd build
# cmake -DCMAKE_BUILD_TYPE=Release ..
# cmake -DCMAKE_BUILD_TYPE=Debug ..
# cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..
# cmake -DCMAKE_BUILD_TYPE=MinSizeRel ..
# cmake -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" .

cmake_minimum_required(VERSION 3.1)
project(shelled)



#*********************************************************
# set build type
#*********************************************************
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

#*********************************************************
# output directory
#*********************************************************
set(SHELLED_OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/output)

#*********************************************************
# common compiler flags
#*********************************************************
if (MSVC)
    add_definitions(/W4)
elseif (CMAKE_COMPILER_IS_GNUCXX)
    add_definitions (-Wall)
else ()
    message ("Unknown compiler")
endif ()

#*********************************************************
# output directories
#*********************************************************
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${SHELLED_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${SHELLED_OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${SHELLED_OUTPUT_DIR})

#*********************************************************
# useful macros
#*********************************************************
macro(use_c99)
  if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
      set (CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
    endif ()
  else ()
    set (CMAKE_C_STANDARD 99)
  endif ()
endmacro(use_c99)


add_subdirectory(snode              snode)
add_subdirectory(services/alice     alice)
add_subdirectory(services/test-cocl test-cocl)
add_subdirectory(services/test-tadc test-tadc)
add_subdirectory(services/getd      getd)

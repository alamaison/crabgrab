cmake_minimum_required(VERSION 2.6)
project(crabgrab)

include_directories(src)
include_directories(src/urdl/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows specific includes
    set(PLATFORM "win32")
    include_directories(src/win32/winapi/include)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    #Linux specific includes
    set(PLATFORM "linux")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    #Mac specific includes
    set(PLATFORM "darwin")
endif()

include_directories(src/${PLATFORM})

## Boost

set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_DETAILED_FAILURE_MSG TRUE)
set(Boost_ADDITIONAL_VERSIONS 1.46.1)
# 1.41 is first version with Boost.PropertyTree
find_package(Boost 1.41 REQUIRED COMPONENTS filesystem system thread)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

## Preprocessor

add_definitions(-DURDL_HEADER_ONLY=1)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows specific definitions
    add_definitions(
      -DWIN32_LEAN_AND_MEAN        # Makes Window.h usable
      -D_SCL_SECURE_NO_WARNINGS    # Supresses silly security warnings in STL
      -D_WIN32_WINNT=0x0501        # Windows XP minimum
      -DBOOST_FILESYSTEM_VERSION=2 # Needed by winapi
      )
endif()

## Build

add_subdirectory(src)
# TODO: Make building the tests optional
add_subdirectory(test)

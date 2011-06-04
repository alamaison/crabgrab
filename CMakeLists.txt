cmake_minimum_required(VERSION 2.6)
project(crabgrab)

include_directories(src)
include_directories(src/urdl/include)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	# Windows specific includes
	include_directories(src/win32)
	set(PLATFORM "win32")
	include_directories(src/win32/winapi/include)
	include_directories(src/win32/comet/include)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	#Linux specific includes
	set(PLATFORM "linux")
	include_directories(src/linux)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	#Mac specific includes
	set(PLATFORM "darwin")
	include_directories(src/darwin)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

## Boost

# set(Boost_USE_STATIC_LIBS TRUE)
set(Boost_DETAILED_FAILURE_MSG TRUE)
set(Boost_ADDITIONAL_VERSIONS 1.46.1)
# 1.41 is first version with Boost.PropertyTree
find_package(Boost 1.41 REQUIRED COMPONENTS filesystem system)
link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

## Preprocessor

add_definitions(
  -DWIN32_LEAN_AND_MEAN -D_SCL_SECURE_NO_WARNINGS -DURDL_HEADER_ONLY=1
  -D_WIN32_WINNT=0x0501 -DBOOST_FILESYSTEM_VERSION=2)

## Build

add_subdirectory(src)
# TODO: Make building the tests optional
add_subdirectory(test)

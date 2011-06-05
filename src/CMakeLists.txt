## Sources

set(CRABGRAB_SOURCES
  crabgrab/clipboard.hpp
  crabgrab/encode_bmp.hpp
  crabgrab/encode_bmp.cpp
  crabgrab/feedback.hpp
  crabgrab/twitpic/generate_body.hpp
  crabgrab/twitpic/generate_body.cpp
  crabgrab/twitpic/response.hpp
  crabgrab/twitpic/response.cpp
  crabgrab/twitpic/twitpic.hpp
  crabgrab/twitpic/twitpic.cpp
  ${PLATFORM}/crabgrab/feedback.cpp
  ${PLATFORM}/crabgrab/keyboard_hook.hpp
  ${PLATFORM}/crabgrab/screenshot.hpp
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows-only sources
    set(CRABGRAB_SOURCES ${CRABGRAB_SOURCES}
      win32/bitmap.h
      win32/crabgrab/convert_hbitmap.hpp
      win32/crabgrab/keyboard_hook.cpp
      win32/crabgrab/notification.hpp
      win32/crabgrab/notification.cpp
    )
endif()

set(LODEPNG_SOURCES
  LodePNG/lodepng.h
  LodePNG/lodepng.cpp
)

set(SOURCES ${CRABGRAB_SOURCES} ${LODEPNG_SOURCES})

# This bit just prettyfies the source hierarchy in Visual Studio
macro(group_by_hierarchy PROJECT_SOURCES)
  foreach(SOURCE ${PROJECT_SOURCES})
    string(REGEX REPLACE "[\\\\/][^\\\\/]*$" "" GROUP_NAME "${SOURCE}" )
    string(REGEX REPLACE "^[\\\\/]" "" GROUP_NAME "${GROUP_NAME}" )
    string(REGEX REPLACE "/" "\\\\\\\\" GROUP_NAME "${GROUP_NAME}" )

#    if(${SOURCE} MATCHES "\\.h(pp)?$")
#      source_group("${GROUP_NAME}\\Header Files" FILES ${SOURCE})
#    elseif(${SOURCE} MATCHES "\\.c(pp)?$")
#      source_group("${GROUP_NAME}\\Source Files" FILES ${SOURCE})
#    else()
      source_group("${GROUP_NAME}" FILES ${SOURCE})
#    endif()
  endforeach()
endmacro()

group_by_hierarchy("${CRABGRAB_SOURCES}") # must be quoted to pass list
group_by_hierarchy("${LODEPNG_SOURCES}") # must be quoted to pass list

## OpenSSL

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

## Build

add_library(crabgrab-lib ${SOURCES})
add_executable(crabgrab crabgrab/main.cpp)
target_link_libraries(crabgrab crabgrab-lib ${OPENSSL_LIBRARIES})

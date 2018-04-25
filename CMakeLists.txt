cmake_minimum_required(VERSION 3.1.0)
project(mk-cxx LANGUAGES CXX)

# Compiler flags

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if("${UNIX}" OR "${MINGW}")
  set(LIBNDT_FLAGS "-Wall -Wextra -Werror")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBNDT_FLAGS} -Wmissing-prototypes")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LIBNDT_FLAGS}")
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # for GCC, -Wmissing-prototypes only works for C/ObjC.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wmissing-prototypes")
  endif()
endif()

if(${WIN32})
  add_definitions(-D_WIN32_WINNT=0x0600) # for backward compat
endif()

# Library

install(
  FILES
  include/measurement_kit/cxx/cxx.hpp
  include/measurement_kit/cxx/json.hpp
  DESTINATION
  include/measurement_kit/cxx
)
if("${WIN32}" OR "${MINGW}")
  target_link_libraries(ndt ws2_32)
  if ("${MINGW}")
    target_link_libraries(ndt -static-libgcc -static-libstdc++)
  endif()
endif()

# Testing

set(BUILD_TESTING ON CACHE BOOL "Whether to build tests")
enable_testing()

if(${BUILD_TESTING}) # is a standard name in CMake-land

  add_executable(web_connectivity example/web_connectivity.cpp)
  target_include_directories(
    web_connectivity
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
  )
  target_link_libraries(web_connectivity measurement_kit)
  add_test(
    NAME web_connectivity
    COMMAND web_connectivity
  )

  add_executable(ndt example/ndt.cpp)
  target_include_directories(
    ndt
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
  )
  target_link_libraries(ndt measurement_kit)
  add_test(
    NAME ndt
    COMMAND ndt
  )

endif()

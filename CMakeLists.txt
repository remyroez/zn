# cmake
cmake_minimum_required(VERSION 3.16)
cmake_policy(SET CMP0076 NEW)

# vcpkg
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE $ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake CACHE STRING "")
endif()

# project
project(zn C CXX)

# executable
add_executable(main)
set_target_properties(main PROPERTIES OUTPUT_NAME zn)
target_compile_features(main PRIVATE cxx_std_20)
target_compile_definitions(main PRIVATE $<$<CONFIG:Debug>:DEBUG_MODE>)
target_compile_options(main PRIVATE /wd4324)

# dependencies
find_package(argparse CONFIG REQUIRED)
target_link_libraries(main PRIVATE argparse::argparse)

# msvc
if (MSVC)
  target_compile_options(main PRIVATE /W4)
  set_target_properties(main PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
  add_definitions(/bigobj)
else()
  target_compile_options(main PRIVATE -Wall -Wextra)
endif()

# src
add_subdirectory(src)

target_include_directories(main PRIVATE src)

get_property("TARGET_SOURCE_FILES" TARGET main PROPERTY SOURCES)
source_group(TREE "${CMAKE_CURRENT_LIST_DIR}" FILES ${TARGET_SOURCE_FILES})

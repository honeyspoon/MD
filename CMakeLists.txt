set(CMAKE_CXX_COMPILER clang++)

cmake_minimum_required(VERSION 3.29 FATAL_ERROR)

project(
  market_data
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

include(import_std.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
  -Wall
  -Wextra
  -pedantic
  -g
  -O0
  -fmodules
  -fdiagnostics-color=always)
add_link_options(-flto)

add_compile_options(-fdiagnostics-color=always)

set(SOURCES src/parser.cpp src/stats.cpp src/reader.cpp)

add_executable(main ${SOURCES})

set_source_files_properties(${SOURCES} PROPERTIES CXX_MODULES true)

target_sources(main PRIVATE src/main.cpp)

target_sources(main PRIVATE FILE_SET CXX_MODULES FILES ${SOURCES})

target_link_libraries(main std)

file(REMOVE build_tools.txt)
file(
  APPEND build_tools.txt
  "Project: ${PROJECT_NAME}\n"
  "Version: ${PROJECT_VERSION}\n"
  "C++ Standard: ${CMAKE_CXX_STANDARD}\n"
  "Compiler: ${CMAKE_CXX_COMPILER}\n"
  "Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}\n"
  "Generator: ${CMAKE_GENERATOR}\n")

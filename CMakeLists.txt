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

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/modules/*.cpp")
message(STATUS "SOURCES: ${SOURCES}")

set_source_files_properties(${SOURCES} PROPERTIES CXX_MODULES true)

file(GLOB targets "${CMAKE_CURRENT_SOURCE_DIR}/src/exe/*")
foreach(file ${targets})
  get_filename_component(target ${file} NAME_WE)
  add_executable(${target} ${SOURCES})
  target_include_directories(${target}
                             INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
  target_sources(${target} PRIVATE ${file})
  target_sources(${target} PRIVATE FILE_SET CXX_MODULES FILES ${SOURCES})
  target_link_libraries(${target} std)
endforeach()

file(REMOVE build_tools.txt)
file(
  APPEND build_tools.txt
  "Project: ${PROJECT_NAME}\n"
  "Version: ${PROJECT_VERSION}\n"
  "C++ Standard: ${CMAKE_CXX_STANDARD}\n"
  "Compiler: ${CMAKE_CXX_COMPILER}\n"
  "Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}\n"
  "Generator: ${CMAKE_GENERATOR}\n")

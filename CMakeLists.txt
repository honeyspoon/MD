set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_TOOLCHAIN_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

cmake_minimum_required(VERSION 3.29 FATAL_ERROR)

project(
  market_data
  VERSION 1.0
  LANGUAGES CXX)

# flags
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)
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

# dependencies
find_package(rapidjson REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# sources
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/modules/*.cpp")
message(STATUS "SOURCES: ${SOURCES}")

set_source_files_properties(${SOURCES} PROPERTIES CXX_MODULES true)

# targets
file(GLOB targets "${CMAKE_CURRENT_SOURCE_DIR}/src/exe/*")
foreach(file ${targets})
  get_filename_component(target ${file} NAME_WE)
  add_executable(${target} ${SOURCES})
  target_include_directories(${target}
                             INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
  target_sources(${target} PRIVATE ${file})
  target_sources(${target} PRIVATE FILE_SET CXX_MODULES FILES ${SOURCES})
  target_link_libraries(${target} rapidjson spdlog::spdlog)
endforeach()

# log
file(REMOVE build_tools.txt)
file(
  APPEND build_tools.txt
  "Project: ${PROJECT_NAME}\n"
  "Version: ${PROJECT_VERSION}\n"
  "C++ Standard: ${CMAKE_CXX_STANDARD}\n"
  "Compiler: ${CMAKE_CXX_COMPILER}\n"
  "Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}\n"
  "Generator: ${CMAKE_GENERATOR}\n")

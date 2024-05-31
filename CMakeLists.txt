cmake_minimum_required(VERSION 3.29.20240416 FATAL_ERROR)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)

project(
  market_data
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
    # -Wall warning flags
    -Waddress
    -Warray-bounds
    -Wcomment
    -Wformat
    -Wformat-security
    -Wimplicit-function-declaration
    -Wimplicit-int
    -Winit-self
    -Wmain
    -Wmissing-braces
    -Wparentheses
    -Wpointer-sign
    -Wreorder
    -Wreturn-type
    -Wsequence-point
    -Wswitch
    -Wtrigraphs
    -Wuninitialized
    -Wunknown-pragmas
    -Wunused-function
    -Wunused-label
    -Wunused-value
    -Wunused-variable
    -Wvolatile-register-var

    # -Wextra warning flags
    -Wcast-align
    -Wcast-qual
    -Wchar-subscripts
    -Wconversion
    -Wempty-body
    -Wignored-qualifiers
    -Wmissing-field-initializers
    -Wsign-compare
    -Wtype-limits
    -Wunreachable-code
    -Wunused-parameter
)


# add_compile_options(-O3)

add_compile_options(
    -fsanitize=address
    -fsanitize=leak
    -fsanitize=float-divide-by-zero
    -fsanitize=integer-divide-by-zero
    -fsanitize=bounds
    -fsanitize=nullability
    -fsanitize=object-size
    -fsanitize=pointer-overflow
    -fsanitize=return
    -fsanitize=unsigned-integer-overflow
    -fsanitize=vptr
)

add_link_options(
    -fsanitize=address
    -fsanitize=leak
    -fsanitize=float-divide-by-zero
    -fsanitize=integer-divide-by-zero
    -fsanitize=nullability
    -fsanitize=object-size
    -fsanitize=pointer-overflow
    -fsanitize=return
    -fsanitize=unsigned-integer-overflow
    -fsanitize=vptr
)

add_link_options(-flto)
add_compile_options(-fdiagnostics-color=always)

# dependencies
find_package(nlohmann_json CONFIG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(cxxopts CONFIG REQUIRED)

set(LIBS
  nlohmann_json::nlohmann_json
  ZLIB::ZLIB
  cxxopts::cxxopts
)

# sources
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/modules/*.cpp")
message(STATUS "SOURCES: ${SOURCES}")

set_source_files_properties(${SOURCES} PROPERTIES CXX_MODULES true)

# targets
file(GLOB targets "${CMAKE_CURRENT_SOURCE_DIR}/src/exe/*")
foreach(file ${targets})
  get_filename_component(target ${file} NAME_WE)
  add_executable(${target} ${SOURCES})
  target_sources(${target} PRIVATE ${file})
  target_sources(${target} PRIVATE FILE_SET CXX_MODULES FILES ${SOURCES})
  target_compile_features(${target} PUBLIC cxx_std_23)
  target_link_libraries(${target} PRIVATE ${LIBS})
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

cmake_minimum_required(VERSION 3.29)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
project(
  market_data
  VERSION 1.0
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_CXX_MODULE_STD ON)

add_compile_options(-Wall -Wextra -pedantic -g -flto)
add_compile_options(-O0)
# add_compile_options(-ftime-report)

include(FetchContent)

# Also requires one of: libbfd (gnu binutils), libdwarf, libdw (elfutils)
FetchContent_Declare(
  backward
  GIT_REPOSITORY https://github.com/bombela/backward-cpp
  GIT_TAG master # or a version tag, such as v1.6
  SYSTEM # optional, the Backward include directory will be treated as system
         # directory
)
FetchContent_MakeAvailable(backward)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmodules-ts")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -g")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(include)

set(SOURCES src/main.cpp src/parser.cpp src/stats.cpp)

add_executable(main ${SOURCES})

target_link_libraries(main PUBLIC Backward::Interface)

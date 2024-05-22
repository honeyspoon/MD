cmake_minimum_required(VERSION 3.29)

function(detect_llvm_version OUTPUT_VAR)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    execute_process(
      COMMAND ${CMAKE_CXX_COMPILER} --version
      OUTPUT_VARIABLE clang_version_output
      OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REGEX MATCH "clang version ([0-9]+)\\.([0-9]+)\\.([0-9]+)" _
                 ${clang_version_output})
    set(${OUTPUT_VAR}
        "${CMAKE_MATCH_1}.${CMAKE_MATCH_2}.${CMAKE_MATCH_3}"
        PARENT_SCOPE)
  else()
    message(
      FATAL_ERROR
        "Clang compiler not detected. This script only works with Clang.")
  endif()
endfunction()

detect_llvm_version(LLVM_VERSION)
string(REPLACE "." "_" LLVM_VERSION_UNDERSCORE ${LLVM_VERSION})

set(LIBCLC_URL
    "https://github.com/llvm/llvm-project/releases/download/llvmorg-${LLVM_VERSION}/libcxx-${LLVM_VERSION}.src.tar.xz"
)
set(LIBCLC_TAR "${CMAKE_BINARY_DIR}/libcxx-${LLVM_VERSION}.src.tar.xz")
set(LIBCLC_SRC_DIR "${CMAKE_BINARY_DIR}/libcxx-${LLVM_VERSION}.src")

if(NOT EXISTS ${LIBCLC_SRC_DIR})
  file(DOWNLOAD ${LIBCLC_URL} ${LIBCLC_TAR})
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xvf ${LIBCLC_TAR}
                  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

set(LIBCXX_GENERATED_MODULE_DIR ${LIBCLC_SRC_DIR}/modules/b)
set(CMAKE_INSTALL_PREFIX ${LIBCLC_SRC_DIR}/modules/a)
set(LIBCXX_INSTALL_LIBRARY_DIR ${LIBCLC_SRC_DIR}/modules/d)
set(LIBCXX_INSTALL_MODULES_DIR ${LIBCLC_SRC_DIR}/modules/e)
set(LIBCXX_LIBRARY_DIR ${LIBCLC_SRC_DIR}/modules/f)

add_subdirectory(${LIBCLC_SRC_DIR}/modules/)

set(STD_SOURCE_DIR "${LIBCXX_GENERATED_MODULE_DIR}")
add_library(std)

set_target_properties(std PROPERTIES LINKER_LANGUAGE CXX)

target_sources(
  std
  PUBLIC FILE_SET
         CXX_MODULES
         TYPE
         CXX_MODULES
         BASE_DIRS
         ${STD_SOURCE_DIR}
         FILES
         ${STD_SOURCE_DIR}/std.cppm
         ${STD_SOURCE_DIR}/std.compat.cppm)

link_libraries(std)

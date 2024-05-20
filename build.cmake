#!cmake -P

file(REMOVE build_tools.txt)

set(BUILD_DIR out)
set(ABS_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_DIR}")

execute_process(COMMAND cmake --version OUTPUT_VARIABLE cmake_version)
file(APPEND build_tools.txt "cmake version: ${cmake_version}")
execute_process(COMMAND clang --version OUTPUT_VARIABLE clang_version)
file(APPEND build_tools.txt "clang version: ${clang_version}")
execute_process(COMMAND ninja --version OUTPUT_VARIABLE ninja_version)
file(APPEND build_tools.txt "ninja version: ${ninja_version}")

if(NOT EXISTS ${ABS_BUILD_DIR})
  message("Creating build directory")
  execute_process(COMMAND cmake -G Ninja -B ${BUILD_DIR})
else()
  message("Reusing existing build directory")
endif()

execute_process(
  COMMAND cmake --build ${ABS_BUILD_DIR}
  WORKING_DIRECTORY ${ABS_BUILD_DIR}
  RESULT_VARIABLE build_result)

if(build_result EQUAL 0)
  message("Build completed")
  set(BUILD_SUCCESS TRUE)
else()
  set(BUILD_SUCCESS FALSE)
  message("Command failed with exit code: ${build_result}")
endif()

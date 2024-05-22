#!cmake -P

set(BUILD_DIR out)
set(ABS_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${BUILD_DIR}")

if(NOT EXISTS ${ABS_BUILD_DIR})
  message("Creating build directory")
  execute_process(COMMAND unbuffer cmake -GNinja -B ${BUILD_DIR} COMMAND_ECHO
                          STDOUT)
else()
  message("Reusing existing build directory")
endif()

execute_process(
  COMMAND unbuffer cmake --build ${ABS_BUILD_DIR}
  WORKING_DIRECTORY ${ABS_BUILD_DIR} COMMAND_ECHO STDOUT
  RESULT_VARIABLE build_result)

if(build_result EQUAL 0)
  message("Build completed")
  set(BUILD_SUCCESS TRUE)
else()
  set(BUILD_SUCCESS FALSE)
  message("Command failed with exit code: ${build_result}")
endif()

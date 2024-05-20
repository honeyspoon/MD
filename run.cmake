#!cmake -P

include(build.cmake)

if(BUILD_SUCCESS)
  message("build success")
  execute_process(COMMAND echo 4)
  message(${BUILD_DIR}/main)
  execute_process(COMMAND main OUCHLMM2.incoming.packets
                  WORKING_DIRECTORY ${BUILD_DIR})
else()
  message("build failed")
endif()

#!cmake -P

include(build.cmake)

if(BUILD_SUCCESS)
  message("=============")
  execute_process(COMMAND unbuffer main OUCHLMM2.incoming.packets COMMAND_ECHO
                          STDOUT WORKING_DIRECTORY ${BUILD_DIR})
  message("=============")
else()
  message("build failed")
endif()

#!cmake -P

function(build_and_run)
  include(run.cmake)
  message("\n")
endfunction()

build_and_run()

function(watch_files)
  message("Watching for file changes...")
  while(1)
    execute_process(COMMAND fswatch -1 ${CMAKE_CURRENT_SOURCE_DIR} -e out
                    OUTPUT_VARIABLE FILE_CHANGED)

    if(FILE_CHANGED)
      string(REGEX MATCH "\\.cpp|\\.h|CMakeLists\\.txt" FILE_CHANGED_MATCH
                   ${FILE_CHANGED})

      if(FILE_CHANGED_MATCH)
        build_and_run()
      endif()
    endif()
  endwhile()
endfunction()

watch_files()

#!cmake -P

function(build_and_run)
  include(run.cmake)
  message("\n")
endfunction()

build_and_run()

function(watch_files)
  message("Watching for file changes...")
  while(1)
    execute_process(COMMAND fswatch -1 ${CMAKE_CURRENT_SOURCE_DIR}
                    OUTPUT_VARIABLE FILE_CHANGED)

    message("File changed: ${FILE_CHANGED}")
    if(FILE_CHANGED)
      string(FIND "${FILE_CHANGED}" ".cpp$" CPP_POS)
      string(FIND "${FILE_CHANGED}" ".h$" H_POS)
      string(FIND "${FILE_CHANGED}" "CMakeLists.txt$" CMAKE_POS)

      if(CPP_POS GREATER -1
         OR H_POS GREATER -1
         OR CMAKE_POS GREATER -1)
        build_and_run()
      endif()
    endif()
  endwhile()
endfunction()

watch_files()

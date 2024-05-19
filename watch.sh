fswatch -r -1 -e ".*" -i "\\.cpp$|\\.h$" . | while read -r file; do ./run.sh && break; done

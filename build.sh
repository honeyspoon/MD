if [ -d "out" ]; then
	echo "reusing existing build directory"
else
	mkdir -p out
	pushd out
	cmake ..
fi

pushd out
cmake --build .

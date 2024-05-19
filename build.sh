# check if dir exists
if [ ! -d "./out" ]; then
	mkdir -p out
	pushd ./out
	cmake ..
else
	echo "reusing build dir"
fi

pushd ./out
cmake --build .
popd

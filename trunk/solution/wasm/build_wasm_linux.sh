if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
emcmake cmake ../
emmake make
emcc -O3 ./libtest.a \
    -o test.wasm \
	--no-entry
cd ../
exec /bin/bash
cd build
emcc -O3 ./Release/libtest.lib \
    -o test.wasm \
    -s NO_EXIT_RUNTIME=1 -s EXPORTED_RUNTIME_METHODS='["cwrap"]' -s ALLOW_MEMORY_GROWTH=1 \
    -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s USE_WEBGL2=0 \
	--no-entry
cd ../
exec /bin/bash
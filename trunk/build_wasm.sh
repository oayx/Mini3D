if [ ! -d "build/wasm" ]; then
    mkdir -p build/wasm
fi
cd build/wasm
emcmake cmake ../../engine -G "Visual Studio 15 2017" -A x64
emcc -O3 engine.lib\
    -o engine.js
cd ../../
exec /bin/bash
if [ ! -d "build" ]; then
    mkdir -p build
fi
cd build
emcmake cmake ../ -G "Visual Studio 15 2017"
exec /bin/bash
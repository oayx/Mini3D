@echo on
if not exist build\win_x64 (
    mkdir build\win_x64
)
cd build\win_x64
cmake ..\..\engine -G "Visual Studio 15 2017" -A x64 -DCMAKE_SYSTEM_VERSION=10.0.17763.0
cd ..\..\engine
pause
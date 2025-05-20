@echo off
setlocal

REM Create build directory if it doesn't exist
if not exist build mkdir build
cd build

REM Configure and build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build .

echo Build complete! 
@echo off
setlocal

REM Create build directory if it doesn't exist
if not exist build_glfw mkdir build_glfw
cd build_glfw

REM Download GLFW
curl -L -o glfw.zip https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip
powershell -Command "Expand-Archive -Force glfw.zip -DestinationPath ."
cd glfw-3.4

REM Configure with CMake
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_INSTALL=OFF .

REM Build
cmake --build .

REM Copy the built library
copy lib\libglfw3.a ..\..\lib\glfw3.lib

cd ..\..
if exist build_glfw rmdir /s /q build_glfw

echo GLFW build complete! 
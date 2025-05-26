@echo off
setlocal enabledelayedexpansion

echo Setting up external dependencies...

REM Create external directory if it doesn't exist
if not exist external mkdir external
cd external

REM Download and setup GLFW
echo Downloading GLFW...
if not exist glfw (
    mkdir glfw
    powershell -Command "Invoke-WebRequest -Uri 'https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip' -OutFile 'glfw.zip'"
    powershell -Command "Expand-Archive -Path 'glfw.zip' -DestinationPath '.'"
    move glfw-3.4\* glfw\
    rmdir glfw-3.4
    del glfw.zip
)

REM Download and setup GLM
echo Downloading GLM...
if not exist glm (
    mkdir glm
    powershell -Command "Invoke-WebRequest -Uri 'https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip' -OutFile 'glm.zip'"
    powershell -Command "Expand-Archive -Path 'glm.zip' -DestinationPath '.'"
    move glm-0.9.9.8\glm\* glm\
    rmdir glm-0.9.9.8
    del glm.zip
)

REM Download and setup ImGui
echo Downloading ImGui...
if not exist imgui (
    mkdir imgui
    powershell -Command "Invoke-WebRequest -Uri 'https://github.com/ocornut/imgui/archive/refs/tags/v1.90.1.zip' -OutFile 'imgui.zip'"
    powershell -Command "Expand-Archive -Path 'imgui.zip' -DestinationPath '.'"
    move imgui-1.90.1\* imgui\
    rmdir imgui-1.90.1
    del imgui.zip
)

REM Download and setup nlohmann_json
echo Downloading nlohmann_json...
if not exist nlohmann_json (
    mkdir nlohmann_json
    powershell -Command "Invoke-WebRequest -Uri 'https://github.com/nlohmann/json/releases/download/v3.11.3/json.hpp' -OutFile 'nlohmann_json/json.hpp'"
)

cd ..

echo Setup complete!
echo.
echo Note: Make sure you have Qt6 installed at the path specified in CMakeLists.txt
echo If you need to install Qt6, please visit: https://www.qt.io/download-qt-installer
echo.
pause 
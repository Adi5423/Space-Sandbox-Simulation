@echo off
echo Downloading FreeType...
powershell -Command "& {Invoke-WebRequest -Uri 'https://github.com/freetype/freetype/releases/download/VER-2-13-2/freetype-2.13.2.tar.gz' -OutFile 'freetype.tar.gz'}"

echo Extracting FreeType...
powershell -Command "& {Expand-Archive -Path 'freetype.tar.gz' -DestinationPath '.' -Force}"

echo Copying FreeType files...
xcopy /E /I /Y freetype-2.13.2\include\freetype include\freetype
copy /Y freetype-2.13.2\objs\freetype.lib lib\freetype.lib

echo Cleaning up...
rmdir /S /Q freetype-2.13.2
del freetype.tar.gz

echo Done! 
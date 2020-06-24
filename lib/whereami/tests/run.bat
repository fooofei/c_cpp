@echo off

%~d0
cd /d %~dp0


rmdir /S /Q bin >nul 2>&1
mkdir bin

cd library
rmdir /S /Q build >nul 2>&1
mkdir build
cd build
cmake .. || exit /B 1
cmake --build . --config release || exit /B 1
cd ..
rmdir /S /Q build >nul 2>&1

cd ..
cd executable
rmdir /S /Q build >nul 2>&1
mkdir build
cd build
cmake .. || exit /B 1
cmake --build . --config release || exit /B 1
cd ..
rmdir /S /Q build >nul 2>&1

cd ..
cd bin
call.exe
cd ..
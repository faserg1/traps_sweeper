@echo off
set SCRIPT_PATH=%~dp0
cd /d %SCRIPT_PATH%
set /p EM_SDK_PATH=<.emsdk_path
call %EM_SDK_PATH%\emsdk_env.bat
if not exist em_build mkdir em_build
cd em_build
cmake -DCMAKE_TOOLCHAIN_FILE="..\cmake\emscripten-toolchain.cmake" -G "MinGW Makefiles" ..
cd ..
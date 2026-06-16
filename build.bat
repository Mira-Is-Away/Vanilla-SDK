@echo off
REM Windows Build Script for Vanilla Engine

if not exist bin (
    mkdir bin
)

cd bin
cmake ..
if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed.
    exit /b %ERRORLEVEL%
)

cmake --build . --config Debug
if %ERRORLEVEL% neq 0 (
    echo Build failed.
    exit /b %ERRORLEVEL%
)

cd ..
echo Build succeeded!

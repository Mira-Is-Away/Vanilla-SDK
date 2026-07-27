@echo off
REM Windows Build Script for Vanilla Engine

cmake --preset default

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed.
    exit /b %ERRORLEVEL%
)

cmake --build build --config Debug
if %ERRORLEVEL% neq 0 (
    echo Build failed.
    exit /b %ERRORLEVEL%
)

echo Build succeeded!

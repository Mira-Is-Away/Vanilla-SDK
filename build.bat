@echo off
REM Windows Build Script for Vanilla Engine

REM Ensure MSVC compiler environment is initialized
where cl >nul 2>nul
if %ERRORLEVEL% neq 0 (
    for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do (
        if exist "%%i\VC\Auxiliary\Build\vcvars64.bat" (
            call "%%i\VC\Auxiliary\Build\vcvars64.bat"
        )
    )
)

REM Ensure Ninja is in PATH if provided by Visual Studio
where ninja >nul 2>nul
if %ERRORLEVEL% neq 0 (
    for /f "usebackq tokens=*" %%i in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath`) do (
        if exist "%%i\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja" (
            set "PATH=%PATH%;%%i\Common7\IDE\CommonExtensions\Microsoft\CMake\Ninja"
        )
    )
)

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


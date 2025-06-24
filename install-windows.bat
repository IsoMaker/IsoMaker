@echo off
setlocal

echo ===== IsoMaker Installation Script for Windows =====

if not exist "bin" (
    echo Creating bin directory...
    mkdir bin
)

where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo Error: cmake not found. Please install cmake.
    exit /b 1
)

echo Building IsoMaker...
if exist "bin\IsoMaker.exe" del /f "bin\IsoMaker.exe"
cmake -B cmake/build -S cmake
cmake --build cmake/build

if exist "cmake\build\IsoMaker.exe" (
    echo Build successful!
    copy "cmake\build\IsoMaker.exe" "bin\IsoMaker.exe"
    echo IsoMaker has been installed to bin\IsoMaker.exe
) else if exist "cmake\build\Debug\IsoMaker.exe" (
    echo Build successful!
    copy "cmake\build\Debug\IsoMaker.exe" "bin\IsoMaker.exe"
    echo IsoMaker has been installed to bin\IsoMaker.exe
) else if exist "cmake\build\Release\IsoMaker.exe" (
    echo Build successful!
    copy "cmake\build\Release\IsoMaker.exe" "bin\IsoMaker.exe"
    echo IsoMaker has been installed to bin\IsoMaker.exe
) else (
    echo Compilation failed: IsoMaker.exe binary not found!
    exit /b 1
)

echo Installation complete!
echo You can run it by double-clicking the executable or with: bin\IsoMaker.exe

endlocal
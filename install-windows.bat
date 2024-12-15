@echo off
if exist "%cd%\bin\IsoMaker.exe" (
    del "%cd%\bin\IsoMaker.exe"
    echo IsoMaker.exe deleted.
) else (
    echo IsoMaker.exe don't exist in the bin directory.
)

if exist "%cd%\cmake\build" (
    rmdir /s /q "%cd%\cmake\build"
    echo Directory build deleted.
) else (
    echo The build directory don't exist.
)

mkdir "%cd%\cmake\build"

cmake cmake -B cmake/build
if errorlevel 1 (
    echo Error during the CMake generation.
    pause
    exit /b 1
)

cmake --build cmake/build
if errorlevel 1 (
    echo Error during the building.
    pause
    exit /b 1
)

if exist "%cd%\cmake\build\Debug\IsoMaker.exe" (
    move "%cd%\cmake\build\Debug\IsoMaker.exe" "%cd%\bin"
    echo IsoMaker.exe moved in the bin directory.
) else (
    echo IsoMaker.exe not found.
    pause
    exit /b 1
)

pause

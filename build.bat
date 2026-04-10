@echo off
setlocal

set "SCRIPT_DIR=%~dp0"
if "%SCRIPT_DIR:~-1%"=="\" set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
set "BUILD_DIR=%SCRIPT_DIR%\build-win"
set "GENERATOR=Visual Studio 16 2019"

set "PostgreSQL_ROOT=C:\Program Files\PostgreSQL\18"

set "CMAKE_ARGS="
if defined PostgreSQL_ROOT set "CMAKE_ARGS=%CMAKE_ARGS% -DPostgreSQL_ROOT=%PostgreSQL_ROOT%"
if defined CMAKE_PREFIX_PATH set "CMAKE_ARGS=%CMAKE_ARGS% -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH%"

cmake -S "%SCRIPT_DIR%" -B "%BUILD_DIR%" -G "%GENERATOR%" -A x64 %CMAKE_ARGS%
if errorlevel 1 exit /b %errorlevel%

cmake --build "%BUILD_DIR%" --config Release --target postgres.hdll
if errorlevel 1 exit /b %errorlevel%

echo.
echo Built postgres.hdll in:
echo   %BUILD_DIR%\bin


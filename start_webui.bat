@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"
title RC Engine Sound Configurator

echo ============================================
echo   RC Engine Sound ESP32 Configurator
echo ============================================
echo.

:: -----------------------------------------------------------
:: 1. Find a REAL Python 3 (skip Inkscape / Store stubs)
:: -----------------------------------------------------------
set "PY="

:: Check the py launcher first (most reliable on Windows)
where py >nul 2>nul
if !errorlevel!==0 (
  for /f "tokens=*" %%V in ('py -3 --version 2^>nul') do set "PY=py -3"
)

:: Check common install locations explicitly
if not defined PY (
  for %%P in (
    "%LOCALAPPDATA%\Programs\Python\Python312\python.exe"
    "%LOCALAPPDATA%\Programs\Python\Python311\python.exe"
    "%LOCALAPPDATA%\Programs\Python\Python310\python.exe"
    "C:\Python312\python.exe"
    "C:\Python311\python.exe"
    "C:\Python310\python.exe"
    "%ProgramFiles%\Python312\python.exe"
    "%ProgramFiles%\Python311\python.exe"
  ) do (
    if exist %%P (
      if not defined PY set "PY=%%~P"
    )
  )
)

:: Last resort: 'python' from PATH, but verify it's real (not Inkscape)
if not defined PY (
  where python >nul 2>nul
  if !errorlevel!==0 (
    for /f "tokens=*" %%L in ('python -c "import sys; print(sys.executable)" 2^>nul') do (
      echo %%L | findstr /i "inkscape" >nul
      if !errorlevel! neq 0 set "PY=python"
    )
  )
)

if not defined PY (
  echo.
  echo  ERROR: Python 3 is not installed.
  echo.
  echo  Please install Python from:  https://www.python.org/downloads/
  echo  CHECK the box "Add Python to PATH" during install!
  echo.
  pause
  exit /b 1
)

echo  Found Python: %PY%

:: -----------------------------------------------------------
:: 2. Auto-install pyserial if missing (needed for USB flash)
:: -----------------------------------------------------------
%PY% -c "import serial" 2>nul
if !errorlevel! neq 0 (
  echo  Installing pyserial ^(needed for USB connection^)...
  %PY% -m pip install pyserial --quiet 2>nul
  if !errorlevel! neq 0 (
    echo  WARNING: Could not install pyserial. Flash-over-USB may not work.
    echo  You can still build and download the .bin file.
  ) else (
    echo  pyserial installed OK.
  )
)

:: -----------------------------------------------------------
:: 3. Start the server hidden and open the browser
:: -----------------------------------------------------------

start /b "" wscript //nologo "%~dp0_launch_hidden.vbs" "%PY%"
timeout /t 2 /noexec >nul
start "" http://localhost:8080
exit

@echo off
cd /d %~dp0

:: Set download filename
set ARDUINO_CLI_FILENAME=arduino-cli_latest_Windows_64bit.zip

:: Download
echo Downloading file https://downloads.arduino.cc/arduino-cli/%ARDUINO_CLI_FILENAME%
curl -fsSL -o "%TEMP%\%ARDUINO_CLI_FILENAME%" "https://downloads.arduino.cc/arduino-cli/%ARDUINO_CLI_FILENAME%"
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo.

:: Installing
set ARDUINO_CLI_INSTALL_DIR=%USERPROFILE%\Desktop\arduino-cli
echo Installing Arduino CLI to directory: %ARDUINO_CLI_INSTALL_DIR%
7z x -y "%TEMP%\%ARDUINO_CLI_FILENAME%" "-o%ARDUINO_CLI_INSTALL_DIR%"
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo.

:: Verify
echo Searching for arduino cli executable...
set PATH=%ARDUINO_CLI_INSTALL_DIR%;%PATH%
where arduino-cli.exe
if %errorlevel% neq 0 exit /b %errorlevel%
arduino-cli version
echo.
echo.

echo Initialize configuration file...
REM Expecting something like:  Config file written to: C:\Users\%USERNAME%\AppData\Local\Arduino15\arduino-cli.yaml
arduino-cli config init --overwrite
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo.

echo Installing arduino:avr core...
REM Use `--skip-post-install` on AppVeyor to skip UAC prompt which is blocking the build.
arduino-cli core install arduino:avr --skip-post-install
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo.

echo Installing esp8266:esp8266 core...
arduino-cli core install esp8266:esp8266
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo.

echo Merging arduino-cli configuration file with temp-esp32.yaml...
yaml-merge temp-esp32.yaml "C:\Users\%USERNAME%\AppData\Local\Arduino15\arduino-cli.yaml" --overwrite="C:\Users\%USERNAME%\AppData\Local\Arduino15\arduino-cli.yaml"
if %errorlevel% neq 0 exit /b %errorlevel%
type "C:\Users\%USERNAME%\AppData\Local\Arduino15\arduino-cli.yaml"
echo.
echo.

echo Installing esp32:esp32 core...
arduino-cli core update-index
if %errorlevel% neq 0 exit /b %errorlevel%
arduino-cli board listall
if %errorlevel% neq 0 exit /b %errorlevel%
arduino-cli core install esp32:esp32
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo.

echo Listing all installed cores...
arduino-cli core list
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo.

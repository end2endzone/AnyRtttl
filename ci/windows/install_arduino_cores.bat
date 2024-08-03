@echo off
cd /d %~dp0

:: Set arduino-cli config file path
set ARDUINO_CONFIG_PATH=C:\Users\%USERNAME%\AppData\Local\Arduino15\arduino-cli.yaml
echo ARDUINO_CONFIG_PATH set to '%ARDUINO_CONFIG_PATH%'
type "%ARDUINO_CONFIG_PATH%"
echo.
echo.

echo Adding arduinocli-core-esp8266.yaml to arduino-cli config...
yaml-merge arduinocli-core-esp8266.yaml "%ARDUINO_CONFIG_PATH%" --overwrite="%ARDUINO_CONFIG_PATH%"
if %errorlevel% neq 0 exit /b %errorlevel%
type "%ARDUINO_CONFIG_PATH%"
echo.
echo.

echo Adding arduinocli-core-esp32.yaml to arduino-cli config...
yaml-merge arduinocli-core-esp32.yaml "%ARDUINO_CONFIG_PATH%" --overwrite="%ARDUINO_CONFIG_PATH%"
if %errorlevel% neq 0 exit /b %errorlevel%
type "%ARDUINO_CONFIG_PATH%"
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

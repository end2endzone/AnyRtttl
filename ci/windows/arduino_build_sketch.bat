@echo off

:: Set PRODUCT_SOURCE_DIR root directory
setlocal enabledelayedexpansion
if "%PRODUCT_SOURCE_DIR%"=="" (
  :: Delayed expansion is required within parentheses https://superuser.com/questions/78496/variables-in-batch-file-not-being-set-when-inside-if
  cd /d "%~dp0"
  cd ..\..
  set PRODUCT_SOURCE_DIR=!CD!
  cd ..\..
  echo PRODUCT_SOURCE_DIR set to '!PRODUCT_SOURCE_DIR!'.
)
endlocal & set PRODUCT_SOURCE_DIR=%PRODUCT_SOURCE_DIR%
echo.

:: Check Arduino CLI installation
echo Expecting Arduino CLI installed in directory: %ARDUINO_CLI_INSTALL_DIR%
echo Searching for arduino cli executable...
set PATH=%PATH%;%ARDUINO_CLI_INSTALL_DIR%
where arduino-cli.exe
if %errorlevel% neq 0 exit /b %errorlevel%
echo.

set ARDUINO_INO_FILE=%PRODUCT_SOURCE_DIR%\examples\%~1\%~1.ino

echo ==========================================================================================================
echo Compiling %ARDUINO_INO_FILE% (atmega328)
echo ==========================================================================================================
cd /d "%PRODUCT_SOURCE_DIR%\examples\%~1"
arduino-cli compile --fqbn "arduino:avr:nano:cpu=atmega328" %~1.ino
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo.

echo ==========================================================================================================
echo Compiling %ARDUINO_INO_FILE% (esp8266)
echo ==========================================================================================================
cd /d "%PRODUCT_SOURCE_DIR%\examples\%~1"
arduino-cli compile --fqbn "esp8266:esp8266:nodemcuv2" %~1.ino
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo.

echo ==========================================================================================================
echo Compiling %ARDUINO_INO_FILE% (esp32)
echo ==========================================================================================================
cd /d "%PRODUCT_SOURCE_DIR%\examples\%~1"
arduino-cli compile --fqbn "esp32:esp32:esp32" %~1.ino
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo.

echo ==========================================================================================================
echo Compiling %ARDUINO_INO_FILE% (esp32wroverkit)
echo ==========================================================================================================
cd /d "%PRODUCT_SOURCE_DIR%\examples\%~1"
arduino-cli compile --fqbn "esp32:esp32:esp32wroverkit" %~1.ino
if %errorlevel% neq 0 exit /b %errorlevel%
echo.
echo.

cd /d "%~dp0"

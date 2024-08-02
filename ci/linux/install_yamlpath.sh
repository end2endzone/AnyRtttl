# Any commands which fail will cause the shell script to exit immediately
set -e

# Set download filename
export ARDUINO_CLI_FILENAME=""
if [ "$RUNNER_OS" = "Linux" ]; then
  export ARDUINO_CLI_FILENAME=arduino-cli_latest_Linux_64bit.tar.gz
elif [ "$RUNNER_OS" = "macOS" ]; then
  export ARDUINO_CLI_FILENAME=arduino-cli_latest_macOS_64bit.tar.gz
fi

# Download
echo Downloading file https://downloads.arduino.cc/arduino-cli/$ARDUINO_CLI_FILENAME
wget --no-verbose https://downloads.arduino.cc/arduino-cli/$ARDUINO_CLI_FILENAME
echo
echo

# Installing
export ARDUINO_CLI_INSTALL_DIR=$HOME/arduino-cli
echo Installing Arduino CLI to directory: $ARDUINO_CLI_INSTALL_DIR
mkdir -p $ARDUINO_CLI_INSTALL_DIR
tar xf $ARDUINO_CLI_FILENAME --directory $ARDUINO_CLI_INSTALL_DIR
echo
echo

# Verify
echo Searching for arduino cli executable...
export PATH=$PATH:$ARDUINO_CLI_INSTALL_DIR
which arduino-cli
arduino-cli version
echo
echo

echo Initialize configuration file
# Expecting something like:  Config file written: /home/$USER/.arduino15/arduino-cli.yaml
arduino-cli config init
echo
echo

echo Installing "arduino:avr" core...
arduino-cli core install "arduino:avr"
echo
echo

echo Installing "esp8266:esp8266" core...
arduino-cli core install "esp8266:esp8266"
echo
echo

if %errorlevel% neq 0 exit /b %errorlevel%


# Also install esp32:esp32 ?

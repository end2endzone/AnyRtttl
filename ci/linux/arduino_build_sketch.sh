# Any commands which fail will cause the shell script to exit immediately
set -e

# Set PRODUCT_SOURCE_DIR root directory
if [ "$PRODUCT_SOURCE_DIR" = "" ]; then
  RESTORE_DIRECTORY="$PWD"
  cd "$(dirname "$0")"
  cd ../..
  export PRODUCT_SOURCE_DIR="$PWD"
  echo "PRODUCT_SOURCE_DIR set to '$PRODUCT_SOURCE_DIR'."
  cd "$RESTORE_DIRECTORY"
  unset RESTORE_DIRECTORY
fi

# Check Arduino CLI installation
echo Expecting Arduino IDE installed in directory: $ARDUINO_CLI_INSTALL_DIR
echo Searching for arduino cli executable...
export PATH=$PATH:$ARDUINO_CLI_INSTALL_DIR
which arduino-cli
echo

export ARDUINO_INO_FILE=$PRODUCT_SOURCE_DIR/examples/$1/$1.ino

echo ==========================================================================================================
echo Compiling $ARDUINO_INO_FILE \(atmega328\)
echo ==========================================================================================================
cd $PRODUCT_SOURCE_DIR/examples/$1
arduino-cli compile --fqbn "arduino:avr:nano:cpu=atmega328" "$1.ino"
echo
echo

echo ==========================================================================================================
echo Compiling $ARDUINO_INO_FILE \(esp8266\)
echo ==========================================================================================================
cd $PRODUCT_SOURCE_DIR/examples/$1
arduino-cli compile --fqbn "esp8266:esp8266:nodemcuv2" "$1.ino"
echo
echo

echo ==========================================================================================================
echo Compiling $ARDUINO_INO_FILE \(esp32\)
echo ==========================================================================================================
cd $PRODUCT_SOURCE_DIR/examples/$1
arduino-cli compile --fqbn "esp32:esp32:esp32" "$1.ino"
echo
echo

echo ==========================================================================================================
echo Compiling $ARDUINO_INO_FILE \(esp32wroverkit\)
echo ==========================================================================================================
cd $PRODUCT_SOURCE_DIR/examples/$1
arduino-cli compile --fqbn "esp32:esp32:esp32wroverkit" "$1.ino"
echo
echo

cd "$(dirname "$0")"

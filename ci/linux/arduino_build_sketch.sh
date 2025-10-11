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
export ARDUINO_BOARDS_FILE=$PRODUCT_SOURCE_DIR/examples/$1/boards.txt

# Check for boards.txt
if [[ -f "$ARDUINO_BOARDS_FILE" ]]; then
  # ok
  echo "Found boards.txt at $ARDUINO_BOARDS_FILE."
  echo
else
  echo "No boards.txt found at $ARDUINO_BOARDS_FILE."
  exit 1
fi

echo ==========================================================================================================
echo Compiling $ARDUINO_INO_FILE \(atmega328\)
echo ==========================================================================================================
board=atmega328
fqbn="arduino:avr:nano:cpu=atmega328"
if grep -q "^all$" "$ARDUINO_BOARDS_FILE" || grep -q "^$board$" "$ARDUINO_BOARDS_FILE"; then
  cd $PRODUCT_SOURCE_DIR/examples/$1
  arduino-cli compile --fqbn "$fqbn" "$1.ino"
else
  echo "Skipping $ARDUINO_BOARDS_FILE for $board (incompatible)."
fi
echo
echo

echo ==========================================================================================================
echo Compiling $ARDUINO_INO_FILE \(nodemcuv2\)
echo ==========================================================================================================
board=nodemcuv2
fqbn="esp8266:esp8266:nodemcuv2"
if grep -q "^all$" "$ARDUINO_BOARDS_FILE" || grep -q "^$board$" "$ARDUINO_BOARDS_FILE"; then
  cd $PRODUCT_SOURCE_DIR/examples/$1
  arduino-cli compile --fqbn "$fqbn" "$1.ino"
else
  echo "Skipping $ARDUINO_BOARDS_FILE for $board (incompatible)."
fi
echo
echo

echo ==========================================================================================================
echo Compiling $ARDUINO_INO_FILE \(esp32\)
echo ==========================================================================================================
board=esp32
fqbn="esp32:esp32:esp32"
if grep -q "^all$" "$ARDUINO_BOARDS_FILE" || grep -q "^$board$" "$ARDUINO_BOARDS_FILE"; then
  cd $PRODUCT_SOURCE_DIR/examples/$1
  arduino-cli compile --fqbn "$fqbn" "$1.ino"
else
  echo "Skipping $ARDUINO_BOARDS_FILE for $board (incompatible)."
fi
echo
echo

echo ==========================================================================================================
echo Compiling $ARDUINO_INO_FILE \(esp32wroverkit\)
echo ==========================================================================================================
board=esp32wroverkit
fqbn="esp32:esp32:esp32wroverkit"
if grep -q "^all$" "$ARDUINO_BOARDS_FILE" || grep -q "^$board$" "$ARDUINO_BOARDS_FILE"; then
  cd $PRODUCT_SOURCE_DIR/examples/$1
  arduino-cli compile --fqbn "$fqbn" "$1.ino"
else
  echo "Skipping $ARDUINO_BOARDS_FILE for $board (incompatible)."
fi
echo
echo

cd "$(dirname "$0")"

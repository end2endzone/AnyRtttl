# Any commands which fail will cause the shell script to exit immediately
set -e

# Set arduino-cli config file path
export ARDUINO_CONFIG_PATH="/home/$USER/.arduino15/arduino-cli.yaml"
echo ARDUINO_CONFIG_PATH set to '$ARDUINO_CONFIG_PATH'
cat "$ARDUINO_CONFIG_PATH"
echo.
echo.

echo Adding arduinocli-core-esp8266.yaml to arduino-cli config...
yaml-merge --nostdin arduinocli-core-esp8266.yaml '$ARDUINO_CONFIG_PATH' --overwrite='$ARDUINO_CONFIG_PATH'
cat "$ARDUINO_CONFIG_PATH"
echo
echo

echo Adding arduinocli-core-esp32.yaml to arduino-cli config...
yaml-merge --nostdin arduinocli-core-esp32.yaml '$ARDUINO_CONFIG_PATH' --overwrite='$ARDUINO_CONFIG_PATH'
cat "$ARDUINO_CONFIG_PATH"
echo
echo

echo arduino update-index...
arduino-cli core update-index
echo
echo

echo arduino board install...
arduino-cli board install
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

echo Installing "esp32:esp32" core...
arduino-cli core install "esp32:esp32"
echo
echo

echo Listing all installed cores...
arduino-cli core list
echo
echo

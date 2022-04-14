# Documentation for main.ino

## Functions
- setup()
Default Arduino Function that will run once upon boot, resets the entire system by delegating pinmode, initialising the OLED display etc.

- loop()
Default Arduino Function that will continuously loop after setup function has run, responsible for the main flow of the program. For now, the loop function continuously gets the number from the keypad, and calls the processDisplay() function according to the number input from the keypad. Also continuously gets the temperature from the thermoprobe

- getTemperature()
Function to receive the temperature from the thermoprobe, artificial smoothing is done right now across an average of 5 readings. Might be erroneous now, as we're receiving inconsistent readings from the thermoprobe

- processDisplay(String input)
Function that receives the number input from the user, and switches on / off the relay correspondingly. Responsible for sending a display output to the OLED as well.

## Keypad Inputs
- 1 - Turns all oven components on
- 2 - Toggles the upper heating coil on/off
- 3 - Toggles the lower heating coil on/off
- 4 - Toggles the fan on/off
- 5 - Toggles the rotisserie on/off
- 6 - Toggles the bulb on/off
- 7 - Turns all oven components off
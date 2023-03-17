Preloads the HTML, CSS and preset configurables files into the ESP32 board
^ so need to click Sketch Data Upload before loading the file
^ to apply changes on html or css, need to load SPIFFS with new data before loading

Saving configurations will only save the data into the ESP32's SPIFFS- it cannot be seen (need to look at Arduino Serial Monitor when rebooting to see the changes)
To set up SPIFFS: https://www.tutorialspoint.com/esp32_for_iot/esp32_for_iot_spiffs_storage.htm

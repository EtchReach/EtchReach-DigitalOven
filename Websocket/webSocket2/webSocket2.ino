/*********
  references:
  https://randomnerdtutorials.com/esp32-websocket-server-arduino/#1
  https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/
*********/

// Websocket
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

// Oven
#include <SPI.h>
#include <Wire.h>
#include <Keypad.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_MAX31865.h>

// KEYPAD constants & variables
#define ROW_NUM     4 // four rows
#define COLUMN_NUM  3 // three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte pin_rows[ROW_NUM] = {13, 12, 14, 27}; // GIOP18, GIOP5, GIOP17, GIOP16 connect to the row pins
byte pin_column[COLUMN_NUM] = {26, 25, 33};  // GIOP4, GIOP0, GIOP2 connect to the column pins

using namespace std;

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// OLED constants & variables
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Thermoprobe: Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(05, 23, 19, 18);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

// relay constants & variables
//bool upper = false;
//bool lower = false;
//bool bulb = false;
//bool fan = false;
//bool rotisserie = false;

// 18 yellow fan #4
// 5 purple lower #3
// 4 orange bulb #6
// 2 blue upper #2
// 15 green rotisserie #5

const int upperPin = 2; 
const int lowerPin = 16; 
const int bulbPin = 4; 
const int fanPin = 17; 
const int rotisseriePin = 15; 

// thermoprobe constants & variables
float temp = 0;
unsigned long long prevTempMillis = 0;

//logtime constants
const int timeInterval = 60000;
unsigned long prevTimeMillis = 0;
const long millisToHour = 3600000;
const int millisToMin = 60000;
const int millisToSec = 1000;

// Replace with your network credentials
// const char* ssid = "Wong’s iPhone";
// const char* password = "yufei12345";

// Websocket Settings
const char *ssid = "Xiaomi_7660";
const char *password = "spaghetti";
bool ledState = 0;
const int ledPin = 2;
String temperature1;
String temperature2;
String temperature3;
String temperature4;
String duration1;
String duration2;
String duration3;
String duration4;
String function1;
String function2;
String function3;
String function4;

// Websocket read values
String receivedFunction = "0";
String receivedTemp = "0";
String receivedDuration = "0";

// Websocket send values
unsigned long long last_send = 0;
const int sendInterval = 10000;

// Main loop values
unsigned long long last_time = 0;
unsigned long long current_time = 0;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//void notifyClients()
//{
//  ws.textAll(String(ledState)); // communicates to all clients connected to the server (aka our webserver)
//}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{ // callback to handle the data from clients via websocket protocol
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    Serial.printf("%s\n", (char *)data);
    char* ptr;
    ptr = strtok((char *)data, ",");
    if (strcmp((char *)ptr, "function") == 0) {
      Serial.printf("%s\n", (char *)data);
      Serial.printf("Yes\n"); // This will turn the oven on/off

      receivedFunction = String(strtok(NULL, ","));
      receivedTemp = String(strtok(NULL, ","));
      receivedDuration = String(strtok(NULL, ","));
      last_time = millis();
      last_send = millis();
      
    } else {
      if (strcmp((char *)ptr, "save") == 0) {
        Serial.printf("Saving Configurations\n");
        char * settings= strtok(NULL, ",");
        Serial.printf("%s\n", settings);
        savePresets(settings);
        return;
      }
      while (ptr != NULL) {
        Serial.println(ptr);
        ptr = strtok(NULL, ",");
      }
    }
  }
}

// configure event listener to handle different WebSocket protocol
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA: // when a data packet is received from the client
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

// initialises websocket protocol
void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// Searches for placeholders in HTML and replaces them before sending the webpage to the browser
String processor(const String &var)
{
  Serial.println(var);
  if (var == "STATE")
  {
    if (ledState)
    {
      return "ON";
    }
    else
    {
      return "OFF";
    }
  }
  else if (var == "defaultT1") {
    return temperature1;
  }
  else if (var == "defaultT2") {
    return temperature2;
  }
  else if (var == "defaultT3") {
    return temperature3;
  }
  else if (var == "defaultT4") {
    return temperature4;
  }
  else if (var == "defaultD1") {
    return duration1;
  }
  else if (var == "defaultD2") {
    return duration2;
  }
  else if (var == "defaultD3") {
    return duration3;
  }
  else if (var == "defaultD4") {
    return duration4;
  }
  else if (var == "defaultF1") {
    return function1;
  }
  else if (var == "defaultF2") {
    return function2;
  }
  else if (var == "defaultF3") {
    return function3;
  }
  else if (var == "defaultF4") {
    return function4;
  }
  return String();
}

void savePresets(char * data) {
  File file = SPIFFS.open("/text.txt", "w");
  if (!file) {
    Serial.println("Error opening file for writing");
    return;
  }
  int bytesWritten = file.print(data);
  if (bytesWritten > 0) {
    Serial.println("File was written");
    Serial.println(bytesWritten);
  } else {
    Serial.println("File write failed");
  }
  file.close();
  return;
}

void loadPresets() { //read preset configurations
  File file = SPIFFS.open("/text.txt");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }
  vector<String> v;
  while (file.available()) {
    v.push_back(file.readStringUntil('\n'));
  }
  file.close();
  for (String s : v) {
    Serial.println(s);
  }
  temperature1 = v[0];
  duration1 = v[1];
  function1 = v[2];

  temperature2 = v[3];
  duration2 = v[4];
  function2 = v[5];

  temperature3 = v[6];
  duration3 = v[7];
  function3 = v[8];

  temperature4 = v[9];
  duration4 = v[10];
  function4 = v[11];
}

void setup()
{
  Serial.begin(115200);

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  loadPresets();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());
  
  initWebSocket();
  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Start server
  server.begin();

  // Oven Setup
  pinMode(upperPin, OUTPUT);
  pinMode(lowerPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(rotisseriePin, OUTPUT);
  pinMode(bulbPin, OUTPUT);
  
  digitalWrite(upperPin, HIGH);
  digitalWrite(lowerPin, HIGH);
  digitalWrite(fanPin, HIGH);
  digitalWrite(rotisseriePin, HIGH);
  digitalWrite(bulbPin, HIGH);
  
  Serial.begin(115200);

  thermo.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary, initialising thermoprobe

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.setTextColor(WHITE);
  display.setTextSize(2);  
}

float getTemperature() {
  uint16_t rtd = thermo.readRTD();

//  Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
//  Serial.print("Ratio = "); Serial.println(ratio,8);
//  Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
//  Serial.print("Temperature = "); Serial.println(thermo.temperature(RNOMINAL, RREF));

  // Check and print any faults
  uint8_t fault = thermo.readFault();
  if (fault) {
    Serial.print("Fault 0x"); Serial.println(fault, HEX);
    if (fault & MAX31865_FAULT_HIGHTHRESH) {
      Serial.println("RTD High Threshold"); 
    }
    if (fault & MAX31865_FAULT_LOWTHRESH) {
      Serial.println("RTD Low Threshold"); 
    }
    if (fault & MAX31865_FAULT_REFINLOW) {
      Serial.println("REFIN- > 0.85 x Bias"); 
    }
    if (fault & MAX31865_FAULT_REFINHIGH) {
      Serial.println("REFIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_RTDINLOW) {
      Serial.println("RTDIN- < 0.85 x Bias - FORCE- open"); 
    }
    if (fault & MAX31865_FAULT_OVUV) {
      Serial.println("Under/Over voltage"); 
    }
    thermo.clearFault();
  }
  Serial.println();
  return (thermo.temperature(RNOMINAL, RREF));
}

void monitorTemperature(bool bottomCoil) {
  float readTemp = getTemperature();
  temp = (float)(receivedTemp.toInt());
  if (readTemp < temp) {
    digitalWrite(upperPin, LOW);
    if (bottomCoil) {
      digitalWrite(lowerPin, LOW);
    }
  } else {
    digitalWrite(upperPin, HIGH);
    digitalWrite(lowerPin, HIGH);
  }
}

void resetOven() {
  current_time = millis();
  last_time = millis();
  receivedFunction = "0";
  receivedTemp = "0";
  receivedDuration = "0";
  digitalWrite(upperPin, HIGH);
  digitalWrite(lowerPin, HIGH);
  digitalWrite(fanPin, HIGH);
  digitalWrite(rotisseriePin, HIGH);
  digitalWrite(bulbPin, HIGH);
}

void notifyClients() {
  ws.textAll("temperature," + String(temp) + ",duration," + String((current_time - last_time)/1000));
}

void loop() {
  int duration = receivedDuration.toInt() * 1000;
  int function = receivedFunction.toInt();

  // function != "0" => start oven
  if (function != 0) {
    current_time = millis();

    // function == "1" => Fermentation => Bulb on, Top & Bottom Coils
    // function == "2" => Top & Bottom Heat => Top & Bottom Coils
    // function == "3" => Top Heat with Rotisserie => Rotisserie on, Top Coil
    // function == "4" => Top & Bottom Heat with Fan => Fan on, Top & Bottom Coils
    // function == "5" => Top Heat with Fan & Rotisserie => Fan on & Rotisserie on, Top & Bottom Coils
    
    switch(function) {
      case 1:
        Serial.println("Fermentation");
        digitalWrite(bulbPin, LOW);
        monitorTemperature(true);
        break;

      case 2:
        Serial.println("Top & Bottom Heat");
        monitorTemperature(true);
        break;
        
      case 3:
        Serial.println("Top Heat with Rotisserie");
        digitalWrite(rotisseriePin, LOW);
        monitorTemperature(false);
        break;
        
      case 4:
        Serial.println("Top & Bottom Heat with Fan");
        digitalWrite(fanPin, LOW);
        monitorTemperature(true);
        break;
        
      case 5:
        Serial.println("Top Heat with Fan & Rotisserie");
        digitalWrite(rotisseriePin, LOW);
        digitalWrite(fanPin, LOW);
        monitorTemperature(false);
        break;
        
      default:
        resetOven();
        break;
    }

    if (current_time - last_send >= sendInterval) {
      last_send = current_time;
      notifyClients();
    }
          
    if (current_time - last_time >= duration) {
      resetOven();
    }
  } else { // function == "0" => stop oven
    resetOven();
  }
  
  ws.cleanupClients(); // limits number of clients by closing oldest client when maximum num of client has exceeded - can call once per second to conserve power
}

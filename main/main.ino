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

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

// OLED constants & variables
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Use software SPI: CS, DI, DO, CLK
Adafruit_MAX31865 thermo = Adafruit_MAX31865(05, 23, 19, 18);

// The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

// relay constants & variables
bool upper = false;
bool lower = false;
bool bulb = false;
bool fan = false;
bool rotisserie = false;

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
const int interval = 1000;

float Temp = 0;
unsigned long prevTempMillis = 0;

//logtime constants
const int timeInterval = 60000;
unsigned long prevTimeMillis = 0;
const long millisToHour = 3600000;
const int millisToMin = 60000;
const int millisToSec = 1000;

void setup() {
  // put your setup code here, to run once:
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
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
}

//void loop() {
//  // put your main code here, to run repeatedly:
//  unsigned long long currentMillis = millis();
//  
//  char key = keypad.getKey();
//  String input = String(key);
//  
//  if (key) {
//    Serial.println(key);
//    display.clearDisplay();
//    display.setCursor(0,0);
//    display.print("Key: "+input);
//    display.display();
//    processDisplay(input);
//  }
//
//  // runs every 1s
//  if (currentMillis - prevTempMillis >= interval) {
//    prevTempMillis = currentMillis;
//    float temp = getTemperature();
//    Serial.println("Temperature: " + String(temp));
//  }
//
//  // runs every minute
//  if (currentMillis - prevTimeMillis >= timeInterval) {
//    prevTimeMillis = currentMillis;
//    logTime();
//  }
//}

void loop() {
  display.setCursor(0,0);
  display.print("Key: 1");
  display.display();
}

//void loop() {
//    digitalWrite(upperPin, LOW);
//    Serial.println("Upper Pin");
//    delay(2000);
//    digitalWrite(lowerPin, LOW);
//    Serial.println("Lower Pin");
//    delay(2000);
//    digitalWrite(fanPin, LOW);
//    Serial.println("Fan Pin");
//    delay(2000);
//    digitalWrite(rotisseriePin, LOW);
//    Serial.println("Rotisserie Pin");
//    delay(2000);
//    digitalWrite(bulbPin, LOW);
//    Serial.println("Bulb Pin");
//    delay(2000);
//    Serial.println("All off");
//    digitalWrite(upperPin, HIGH);
//    digitalWrite(lowerPin, HIGH);
//    digitalWrite(fanPin, HIGH);
//    digitalWrite(rotisseriePin, HIGH);
//    digitalWrite(bulbPin, HIGH);
//    delay(2000);
//}

void logTime() {
  unsigned long long currentMillis = millis();
  int hour = currentMillis / millisToHour;
  int hourRemainder = currentMillis % millisToHour;
  int mins = hourRemainder / millisToMin;
  int minRemainder = hourRemainder % millisToMin;
  int secs = minRemainder / millisToSec;
  int remainder = minRemainder % millisToSec;

  Serial.println("Time: " + String(hour) + "h " + String(mins) + "mins " + String(secs) + "s " + String(remainder) + "ms");
}

float getTemperature() {
  uint16_t rtd = thermo.readRTD();

  Serial.print("RTD value: "); Serial.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
  Serial.print("Ratio = "); Serial.println(ratio,8);
  Serial.print("Resistance = "); Serial.println(RREF*ratio,8);
  Serial.print("Temperature = "); Serial.println(thermo.temperature(RNOMINAL, RREF));

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


void processDisplay(String input) {
  // input 1: turn everything on
  // input 2: toggle upper coil
  // input 3: toggle lower coil
  // input 4: toggle fan
  // input 5: toggle rotisserie
  // input 6: toggle bulb
  // input 7: turn everything off
  
  if (input == "1"){
      digitalWrite(upperPin, LOW);
      digitalWrite(lowerPin, LOW);
      digitalWrite(fanPin, LOW);
      digitalWrite(rotisseriePin, LOW);
      digitalWrite(bulbPin, LOW);
      upper = false;
      lower = false;
      bulb = false;
      fan = false;
      rotisserie = false;
      Serial.println("on!");
    }
      
    if (input == "2"){
      upper = !upper;
      digitalWrite(upperPin, upper);
      Serial.println("upper! : "+String(upper));
    }
      
    if (input == "3"){
      lower = !lower;
      digitalWrite(lowerPin, lower);
      Serial.println("lower! : "+String(lower));
    }
      
    if (input == "4") {
      fan = !fan;
      digitalWrite(fanPin, fan);
      Serial.println("fan! : "+String(fan));
    }
      
    if (input == "5"){
      rotisserie = !rotisserie;
      digitalWrite(rotisseriePin, rotisserie);
      Serial.println("rotisserie! : "+String(rotisserie));
    }

    if (input == "6"){
      bulb = !bulb;
      digitalWrite(bulbPin, bulb);
      Serial.println("bulb! : "+String(bulb));
    }
      
    if (input == "7"){
      digitalWrite(upperPin, HIGH);
      digitalWrite(lowerPin, HIGH);
      digitalWrite(fanPin, HIGH);
      digitalWrite(rotisseriePin, HIGH);
      digitalWrite(bulbPin, HIGH);
      Serial.println("off!");
      upper = true;
      lower = true;
      bulb = true;
      fan = true;
      rotisserie = true;
    }
    if (input == "8"){
      unsigned long long timeStart = millis();
      unsigned long long timeNow = millis();
      unsigned long long preTime = 0;
      digitalWrite(upperPin, LOW);
      digitalWrite(lowerPin, LOW);
      while (timeNow-timeStart <  3600000){ // 60min
//      while (true) {
        char key = keypad.getKey();
        if (key) {
          Serial.println("Key: " + String(key));
          Serial.println("Aborting test...");
          break;
        }
        Temp = getTemperature();
        if (Temp < 230){
//           turn oven on when temp is < 150
            digitalWrite(upperPin, LOW);
            digitalWrite(lowerPin, LOW);
        } else {
          digitalWrite(upperPin, HIGH);
          digitalWrite(lowerPin, HIGH);
        }
        if (timeNow - preTime >= timeInterval) {
          preTime = timeNow;
          logTime();
        }
        timeNow = millis();
        delay(1000);
      }
    }
    }
//}

#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
const int lowerPin = 5; 
const int bulbPin = 4; 
const int fanPin = 18; 
const int rotisseriePin = 15; 

// thermoprobe constants & variables
const int analogInPin = 34; //sensor connected to A5 pin
const int interval = 100;
const float m = 0.208602;
const float c = -458.66;
const int numReadings = 5;
float readings[numReadings];
int readIndex = 0;
float total = 0;
float average = 0;

int sensorValue = 0;
float Temp = 0;
unsigned long prevTempMillis = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(analogInPin, INPUT);
  
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
  
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.setTextColor(WHITE);
  display.setTextSize(2);

  // Reset Temperature array
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentMillis = millis();
  
  char key = keypad.getKey();
  String input = String(key);
  
  if (key) {
    Serial.println(key);
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Key: "+input);
    display.display();
    processDisplay(input);
  }

  if (currentMillis - prevTempMillis >= interval) {
    prevTempMillis = currentMillis;
    getTemperature();
  }
}

void getTemperature(){
  sensorValue = analogRead(analogInPin);
  Temp = m*sensorValue + c;
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = Temp;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;
  Serial.println(readIndex);
  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;

  Serial.println("Sensor Value: "+String(sensorValue)); //printing value of sensor on the serial monitor
  Serial.println("Temperature: "+String(average)); // printing temperature on the serial monitor
}

void processDisplay(String input) {
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
}

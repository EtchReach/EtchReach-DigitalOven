/*********
  references:
  https://randomnerdtutorials.com/esp32-websocket-server-arduino/#1
  https://randomnerdtutorials.com/esp32-web-server-spiffs-spi-flash-file-system/
*********/

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

using namespace std;

// Replace with your network credentials
// const char* ssid = "Wong’s iPhone";
// const char* password = "yufei12345";
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

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void notifyClients()
{
  ws.textAll(String(ledState)); // communicates to all clients connected to the server (aka our webserver)
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{ // callback to handle the data from clients via websocket protocol
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
    data[len] = 0;
    if (strcmp((char *)data, "toggle") == 0)
    {
      Serial.printf("%s\n", (char *)data);
      Serial.printf("Yes\n"); // This will turn the oven on/off
      ledState = !ledState;
      notifyClients();
    }
    else
    {
      char *ptr;
      ptr = strtok((char *)data, ",");
      while (ptr != NULL)
      {
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
//used to load diff preset configurations in
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
  else if (var=="defaultT1"){
    return temperature1;
  }
  else if (var=="defaultT2"){
    return temperature2;
  }
  else if (var=="defaultT3"){
    return temperature3;
  }
  else if (var=="defaultT4"){
    return temperature4;
  }
  else if (var=="defaultD1"){
    return duration1;
  }
  else if (var=="defaultD2"){
    return duration2;
  }
  else if (var=="defaultD3"){
    return duration3;
  }
  else if (var=="defaultD4"){
    return duration4;
  }
  return String();
}

void loadPresets(){ //read preset configurations
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
  temperature1= v[0];
  duration1= v[1];
  temperature2= v[2];
  duration2= v[3];
  temperature3= v[4];
  duration3= v[5];
  temperature4= v[6];
  duration4= v[7];
}

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
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
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Start server
  server.begin();
}

void loop()
{
  ws.cleanupClients(); // limits number of clients by closing oldest client when maximum num of client has exceeded - can call once per second to conserve power
  digitalWrite(ledPin, ledState);
}

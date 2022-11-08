/*********
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-websocket-server-arduino/
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "Xiaomi_7660";
const char* password = "spaghetti";

bool ledState = 0;
const int ledPin = 2;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .buttons{
    display:flex;
    flex-direction: column;
    gap:20px;
    margin-top:50px;
    
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
   
   .buttonInner{
    display:flex;
    flex-direction: column;
   }
   .firstSet{
    display:flex;
    flex-direction: column;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>ESP WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - GPIO 2</h2>
      <p class="state">state: <span id="state">%STATE%</span></p>
      <p><button id="button" class="button">Toggle</button></p>
    </div>

    <div class="buttons">
      <div class="toprow">
        <div class="firstSet">
          <div class="first">
            <div class="buttonInner">
              <Text> 1 </Text>
              <h2>Temperature</p>
              <input disabled id= "temperature1" type="number" min="1" max="100">
              <h2>Duration</h2>
              <input disabled id= "duration1" type="number" min="1" max="100">
              <h2>Function</h2>
              <input disabled id= "function1" type="number" min="1" max="5">
              <h2></h2>
            </div>
          </div>
            <button id="setting1" class="settingButton">Setting</button>
            <button id="firstbutton" class="settingButton">Toggle</button>
        </div>
        <div class="firstSet">
          <div class="second">
            <div class="buttonInner">
              <Text> 2 </Text>
              <h2>Temperature</p>
              <input disabled id= "temperature2" type="number" min="1" max="100">
              <h2>Duration</h2>
              <input disabled id= "duration2" type="number" min="1" max="100">
              <h2>Function</h2>
              <input disabled id= "function2" type="number" min="1" max="5">
              <h2></h2>
            </div>
          </div>
            <button id="setting2" class="settingButton">Setting</button>
            <button id="secondbutton" class="settingButton">Toggle</button>
        </div>
      </div>  
        </div>
        </div>
      <div class="bottomrow">
        <div class="firstSet">
          <div class="third">
            <div class="buttonInner">
              <Text> 3 </Text>
              <h2>Temperature</p>
              <input disabled id= "temperature3" type="number" min="1" max="100">
              <h2>Duration</h2>
              <input disabled id= "duration3" type="number" min="1" max="100">
              <h2>Function</h2>
              <input disabled id= "function3" type="number" min="1" max="5">
              <h2></h2>
            </div>
          </div>
            <button id="setting3" class="settingButton">Setting</button>
            <button id="thirdbutton" class="settingButton">Toggle</button>
        </div>
        <div class="firstSet">
          <div class="fourth">
            <div class="buttonInner">
              <Text> 4 </Text>
              <h2>Temperature</p>
              <input disabled id= "temperature4" type="number" min="1" max="100">
              <h2>Duration</h2>
              <input disabled id= "duration4" type="number" min="1" max="100">
              <h2>Function</h2>
              <input disabled id= "function4" type="number" min="1" max="5">
              <h2></h2>
            </div>
          </div>
            <button id="setting4" class="settingButton">Setting</button>
            <button id="fourthbutton" class="settingButton">Toggle</button>
        </div>
      </div>
  
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}/ws`;
  var websocket;
  let temperatureInput1 = true;
  let temperatureInput2 = true;
  let temperatureInput3 = true;
  let temperatureInput4 = true;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
    if (event.data == "1"){
      state = "ON";
    }
    else{
      state = "OFF";
    }
    document.getElementById('state').innerHTML = state;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
    document.getElementById('firstbutton').addEventListener('click', toggle1);
    document.getElementById('secondbutton').addEventListener('click', toggle2);
    document.getElementById('thirdbutton').addEventListener('click', toggle3);
    document.getElementById('fourthbutton').addEventListener('click', toggle4);

    document.getElementById('setting1').addEventListener('click', () => {
      temperatureInput1 = !temperatureInput1;
      document.getElementById('temperature1').disabled= temperatureInput1;
      document.getElementById('duration1').disabled= temperatureInput1;
      document.getElementById('function1').disabled= temperatureInput1;
    });
    document.getElementById('setting2').addEventListener('click', () => {
      temperatureInput2 = !temperatureInput2;
      document.getElementById('temperature2').disabled= temperatureInput2;
      document.getElementById('duration2').disabled= temperatureInput2;
      document.getElementById('function2').disabled= temperatureInput2;
    });
    document.getElementById('setting3').addEventListener('click', () => {
      temperatureInput3 = !temperatureInput3;
      document.getElementById('temperature3').disabled= temperatureInput3;
      document.getElementById('duration3').disabled= temperatureInput3;
      document.getElementById('function3').disabled= temperatureInput3;
    });
    document.getElementById('setting4').addEventListener('click', () => {
      temperatureInput4 = !temperatureInput4;
      document.getElementById('temperature4').disabled= temperatureInput4;
      document.getElementById('duration4').disabled= temperatureInput4;
      document.getElementById('function4').disabled= temperatureInput4;
    });
  }
  function toggle(){
    console.log("toggle");
    websocket.send("toggle");
  }
  function toggle1(){
    var temp = document.getElementById("temperature1").value;
    var duration = document.getElementById("duration1").value;
    var func = document.getElementById("function1").value;
    var value= temp+','+duration+','+func
    console.log(value);
    websocket.send(value);
  }
  function toggle2(){
    var temp = document.getElementById("temperature2").value;
    var duration = document.getElementById("duration2").value;
    var func = document.getElementById("function2").value;
    var value= temp+','+duration+','+func
    console.log(value);
    websocket.send(value);
  }
  function toggle3(){
    var temp = document.getElementById("temperature3").value;
    var duration = document.getElementById("duration3").value;
    var func = document.getElementById("function3").value;
    var value= temp+','+duration+','+func
    console.log(value);
    websocket.send(value);
  }
  function toggle4(){
    var temp = document.getElementById("temperature4").value;
    var duration = document.getElementById("duration4").value;
    var func = document.getElementById("function4").value;
    var value= temp+','+duration+','+func
    console.log(value);
    websocket.send(value);
  }

</script>
</body>
</html>
)rawliteral";

void notifyClients() {
  ws.textAll(String(ledState));
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggle") == 0) {
      Serial.printf("%s\n",(char*)data);
      Serial.printf("Yes\n"); // This will turn the oven on/off
      ledState = !ledState;
      notifyClients();
    }
    else{
       char* ptr;
       ptr = strtok((char*)data,",");
       while(ptr!=NULL){
        Serial.println(ptr);
        ptr=strtok(NULL,",");
       }      
    }


  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("WebSocket client #%u disconnected\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  initWebSocket();

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });

  // Start server
  server.begin();
}

void loop() {
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
}

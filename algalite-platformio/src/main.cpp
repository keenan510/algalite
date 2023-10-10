/*
This is the control code to run the AlgaLite project. 

The main features are
- Control the LEDs
- Control the Air Pump
- Read data from the colour sensor

*/


#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPI.h>


// Replace with your network credentials
const char* ssid = "WiFimodem-DE58";
const char* password = "qjmx2mlfdz";

// Define pin numbers and other constants
const int neoPixelPin = 33;
const int airPumpPin = 25;
const int onboardLedPin = 32; 
#define I2C_SDA 23
#define I2C_SCL 19

Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, neoPixelPin, NEO_GRB + NEO_KHZ800);



// Instantiate the TCS3472 object
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_300MS, TCS34725_GAIN_1X);
//Adafruit_TCS34725 tcs = Adafruit_TCS34725();

AsyncWebServer server(80);

//----------------------------------------------------------
String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
  body {font-family: Arial, sans-serif; text-align: center;}
  .subtitle {text-align: left; font-weight: bold;}
  button {font-size: 1.5em; margin: 5px; padding: 10px; display: inline-block;}
  .color-buttons {text-align: center;}  /* Center the color buttons */
</style>
</head>
<body>

<h1>AlgaLite Controller</h1>

<div class="subtitle">Control Airflow:</div>
<button onclick="sendCommand('pumpToggle')">Toggle Air Pump</button>
<br>

<div class="subtitle">Control Lamp Colour:</div>
<div class="color-buttons">
  <button onclick="sendCommand('red')">Red</button>
  <button onclick="sendCommand('green')">Green</button>
  <button onclick="sendCommand('blue')">Blue</button>
  <button onclick="sendCommand('cyan')">Cyan</button>
  <button onclick="sendCommand('magenta')">Magenta</button>
  <button onclick="sendCommand('yellow')">Yellow</button>
  <button onclick="sendCommand('off')">Off</button>
</div>
<br>

<div class="subtitle">Measure Algae Thickness:</div>
<button onclick="sendCommand('measure')">Measure</button>
<div id="measurementResult"></div>

<script>
  function sendCommand(command) {
    fetch("/cmd?command=" + command)
    .then(response => response.text())
    .then(data => {
      if (command === 'measure') {
        document.getElementById('measurementResult').innerText = data;
      }
    });
  }
</script>

</body>
</html>
)rawliteral";
//----------------------------------------------------------
void handleWebCommand(String command);
void measureAlgaeThickness(String &result);

void setup() {
  Serial.begin(9600);
  while(!Serial) { }

  strip.begin();
  strip.show();
  pinMode(airPumpPin, OUTPUT);
  pinMode(onboardLedPin, OUTPUT);

  Wire.begin(I2C_SDA, I2C_SCL, 100000);
  if (!tcs.begin()) {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", htmlPage);
  });

  
server.on("/cmd", HTTP_GET, [](AsyncWebServerRequest *request){
  String command = request->getParam("command")->value();
  String result;
  if (command == "measure") {
    measureAlgaeThickness(result);
    request->send(200, "text/plain", result);
  } else {
    handleWebCommand(command);
    request->send(200, "text/plain", "OK");
  }
});

  server.begin();
}




void setColor(uint8_t red, uint8_t green, uint8_t blue) {
  // Set all pixels to the specified color
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(red, green, blue));
  }
  strip.show();
}

void handleWebCommand(String command) {
  if (command == "pumpToggle") {
    // Toggle air pump
    static bool pumpState = false;  // Keeps track of air pump state
    pumpState = !pumpState;  // Toggle state
    digitalWrite(airPumpPin, pumpState ? HIGH : LOW);  // Set air pump state
  } 
  else if (command == "ledToggle") {
    // Toggle onboard LED
    static bool ledState = false;  // Keeps track of onboard LED state
    ledState = !ledState;  // Toggle state
    digitalWrite(onboardLedPin, ledState ? HIGH : LOW);  // Set onboard LED state
  } 
  else if (command == "red") {
    // Set NeoPixels to red
    setColor(255, 0, 0);  // Red color
  } 
  else if (command == "green") {
    // Set NeoPixels to green
    setColor(0, 255, 0);  // Green color
  } 
  else if (command == "blue") {
    // Set NeoPixels to blue
    setColor(0, 0, 255);  // Blue color
  } 
  else if (command == "magenta") {
    // Set NeoPixels to magenta
    setColor(255, 0, 255);  // Magenta color
  } 
  else if (command == "cyan") {
    // Set NeoPixels to cyan
    setColor(0, 255, 255);  // Cyan color
  } 
  else if (command == "yellow") {
    // Set NeoPixels to yellow
    setColor(255, 255, 0);  // Yellow color
  }
  else if (command == "off") {
    // Set NeoPixels to off
    setColor(0, 0, 0);  // Off
  }
}

void measureAlgaeThickness(String &result) {
  digitalWrite(onboardLedPin, HIGH);  // Turn the onboard LED on

  uint16_t r, g, b, c, colorTemp, lux;
  
  delay(500);  // Allow some time for the sensor to stabilize (if necessary)
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  digitalWrite(onboardLedPin, LOW);  // Turn the onboard LED off

  // Construct a String to hold the formatted measurement result
   result = "Lux: " + String(lux) + " - "
                  "R: " + String(r) + " G: " + String(g) + " B: " + String(b) + " C: " + String(c);
  

}

void loop() {
  
}

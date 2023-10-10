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
#include <SPI.h>


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

unsigned long previousMillisPump = 0;
unsigned long previousMillisLED = 0;
unsigned long previousMillisColor = 0;
unsigned long previousMillisNeoPixel = 0;

const long intervalPump = 600000;  // 10 minutes in milliseconds
const long intervalLED = 7200000;  // 2 hours in milliseconds
const long intervalColor = 7200000;  // 2 hours in milliseconds (same as LED)


void setup() {
  Serial.begin(9600);
  while(!Serial) { }

  Serial.println(F("Setup started"));

  strip.begin();
  strip.show();
  pinMode(airPumpPin, OUTPUT);
  pinMode(onboardLedPin, OUTPUT);  // Set the onboard LED pin as output

  Serial.println(F("Pin modes set"));

  Wire.begin(I2C_SDA, I2C_SCL, 100000);

if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  Serial.println(F("TCS3472 OK"));
  Serial.println(F("Setup completed"));
}

void handleLighting() {
  unsigned long currentMillis = millis();
  //int hour = hour();
  //if (hour >= 18 && hour < 22) {  // Check if the current time is between 6 PM and 10 PM
  // Simple example: Cycle through basic colors red, green, blue
  Serial.println(F("Handling lighting"));
  for(int i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(200,0,200));  // Red color
  }
  strip.show();
  delay(1000);
  } 
  /*
  else {
    strip.clear();  // Turn off all pixels when outside the 6 PM to 10 PM window
    strip.show();
  }
*/

void testAirPump() {
  // Simple example: Turn air pump on for 2 seconds, then off for 2 seconds
  Serial.println(F("Handling air pump"));
  digitalWrite(airPumpPin, HIGH);  // Turn air pump on
  delay(500);
  digitalWrite(airPumpPin, LOW);   // Turn air pump off
  delay(500);
}

void handleAirPump() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisPump >= intervalPump) {
    previousMillisPump = currentMillis;
    digitalWrite(airPumpPin, HIGH);  // Turn air pump on
    delay(60000);  // Keep air pump on for 1 minute
    digitalWrite(airPumpPin, LOW);   // Turn air pump off
  }
}

void readColorSensor() {
  uint16_t r, g, b, c, colorTemp, lux;

  delay(500);  
  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print("Lux: "); Serial.print(lux, DEC); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" ");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" ");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" ");
  Serial.println(" ");
  delay(5000);  // Example: keep the onboard LED off for 5 seconds
  //digitalWrite(onboardLedPin, LOW);   // Turn the onboard LED off
  
}


void handleOnboardLed() {
  Serial.println(F("Handling onboard LED"));
  digitalWrite(onboardLedPin, HIGH);  // Turn the onboard LED on
  delay(5000);  // Example: keep the onboard LED on for 5 seconds
  digitalWrite(onboardLedPin, LOW);   // Turn the onboard LED off
  delay(5000);  // Example: keep the onboard LED off for 5 seconds
}

void handleOnboardLedAndColorSensor() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillisLED >= intervalLED) {
    previousMillisLED = currentMillis;
    digitalWrite(onboardLedPin, HIGH);  // Turn the onboard LED on
    readColorSensor();  // Take color sensor reading while onboard LED is on
    digitalWrite(onboardLedPin, LOW);   // Turn the onboard LED off
  }
}

void loop() {
  Serial.println(F("Loop started"));
  handleLighting();
  delay(1000);
  testAirPump();
  delay(1000);
  handleOnboardLed();
  readColorSensor();
  //handleOnboardLedAndColorSensor();
  Serial.println(F("Loop ended"));
  delay(1000);
}

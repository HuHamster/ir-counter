#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen dimensions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pins for sensors
#define SENSOR_A_PIN 2 // First sensor (e.g., closer to the entrance)
#define SENSOR_B_PIN 3 // Second sensor (e.g., further from the entrance)

// Counter and states
int peopleCount = 0;
bool sensorAState = true;
bool sensorBState = true;
bool isGoingIn = false;
bool isGoingOut = false;
bool isCounting = false;

void setup() {
  Serial.begin(9600);
  
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  // Set sensor pins as inputs with pull-up resistors
  pinMode(SENSOR_A_PIN, INPUT_PULLUP);
  pinMode(SENSOR_B_PIN, INPUT_PULLUP);
}

void loop() {
  sensorAState = digitalRead(SENSOR_A_PIN);
  sensorBState = digitalRead(SENSOR_B_PIN);

  // Logic to determine direction of movement
  if (!sensorAState && sensorBState && !isCounting) {
    isGoingIn = true;
    isCounting = true;
  }
  
  if (sensorAState && !sensorBState && !isCounting) {
    isGoingOut = true;
    isCounting = true;
  }

  // Finalize entry movement
  if (!sensorAState && !sensorBState && isGoingIn) {
    peopleCount++;
    isGoingIn = false;
    isCounting = false;
  }
  
  // Finalize exit movement
  if (!sensorAState && !sensorBState && isGoingOut) {
    peopleCount--;
    isGoingOut = false;
    isCounting = false;
  }
  
  // Reset logic if no movement is detected
  if (sensorAState && sensorBState && (isGoingIn || isGoingOut)) {
    isGoingIn = false;
    isGoingOut = false;
    isCounting = false;
  }

  // Update the display to show the number centered
  display.clearDisplay();
  display.setTextSize(6); // Large font size
  display.setTextColor(SSD1306_WHITE);
  
  // Get text bounds for centering
  int16_t x, y;
  uint16_t w, h;
  display.getTextBounds(String(peopleCount), 0, 0, &x, &y, &w, &h);

  // Calculate coordinates to center the text
  display.setCursor((display.width() - w) / 2, (display.height() - h) / 2);
  
  display.println(peopleCount);
  display.display();
  
  delay(100);
}
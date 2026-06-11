#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

// Pin configuration
#define LED_PIN D4
#define LED_COUNT 60

// Serial protocol
#define SERIAL_BAUD 115200
#define BUFFER_SIZE 256

// NeoPixel setup
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

// Global state
struct LEDState {
  uint8_t r, g, b;
  uint8_t brightness;
  uint8_t mode;
} ledState = {255, 0, 0, 255, 0};

// Buffer for serial commands
char serialBuffer[BUFFER_SIZE];
int bufferIndex = 0;

// Function declarations
void handleCommand(const char* cmd);
void setColor(uint8_t r, uint8_t g, uint8_t b);
void setBrightness(uint8_t brightness);
void setMode(uint8_t mode);
void displayColor();
void sendAck();
void sendError(const char* msg);

void setup() {
  Serial.begin(SERIAL_BAUD);
  strip.begin();
  strip.show();
  
  Serial.println("{\"status\":\"ready\",\"device\":\"capellan-led-d1\",\"leds\":" + String(LED_COUNT) + "}");
}

void loop() {
  // Read serial commands
  while (Serial.available()) {
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {
      if (bufferIndex > 0) {
        serialBuffer[bufferIndex] = '\0';
        handleCommand(serialBuffer);
        bufferIndex = 0;
      }
    } else if (bufferIndex < BUFFER_SIZE - 1) {
      serialBuffer[bufferIndex++] = c;
    }
  }
}

void handleCommand(const char* cmd) {
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, cmd);
  
  if (error) {
    sendError("JSON parse error");
    return;
  }
  
  const char* action = doc["action"];
  
  if (strcmp(action, "color") == 0) {
    uint8_t r = doc["r"] | 255;
    uint8_t g = doc["g"] | 0;
    uint8_t b = doc["b"] | 0;
    setColor(r, g, b);
    sendAck();
  }
  else if (strcmp(action, "brightness") == 0) {
    uint8_t brightness = doc["value"] | 255;
    setBrightness(brightness);
    sendAck();
  }
  else if (strcmp(action, "mode") == 0) {
    uint8_t mode = doc["value"] | 0;
    setMode(mode);
    sendAck();
  }
  else if (strcmp(action, "off") == 0) {
    setColor(0, 0, 0);
    sendAck();
  }
  else if (strcmp(action, "status") == 0) {
    StaticJsonDocument<256> response;
    response["r"] = ledState.r;
    response["g"] = ledState.g;
    response["b"] = ledState.b;
    response["brightness"] = ledState.brightness;
    response["mode"] = ledState.mode;
    response["leds"] = LED_COUNT;
    serializeJson(response, Serial);
    Serial.println();
  }
  else {
    sendError("Unknown action");
  }
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  ledState.r = r;
  ledState.g = g;
  ledState.b = b;
  displayColor();
}

void setBrightness(uint8_t brightness) {
  ledState.brightness = brightness;
  strip.setBrightness(brightness);
  displayColor();
}

void setMode(uint8_t mode) {
  ledState.mode = mode;
  // Mode implementation here (rainbow, pulse, etc.)
}

void displayColor() {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(
      (ledState.r * ledState.brightness) / 255,
      (ledState.g * ledState.brightness) / 255,
      (ledState.b * ledState.brightness) / 255
    ));
  }
  strip.show();
}

void sendAck() {
  Serial.println("{\"ack\":true}");
}

void sendError(const char* msg) {
  Serial.print("{\"error\":\"");
  Serial.print(msg);
  Serial.println("\"}");
}

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Adafruit_NeoPixel.h>

// ==================== CONFIGURACIÓN ====================
#define LED_PIN D4              // Pin GPIO2 para datos LED
#define NUM_LEDS 60             // Cantidad de LEDs WS2812
#define SERIAL_BAUD 115200      // Velocidad Serial

// ==================== VARIABLES GLOBALES ====================
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

struct Color {
  uint8_t r, g, b;
};

struct Config {
  uint8_t brightness = 255;
  bool enabled = true;
  Color currentColor = {255, 0, 0};  // Rojo por defecto
};

Config config;

// ==================== FUNCIONES DE LED ====================

void setupLED() {
  strip.begin();
  strip.show();
  strip.setBrightness(config.brightness);
  Serial.println("LED initialized");
}

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  config.currentColor = {r, g, b};
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

void setBrightness(uint8_t brightness) {
  config.brightness = brightness;
  strip.setBrightness(brightness);
  strip.show();
}

void rainbow(uint8_t wait) {
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) {
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();
    delay(wait);
  }
}

void pulse(uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
  for (int brightness = 0; brightness <= 255; brightness += speed) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(
        (r * brightness) / 255,
        (g * brightness) / 255,
        (b * brightness) / 255
      ));
    }
    strip.show();
    delay(20);
  }
  for (int brightness = 255; brightness >= 0; brightness -= speed) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(
        (r * brightness) / 255,
        (g * brightness) / 255,
        (b * brightness) / 255
      ));
    }
    strip.show();
    delay(20);
  }
}

void theater(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  for (int a = 0; a < 10; a++) {
    for (int b = 0; b < 3; b++) {
      strip.clear();
      for (int c = b; c < strip.numPixels(); c += 3) {
        strip.setPixelColor(c, strip.Color(r, g, b));
      }
      strip.show();
      delay(wait);
    }
  }
}

void allOff() {
  strip.clear();
  strip.show();
}

// ==================== PROTOCOLO SERIAL ====================

void processCommand(String cmd) {
  int delimiterIndex = cmd.indexOf('|');
  
  if (delimiterIndex == -1) {
    Serial.println("ERROR|Invalid format");
    return;
  }

  String command = cmd.substring(0, delimiterIndex);
  String params = cmd.substring(delimiterIndex + 1);

  if (command == "COLOR") {
    int r = 0, g = 0, b = 0;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    
    r = params.substring(0, p1).toInt();
    g = params.substring(p1 + 1, p2).toInt();
    b = params.substring(p2 + 1).toInt();
    
    setColor(r, g, b);
    Serial.println("OK|Color changed");
  }
  else if (command == "BRIGHTNESS") {
    uint8_t brightness = params.toInt();
    setBrightness(brightness);
    Serial.println("OK|Brightness updated");
  }
  else if (command == "RAINBOW") {
    uint8_t speed = params.toInt();
    rainbow(speed);
    Serial.println("OK|Rainbow effect");
  }
  else if (command == "PULSE") {
    int r = 0, g = 0, b = 0;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    
    r = params.substring(0, p1).toInt();
    g = params.substring(p1 + 1, p2).toInt();
    b = params.substring(p2 + 1, p3).toInt();
    uint8_t speed = params.substring(p3 + 1).toInt();
    
    pulse(r, g, b, speed);
    Serial.println("OK|Pulse effect");
  }
  else if (command == "THEATER") {
    int r = 0, g = 0, b = 0;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    
    r = params.substring(0, p1).toInt();
    g = params.substring(p1 + 1, p2).toInt();
    b = params.substring(p2 + 1, p3).toInt();
    uint8_t speed = params.substring(p3 + 1).toInt();
    
    theater(r, g, b, speed);
    Serial.println("OK|Theater effect");
  }
  else if (command == "OFF") {
    allOff();
    Serial.println("OK|All LEDs off");
  }
  else if (command == "STATUS") {
    String status = "OK|";
    status += "R:" + String(config.currentColor.r) + "|";
    status += "G:" + String(config.currentColor.g) + "|";
    status += "B:" + String(config.currentColor.b) + "|";
    status += "BRIGHTNESS:" + String(config.brightness) + "|";
    status += "LEDS:" + String(NUM_LEDS);
    Serial.println(status);
  }
  else if (command == "HELP") {
    Serial.println("COMMANDS:");
    Serial.println("  COLOR|R|G|B - Set color (0-255 each)");
    Serial.println("  BRIGHTNESS|0-255 - Set brightness");
    Serial.println("  RAINBOW|speed - Rainbow effect");
    Serial.println("  PULSE|R|G|B|speed - Pulse effect");
    Serial.println("  THEATER|R|G|B|speed - Theater effect");
    Serial.println("  OFF - Turn all off");
    Serial.println("  STATUS - Get current status");
  }
  else {
    Serial.println("ERROR|Unknown command: " + command);
  }
}

// ==================== SETUP ====================

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  
  Serial.println("\n\n==================== CAPELLÁN LED ====================");
  Serial.println("Firmware v1.0.0");
  Serial.println("ESP8266 D1 Mini - WS2812 LED Controller");
  Serial.println("====================================================\n");
  
  setupLED();
  setColor(255, 0, 0); // Rojo inicial
  
  Serial.println("Ready! Type 'HELP' for commands");
}

// ==================== LOOP ====================

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command.length() > 0) {
      Serial.print("CMD: ");
      Serial.println(command);
      processCommand(command);
    }
  }
  
  delay(10);
}

#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Adafruit_NeoPixel.h>

// ==================== CONFIGURACIÓN ====================
// Usar pin de datos 2 (GPIO2) para todos los dispositivos (ESP8266/ESP32/C3/S3)
#define LED_PIN 2              // Pin GPIO2 para datos LED
#define NUM_LEDS 60            // Cantidad de LEDs WS2812
#define SERIAL_BAUD 115200     // Velocidad Serial

// Demo settings
#define DEMO_TOTAL_MS 20000    // Duración total del demo automático al subir el código (20s)
#define DEMO_STEP_MS 4000      // Duración de cada paso/efecto (4s)

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

// Demo runtime
unsigned long demoStartTime = 0;
bool demoRunning = true;

// ==================== FUNCIONES DE LED ====================

void setupLED() {
  strip.begin();
  strip.show();
  strip.setBrightness(config.brightness);
  Serial.println("LED initialized on data pin: " + String(LED_PIN));
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
    for (int b_iter = 0; b_iter < 3; b_iter++) {
      strip.clear();
      for (int c = b_iter; c < strip.numPixels(); c += 3) {
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

// ==================== DEMO AUTOMÁTICO AL SUBIR CÓDIGO ====================

void runDemoStep(int step) {
  switch (step) {
    case 0:
      setColor(255, 0, 0); // Rojo
      break;
    case 1:
      setColor(0, 255, 0); // Verde
      break;
    case 2:
      setColor(0, 0, 255); // Azul
      break;
    case 3:
      rainbow(10); // Arcoiris (bloqueante corto)
      break;
    case 4:
      pulse(255, 128, 0, 10); // Pulso naranja
      break;
    case 5:
      theater(0, 255, 255, 40); // Theater cian
      break;
    default:
      setColor(255, 255, 255); // Blanco
      break;
  }
}

void runDemoNonBlocking() {
  unsigned long now = millis();
  unsigned long elapsed = now - demoStartTime;
  if (elapsed >= DEMO_TOTAL_MS) {
    demoRunning = false;
    allOff();
    Serial.println("Demo complete");
    return;
  }

  int step = (elapsed / DEMO_STEP_MS);
  int localStep = step % 6; // tenemos 6 pasos visibles

  // Ejecutar efectos que no bloquean mucho; for blocking effects we call small functions
  switch (localStep) {
    case 0:
      setColor(255, 0, 0);
      break;
    case 1:
      setColor(0, 255, 0);
      break;
    case 2:
      setColor(0, 0, 255);
      break;
    case 3:
      // rainbow iterates internally; call a short iteration
      for (int i = 0; i < 64; i++) {
        int hue = (i * 65536L / strip.numPixels());
        for (int p = 0; p < strip.numPixels(); p++) {
          strip.setPixelColor(p, strip.gamma32(strip.ColorHSV(hue + p * 256)));
        }
        strip.show();
        delay(5);
      }
      break;
    case 4:
      pulse(255, 128, 0, 12);
      break;
    case 5:
      theater(0, 255, 255, 40);
      break;
  }
}

// ==================== PROTOCOLO SERIAL ====================

void processCommand(String cmd) {
  int delimiterIndex = cmd.indexOf('|');
  if (delimiterIndex == -1) {
    // some commands may be single words
    cmd.trim();
    if (cmd == "OFF") { allOff(); Serial.println("OK|All LEDs off"); return; }
    if (cmd == "STATUS") {
      String status = "OK|";
      status += "R:" + String(config.currentColor.r) + "|";
      status += "G:" + String(config.currentColor.g) + "|";
      status += "B:" + String(config.currentColor.b) + "|";
      status += "BRIGHTNESS:" + String(config.brightness) + "|";
      status += "LEDS:" + String(NUM_LEDS);
      Serial.println(status);
      return;
    }
    if (cmd == "HELP") {
      Serial.println("COMMANDS:");
      Serial.println("  COLOR|R|G|B - Set color (0-255 each)");
      Serial.println("  BRIGHTNESS|0-255 - Set brightness");
      Serial.println("  RAINBOW|speed - Rainbow effect");
      Serial.println("  PULSE|R|G|B|speed - Pulse effect");
      Serial.println("  THEATER|R|G|B|speed - Theater effect");
      Serial.println("  OFF - Turn all off");
      Serial.println("  STATUS - Get current status");
      return;
    }
    Serial.println("ERROR|Invalid format");
    return;
  }

  String command = cmd.substring(0, delimiterIndex);
  String params = cmd.substring(delimiterIndex + 1);

  if (command == "COLOR") {
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    if (p1 == -1 || p2 == -1) { Serial.println("ERROR|FORMAT COLOR|R|G|B"); return; }
    int r = params.substring(0, p1).toInt();
    int g = params.substring(p1 + 1, p2).toInt();
    int b = params.substring(p2 + 1).toInt();
    setColor(r, g, b);
    Serial.println("OK|Color changed");
    demoRunning = false;
  }
  else if (command == "BRIGHTNESS") {
    uint8_t brightness = params.toInt();
    setBrightness(brightness);
    Serial.println("OK|Brightness updated");
    demoRunning = false;
  }
  else if (command == "RAINBOW") {
    uint8_t speed = params.toInt();
    rainbow(speed);
    Serial.println("OK|Rainbow effect");
    demoRunning = false;
  }
  else if (command == "PULSE") {
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    if (p1 == -1 || p2 == -1 || p3 == -1) { Serial.println("ERROR|FORMAT PULSE|R|G|B|speed"); return; }
    int r = params.substring(0, p1).toInt();
    int g = params.substring(p1 + 1, p2).toInt();
    int b = params.substring(p2 + 1, p3).toInt();
    uint8_t speed = params.substring(p3 + 1).toInt();
    pulse(r, g, b, speed);
    Serial.println("OK|Pulse effect");
    demoRunning = false;
  }
  else if (command == "THEATER") {
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    if (p1 == -1 || p2 == -1 || p3 == -1) { Serial.println("ERROR|FORMAT THEATER|R|G|B|speed"); return; }
    int r = params.substring(0, p1).toInt();
    int g = params.substring(p1 + 1, p2).toInt();
    int b = params.substring(p2 + 1, p3).toInt();
    uint8_t speed = params.substring(p3 + 1).toInt();
    theater(r, g, b, speed);
    Serial.println("OK|Theater effect");
    demoRunning = false;
  }
}

// ==================== SETUP ====================

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(500);

  Serial.println("\n\n==================== CAPELLÁN LED (Unified) ====================");
  #if defined(ESP32)
    Serial.println("Platform: ESP32 family (ESP32/ESP32-C3/ESP32-S3)");
  #elif defined(ESP8266)
    Serial.println("Platform: ESP8266");
  #else
    Serial.println("Platform: Unknown - assuming ESP32/ESP8266 compatible");
  #endif
  Serial.println("Using data pin GPIO2 for LED strip");
  Serial.println("====================================================\n");

  setupLED();
  setColor(255, 0, 0); // Rojo inicial

  // Iniciar demo automático al subir el código / al encender
  demoRunning = true;
  demoStartTime = millis();
  Serial.println("Auto-demo started: displaying colors and effects. Send any command to stop demo.");
}

// ==================== LOOP ====================

void loop() {
  // If serial input arrives, stop the demo and process the command
  if (Serial.available() > 0) {
    demoRunning = false;
    String command = Serial.readStringUntil('\n');
    command.trim();
    if (command.length() > 0) {
      Serial.print("CMD: ");
      Serial.println(command);
      processCommand(command);
    }
    return;
  }

  // Run demo while active
  if (demoRunning) {
    runDemoNonBlocking();
  }

  delay(10);
}

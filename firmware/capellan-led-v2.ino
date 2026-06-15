#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

// ==================== CONFIGURACIÓN MULTI-ESP ====================

// Detectar plataforma automáticamente
#if defined(ESP32)
  #include <SPIFFS.h>
  #include <Preferences.h>
  
  #if defined(CONFIG_IDF_TARGET_ESP32C3)
    #define DEVICE_TYPE "ESP32-C3"
    #define LED_PIN 8        // GPIO8 para ESP32-C3
  #elif defined(CONFIG_IDF_TARGET_ESP32S3)
    #define DEVICE_TYPE "ESP32-S3"
    #define LED_PIN 16       // GPIO16 para ESP32-S3
  #else
    #define DEVICE_TYPE "ESP32-WROOM-32"
    #define LED_PIN 13       // GPIO13 para ESP32-WROOM-32
  #endif
  
  #define FS SPIFFS
  Preferences preferences;
  
#elif defined(ESP8266)
  #include <FS.h>
  #include <LittleFS.h>
  
  #define DEVICE_TYPE "ESP8266-D1"
  #define LED_PIN D4         // GPIO2 para ESP8266
  #define FS LittleFS
  
#endif

#define NUM_LEDS 60
#define SERIAL_BAUD 115200
#define MAX_TIMELINE_EVENTS 500
#define TIMELINE_FILE "/timeline.json"
#define CONFIG_FILE "/config.json"

// ==================== VARIABLES GLOBALES ====================

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

struct Color {
  uint8_t r, g, b;
};

struct Config {
  uint8_t brightness = 255;
  bool enabled = true;
  Color currentColor = {255, 0, 0};
  uint8_t numLeds = NUM_LEDS;
  uint32_t baudRate = 115200;
};

struct TimelineEvent {
  uint32_t time;
  String type;     // "color", "brightness", "effect", "transition", "delay"
  String effectName;
  uint8_t r, g, b;
  uint8_t speed;
  uint8_t level;
  uint32_t duration;
};

Config config;
TimelineEvent timelineEvents[MAX_TIMELINE_EVENTS];
uint16_t timelineEventCount = 0;
bool timelineLoaded = false;
bool timelineRunning = false;
uint32_t timelineStartTime = 0;
uint16_t currentEventIndex = 0;

// Buffer para serial
char serialBuffer[512];
int bufferIndex = 0;

// ==================== FUNCIONES DE INICIALIZACIÓN ====================

void initFS() {
  if (!FS.begin()) {
    Serial.println("ERROR|FS mount failed");
    return;
  }
  Serial.println("OK|File system initialized");
}

void loadConfig() {
  if (!FS.exists(CONFIG_FILE)) {
    Serial.println("INFO|No config file, using defaults");
    return;
  }
  
  File configFile = FS.open(CONFIG_FILE, "r");
  if (!configFile) {
    Serial.println("ERROR|Cannot open config file");
    return;
  }
  
  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();
  
  if (error) {
    Serial.println("ERROR|Config parse error");
    return;
  }
  
  config.brightness = doc["brightness"] | 255;
  config.numLeds = doc.containsKey("leds") ? doc["leds"] : NUM_LEDS;
  config.baudRate = doc.containsKey("baud") ? doc["baud"] : 115200;
  
  Serial.println("OK|Config loaded");
}

void saveConfig() {
  StaticJsonDocument<256> doc;
  doc["brightness"] = config.brightness;
  doc["leds"] = config.numLeds;
  doc["baud"] = config.baudRate;
  
  File configFile = FS.open(CONFIG_FILE, "w");
  if (!configFile) {
    Serial.println("ERROR|Cannot create config file");
    return;
  }
  
  serializeJson(doc, configFile);
  configFile.close();
  Serial.println("OK|Config saved");
}

void setupLED() {
  strip.begin();
  strip.show();
  strip.setBrightness(config.brightness);
  Serial.println("OK|LED initialized");
}

// ==================== FUNCIONES DE LED - EFECTOS CLÁSICOS ====================

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

// ==================== FUNCIONES DE LED - NUEVOS EFECTOS ====================

// Efecto Strobe - Parpadeo rápido
void strobe(uint8_t r, uint8_t g, uint8_t b, uint8_t speed, uint16_t duration) {
  uint32_t startTime = millis();
  while (millis() - startTime < duration) {
    setColor(r, g, b);
    delay(speed);
    allOff();
    delay(speed);
  }
  setColor(r, g, b);
}

// Efecto Chase - LEDs corren alrededor
void chase(uint8_t r, uint8_t g, uint8_t b, uint8_t speed, uint16_t duration) {
  uint32_t startTime = millis();
  while (millis() - startTime < duration) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.clear();
      strip.setPixelColor(i, strip.Color(r, g, b));
      strip.setPixelColor((i + 1) % strip.numPixels(), strip.Color(r / 2, g / 2, b / 2));
      strip.show();
      delay(speed);
    }
  }
}

// Efecto Fire - Simulación de fuego
void fire(uint8_t speed, uint16_t duration) {
  uint32_t startTime = millis();
  while (millis() - startTime < duration) {
    for (int i = 0; i < strip.numPixels(); i++) {
      uint8_t flicker = random(50, 255);
      strip.setPixelColor(i, strip.Color(255, flicker / 2, 0));
    }
    strip.show();
    delay(speed);
  }
}

// Efecto Rainbow Cycle - Arcoíris que cambia para todos
void rainbowCycle(uint8_t speed, uint16_t duration) {
  uint32_t startTime = millis();
  uint16_t cycleTime = speed * strip.numPixels();
  
  while (millis() - startTime < duration) {
    for (int j = 0; j < 256; j++) {
      for (int i = 0; i < strip.numPixels(); i++) {
        int hue = (i * 65536 / strip.numPixels() + j * 256) % 65536;
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue)));
      }
      strip.show();
      delay(speed);
    }
  }
}

// Efecto Fade - Transición suave entre colores
void fade(uint8_t fromR, uint8_t fromG, uint8_t fromB,
          uint8_t toR, uint8_t toG, uint8_t toB, uint16_t duration) {
  uint32_t startTime = millis();
  
  while (millis() - startTime < duration) {
    uint32_t elapsed = millis() - startTime;
    float progress = (float)elapsed / duration;
    
    uint8_t r = fromR + (toR - fromR) * progress;
    uint8_t g = fromG + (toG - fromG) * progress;
    uint8_t b = fromB + (toB - fromB) * progress;
    
    setColor(r, g, b);
    delay(10);
  }
  
  setColor(toR, toG, toB);
}

// Efecto Breathe - Respiración suave
void breathe(uint8_t r, uint8_t g, uint8_t b, uint8_t speed, uint16_t duration) {
  uint32_t startTime = millis();
  
  while (millis() - startTime < duration) {
    for (int brightness = 0; brightness <= 255; brightness += speed) {
      setColor(
        (r * brightness) / 255,
        (g * brightness) / 255,
        (b * brightness) / 255
      );
      delay(20);
    }
    for (int brightness = 255; brightness >= 0; brightness -= speed) {
      setColor(
        (r * brightness) / 255,
        (g * brightness) / 255,
        (b * brightness) / 255
      );
      delay(20);
    }
  }
}

// Efecto Sparkle - Destellos aleatorios
void sparkle(uint8_t r, uint8_t g, uint8_t b, uint8_t speed, uint16_t duration) {
  uint32_t startTime = millis();
  strip.clear();
  
  while (millis() - startTime < duration) {
    int pixel = random(strip.numPixels());
    strip.setPixelColor(pixel, strip.Color(r, g, b));
    strip.show();
    delay(speed);
    
    strip.setPixelColor(pixel, 0);
    strip.show();
    delay(speed / 2);
  }
}

// ==================== GESTIÓN DE TIMELINE ====================

void parseTimelineJson(const String& jsonStr) {
  StaticJsonDocument<8192> doc;
  DeserializationError error = deserializeJson(doc, jsonStr);
  
  if (error) {
    Serial.println("ERROR|Timeline parse error");
    return;
  }
  
  timelineEventCount = 0;
  JsonArray events = doc["events"].as<JsonArray>();
  
  for (JsonObject event : events) {
    if (timelineEventCount >= MAX_TIMELINE_EVENTS) {
      Serial.println("WARNING|Max timeline events exceeded");
      break;
    }
    
    timelineEvents[timelineEventCount].time = event["time"] | 0;
    timelineEvents[timelineEventCount].type = event["type"].as<String>();
    timelineEvents[timelineEventCount].effectName = event["effect"] | "";
    timelineEvents[timelineEventCount].r = event["r"] | 0;
    timelineEvents[timelineEventCount].g = event["g"] | 0;
    timelineEvents[timelineEventCount].b = event["b"] | 0;
    timelineEvents[timelineEventCount].speed = event["speed"] | 10;
    timelineEvents[timelineEventCount].level = event["level"] | 255;
    timelineEvents[timelineEventCount].duration = event["duration"] | 1000;
    
    timelineEventCount++;
  }
  
  timelineLoaded = true;
  Serial.print("OK|Timeline loaded with ");
  Serial.print(timelineEventCount);
  Serial.println(" events");
}

void loadTimelineFromFile() {
  if (!FS.exists(TIMELINE_FILE)) {
    Serial.println("ERROR|Timeline file not found");
    return;
  }
  
  File timelineFile = FS.open(TIMELINE_FILE, "r");
  if (!timelineFile) {
    Serial.println("ERROR|Cannot open timeline file");
    return;
  }
  
  String jsonStr = timelineFile.readString();
  timelineFile.close();
  
  parseTimelineJson(jsonStr);
}

void saveTimelineToFile(const String& jsonStr) {
  File timelineFile = FS.open(TIMELINE_FILE, "w");
  if (!timelineFile) {
    Serial.println("ERROR|Cannot create timeline file");
    return;
  }
  
  timelineFile.print(jsonStr);
  timelineFile.close();
  
  parseTimelineJson(jsonStr);
}

void playTimeline() {
  if (!timelineLoaded || timelineEventCount == 0) {
    Serial.println("ERROR|No timeline loaded");
    return;
  }
  
  timelineRunning = true;
  timelineStartTime = millis();
  currentEventIndex = 0;
  
  Serial.println("OK|Timeline playback started");
  
  while (timelineRunning && currentEventIndex < timelineEventCount) {
    uint32_t elapsed = millis() - timelineStartTime;
    TimelineEvent& event = timelineEvents[currentEventIndex];
    
    if (elapsed >= event.time) {
      executeTimelineEvent(event);
      currentEventIndex++;
    }
    
    delay(10);
  }
  
  timelineRunning = false;
  Serial.println("OK|Timeline playback completed");
}

void executeTimelineEvent(TimelineEvent& event) {
  if (event.type == "color") {
    setColor(event.r, event.g, event.b);
  }
  else if (event.type == "brightness") {
    setBrightness(event.level);
  }
  else if (event.type == "effect") {
    if (event.effectName == "strobe") {
      strobe(event.r, event.g, event.b, event.speed, event.duration);
    }
    else if (event.effectName == "chase") {
      chase(event.r, event.g, event.b, event.speed, event.duration);
    }
    else if (event.effectName == "fire") {
      fire(event.speed, event.duration);
    }
    else if (event.effectName == "rainbow_cycle") {
      rainbowCycle(event.speed, event.duration);
    }
    else if (event.effectName == "breathe") {
      breathe(event.r, event.g, event.b, event.speed, event.duration);
    }
    else if (event.effectName == "sparkle") {
      sparkle(event.r, event.g, event.b, event.speed, event.duration);
    }
  }
  else if (event.type == "transition") {
    fade(event.r, event.g, event.b, event.r, event.g, event.b, event.duration);
  }
}

void deleteTimelineFile() {
  if (FS.remove(TIMELINE_FILE)) {
    timelineLoaded = false;
    timelineEventCount = 0;
    Serial.println("OK|Timeline deleted");
  } else {
    Serial.println("ERROR|Cannot delete timeline");
  }
}

void listFiles() {
  Serial.println("OK|Files:");
  
  #if defined(ESP32)
    File root = FS.open("/");
    File file = root.openNextFile();
    while (file) {
      Serial.print("  ");
      Serial.print(file.name());
      Serial.print(" (");
      Serial.print(file.size());
      Serial.println(" bytes)");
      file = root.openNextFile();
    }
  #elif defined(ESP8266)
    Dir dir = FS.openDir("/");
    while (dir.next()) {
      Serial.print("  ");
      Serial.print(dir.fileName());
      Serial.print(" (");
      Serial.print(dir.fileSize());
      Serial.println(" bytes)");
    }
  #endif
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
  
  // ========== COMANDOS CLÁSICOS ==========
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
  
  // ========== NUEVOS EFECTOS ==========
  else if (command == "STROBE") {
    int r = 0, g = 0, b = 0, speed = 100, duration = 2000;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    int p4 = params.indexOf('|', p3 + 1);
    
    r = params.substring(0, p1).toInt();
    g = params.substring(p1 + 1, p2).toInt();
    b = params.substring(p2 + 1, p3).toInt();
    speed = params.substring(p3 + 1, p4).toInt();
    duration = params.substring(p4 + 1).toInt();
    
    strobe(r, g, b, speed, duration);
    Serial.println("OK|Strobe effect");
  }
  else if (command == "CHASE") {
    int r = 0, g = 0, b = 0, speed = 50, duration = 5000;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    int p4 = params.indexOf('|', p3 + 1);
    
    r = params.substring(0, p1).toInt();
    g = params.substring(p1 + 1, p2).toInt();
    b = params.substring(p2 + 1, p3).toInt();
    speed = params.substring(p3 + 1, p4).toInt();
    duration = params.substring(p4 + 1).toInt();
    
    chase(r, g, b, speed, duration);
    Serial.println("OK|Chase effect");
  }
  else if (command == "FIRE") {
    int speed = params.indexOf('|') == -1 ? params.toInt() : 50;
    int p1 = params.indexOf('|');
    int duration = p1 == -1 ? 5000 : params.substring(p1 + 1).toInt();
    
    fire(speed, duration);
    Serial.println("OK|Fire effect");
  }
  else if (command == "RAINBOW_CYCLE") {
    int speed = 50, duration = 10000;
    int p1 = params.indexOf('|');
    
    if (p1 != -1) {
      speed = params.substring(0, p1).toInt();
      duration = params.substring(p1 + 1).toInt();
    } else {
      speed = params.toInt();
    }
    
    rainbowCycle(speed, duration);
    Serial.println("OK|Rainbow cycle effect");
  }
  else if (command == "BREATHE") {
    int r = 0, g = 0, b = 0, speed = 5, duration = 5000;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    int p4 = params.indexOf('|', p3 + 1);
    
    r = params.substring(0, p1).toInt();
    g = params.substring(p1 + 1, p2).toInt();
    b = params.substring(p2 + 1, p3).toInt();
    speed = params.substring(p3 + 1, p4).toInt();
    duration = params.substring(p4 + 1).toInt();
    
    breathe(r, g, b, speed, duration);
    Serial.println("OK|Breathe effect");
  }
  else if (command == "SPARKLE") {
    int r = 255, g = 255, b = 255, speed = 100, duration = 5000;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    int p4 = params.indexOf('|', p3 + 1);
    
    r = params.substring(0, p1).toInt();
    g = params.substring(p1 + 1, p2).toInt();
    b = params.substring(p2 + 1, p3).toInt();
    speed = params.substring(p3 + 1, p4).toInt();
    duration = params.substring(p4 + 1).toInt();
    
    sparkle(r, g, b, speed, duration);
    Serial.println("OK|Sparkle effect");
  }
  
  // ========== COMANDOS DE TIMELINE ==========
  else if (command == "TIMELINE") {
    String subcommand = params.substring(0, params.indexOf('|') == -1 ? params.length() : params.indexOf('|'));
    String subparams = params.indexOf('|') == -1 ? "" : params.substring(params.indexOf('|') + 1);
    
    if (subcommand == "LOAD") {
      loadTimelineFromFile();
    }
    else if (subcommand == "PLAY") {
      playTimeline();
    }
    else if (subcommand == "DELETE") {
      deleteTimelineFile();
    }
    else if (subcommand == "LIST") {
      listFiles();
    }
    else if (subcommand == "STATUS") {
      Serial.print("OK|Loaded:");
      Serial.print(timelineLoaded ? "yes" : "no");
      Serial.print("|Events:");
      Serial.println(timelineEventCount);
    }
  }
  else if (command == "UPLOAD_TIMELINE") {
    saveTimelineToFile(params);
  }
  
  // ========== COMANDOS DE SISTEMA ==========
  else if (command == "OFF") {
    allOff();
    Serial.println("OK|All LEDs off");
  }
  else if (command == "STATUS") {
    String status = "OK|";
    status += "DEVICE:" + String(DEVICE_TYPE) + "|";
    status += "R:" + String(config.currentColor.r) + "|";
    status += "G:" + String(config.currentColor.g) + "|";
    status += "B:" + String(config.currentColor.b) + "|";
    status += "BRIGHTNESS:" + String(config.brightness) + "|";
    status += "LEDS:" + String(config.numLeds) + "|";
    status += "TIMELINE:" + String(timelineLoaded ? "loaded" : "empty");
    Serial.println(status);
  }
  else if (command == "CONFIG") {
    String subcommand = params.substring(0, params.indexOf('|') == -1 ? params.length() : params.indexOf('|'));
    
    if (subcommand == "SAVE") {
      saveConfig();
    }
    else if (subcommand == "LOAD") {
      loadConfig();
    }
  }
  else if (command == "HELP") {
    Serial.println("OK|COMMANDS:");
    Serial.println("  === CLASSIC ===");
    Serial.println("  COLOR|R|G|B");
    Serial.println("  BRIGHTNESS|0-255");
    Serial.println("  RAINBOW|speed");
    Serial.println("  PULSE|R|G|B|speed");
    Serial.println("  THEATER|R|G|B|speed");
    Serial.println("  === NEW EFFECTS ===");
    Serial.println("  STROBE|R|G|B|speed|duration");
    Serial.println("  CHASE|R|G|B|speed|duration");
    Serial.println("  FIRE|speed|duration");
    Serial.println("  RAINBOW_CYCLE|speed|duration");
    Serial.println("  BREATHE|R|G|B|speed|duration");
    Serial.println("  SPARKLE|R|G|B|speed|duration");
    Serial.println("  === TIMELINE ===");
    Serial.println("  TIMELINE|LOAD");
    Serial.println("  TIMELINE|PLAY");
    Serial.println("  TIMELINE|DELETE");
    Serial.println("  TIMELINE|STATUS");
    Serial.println("  UPLOAD_TIMELINE|{json}");
    Serial.println("  === SYSTEM ===");
    Serial.println("  STATUS");
    Serial.println("  CONFIG|SAVE|LOAD");
    Serial.println("  OFF");
  }
  else {
    Serial.println("ERROR|Unknown command: " + command);
  }
}

// ==================== SETUP ====================

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  
  Serial.println("\n\n==================== CAPELLÁN LED v2 ====================");
  Serial.print("Device: ");
  Serial.println(DEVICE_TYPE);
  Serial.print("LED Pin: ");
  Serial.println(LED_PIN);
  Serial.println("====================================================\n");
  
  initFS();
  loadConfig();
  setupLED();
  setColor(255, 0, 0);
  
  Serial.println("Ready! Type 'HELP' for commands");
}

// ==================== LOOP ====================

void loop() {
  if (Serial.available() > 0) {
    char c = Serial.read();
    
    if (c == '\n' || c == '\r') {
      if (bufferIndex > 0) {
        serialBuffer[bufferIndex] = '\0';
        String cmd(serialBuffer);
        cmd.trim();
        if (cmd.length() > 0) {
          Serial.print("CMD: ");
          Serial.println(cmd);
          processCommand(cmd);
        }
        bufferIndex = 0;
      }
    } else if (bufferIndex < sizeof(serialBuffer) - 1) {
      serialBuffer[bufferIndex++] = c;
    }
  }
  
  delay(10);
}

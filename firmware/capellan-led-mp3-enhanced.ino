#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

// ==================== CONFIGURACIÓN MULTI-ESP + MP3 ====================

#if defined(ESP32)
  #include <SPIFFS.h>
  #include <Preferences.h>
  #include <I2S.h>
  
  #if defined(CONFIG_IDF_TARGET_ESP32C3)
    #define DEVICE_TYPE "ESP32-C3"
    #define LED_PIN 8
  #elif defined(CONFIG_IDF_TARGET_ESP32S3)
    #define DEVICE_TYPE "ESP32-S3"
    #define LED_PIN 16
  #else
    #define DEVICE_TYPE "ESP32-WROOM-32"
    #define LED_PIN 13
  #endif
  
  #define FS SPIFFS
  Preferences preferences;
  
#elif defined(ESP8266)
  #include <FS.h>
  #include <LittleFS.h>
  
  #define DEVICE_TYPE "ESP8266-D1"
  #define LED_PIN D4
  #define FS LittleFS
#endif

#define NUM_LEDS 60
#define SERIAL_BAUD 115200
#define MAX_TIMELINE_EVENTS 1000
#define TIMELINE_FILE "/timeline.json"
#define MP3_FILE "/audio.mp3"
#define CONFIG_FILE "/config.json"
#define CHUNK_SIZE 2048
#define BAUD_RATE_HIGH 921600

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
  bool audioSyncEnabled = false;
};

struct TimelineEvent {
  uint32_t time;
  String type;
  String effectName;
  uint8_t r, g, b;
  uint8_t speed;
  uint8_t level;
  uint32_t duration;
};

struct MP3Metadata {
  uint32_t duration;
  uint32_t bitrate;
  uint8_t channels;
  uint32_t sampleRate;
  String title;
};

Config config;
TimelineEvent timelineEvents[MAX_TIMELINE_EVENTS];
uint16_t timelineEventCount = 0;
bool timelineLoaded = false;
bool timelineRunning = false;
uint32_t timelineStartTime = 0;
uint16_t currentEventIndex = 0;
MP3Metadata mp3Info = {0, 0, 2, 44100, ""};
bool mp3Loaded = false;

char serialBuffer[1024];
int bufferIndex = 0;
uint32_t lastDownloadTime = 0;
bool downloadInProgress = false;

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
  
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, configFile);
  configFile.close();
  
  if (error) {
    Serial.println("ERROR|Config parse error");
    return;
  }
  
  config.brightness = doc["brightness"] | 255;
  config.numLeds = doc.containsKey("leds") ? doc["leds"] : NUM_LEDS;
  config.baudRate = doc.containsKey("baud") ? doc["baud"] : 115200;
  config.audioSyncEnabled = doc.containsKey("audioSync") ? doc["audioSync"] : false;
  
  Serial.println("OK|Config loaded");
}

void saveConfig() {
  StaticJsonDocument<512> doc;
  doc["brightness"] = config.brightness;
  doc["leds"] = config.numLeds;
  doc["baud"] = config.baudRate;
  doc["audioSync"] = config.audioSyncEnabled;
  
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

// ==================== FUNCIONES MP3 ====================

void parseMP3Metadata(const String& filename) {
  if (!FS.exists(filename)) {
    Serial.println("ERROR|MP3 file not found");
    return;
  }
  
  File mp3File = FS.open(filename, "r");
  if (!mp3File) {
    Serial.println("ERROR|Cannot open MP3 file");
    return;
  }
  
  uint32_t fileSize = mp3File.size();
  mp3Info.duration = fileSize / 16000; // Aproximación básica
  mp3File.close();
  
  mp3Loaded = true;
  Serial.print("OK|MP3 loaded - Duration: ");
  Serial.print(mp3Info.duration);
  Serial.println("ms");
}

void uploadMP3Chunk(uint32_t offset, uint32_t chunkSize) {
  if (!FS.exists(MP3_FILE)) {
    Serial.println("ERROR|MP3 file not found");
    return;
  }
  
  File mp3File = FS.open(MP3_FILE, "r");
  if (!mp3File) {
    Serial.println("ERROR|Cannot open MP3");
    return;
  }
  
  mp3File.seek(offset);
  uint8_t buffer[CHUNK_SIZE];
  size_t bytesRead = mp3File.readBytes((char*)buffer, chunkSize);
  mp3File.close();
  
  // Enviar chunk en base64
  Serial.print("CHUNK|");
  Serial.print(offset);
  Serial.print("|");
  Serial.print(bytesRead);
  Serial.print("|");
  for (size_t i = 0; i < bytesRead; i++) {
    Serial.printf("%02X", buffer[i]);
  }
  Serial.println();
  
  lastDownloadTime = millis();
}

void saveMP3FromSerial(const String& data) {
  // Guardar MP3 recibido por serial con compresión
  static File mp3File;
  static bool fileOpen = false;
  
  if (data == "START") {
    mp3File = FS.open(MP3_FILE, "w");
    fileOpen = true;
    Serial.println("OK|MP3 upload started");
    return;
  }
  
  if (data == "END") {
    if (fileOpen) {
      mp3File.close();
      fileOpen = false;
      parseMP3Metadata(MP3_FILE);
      Serial.println("OK|MP3 saved successfully");
    }
    return;
  }
  
  if (fileOpen && data.length() > 0) {
    // Convertir hex a bytes
    for (size_t i = 0; i < data.length(); i += 2) {
      String hexStr = data.substring(i, i + 2);
      uint8_t byte = (uint8_t)strtol(hexStr.c_str(), NULL, 16);
      mp3File.write(byte);
    }
  }
}

// ==================== FUNCIONES DE LED - EFECTOS ====================

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

void strobe(uint8_t r, uint8_t g, uint8_t b, uint8_t speed, uint16_t duration) {
  uint32_t startTime = millis();
  while (millis() - startTime < duration) {
    setColor(r, g, b);
    delay(speed);
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, 0);
    }
    strip.show();
    delay(speed);
  }
  setColor(r, g, b);
}

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

void breathe(uint8_t r, uint8_t g, uint8_t b, uint8_t speed, uint16_t duration) {
  uint32_t startTime = millis();
  while (millis() - startTime < duration) {
    for (int brightness = 0; brightness <= 255; brightness += speed) {
      setColor((r * brightness) / 255, (g * brightness) / 255, (b * brightness) / 255);
      delay(20);
    }
    for (int brightness = 255; brightness >= 0; brightness -= speed) {
      setColor((r * brightness) / 255, (g * brightness) / 255, (b * brightness) / 255);
      delay(20);
    }
  }
}

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

void allOff() {
  strip.clear();
  strip.show();
}

// ==================== GESTIÓN DE TIMELINE ====================

void parseTimelineJson(const String& jsonStr) {
  StaticJsonDocument<16384> doc;
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
    else if (event.effectName == "breathe") {
      breathe(event.r, event.g, event.b, event.speed, event.duration);
    }
    else if (event.effectName == "sparkle") {
      sparkle(event.r, event.g, event.b, event.speed, event.duration);
    }
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

void downloadTimelineToPC() {
  if (!timelineLoaded) {
    Serial.println("ERROR|No timeline to download");
    return;
  }
  
  downloadInProgress = true;
  Serial.println("START_DOWNLOAD");
  
  StaticJsonDocument<16384> doc;
  doc["version"] = "1.0";
  doc["metadata"]["title"] = "Capellan Timeline";
  doc["metadata"]["mp3_duration"] = mp3Info.duration;
  
  JsonArray events = doc.createNestedArray("events");
  for (int i = 0; i < timelineEventCount; i++) {
    JsonObject event = events.createNestedObject();
    event["time"] = timelineEvents[i].time;
    event["type"] = timelineEvents[i].type;
    if (timelineEvents[i].type == "color") {
      event["r"] = timelineEvents[i].r;
      event["g"] = timelineEvents[i].g;
      event["b"] = timelineEvents[i].b;
    }
  }
  
  serializeJson(doc, Serial);
  Serial.println("\nEND_DOWNLOAD");
  downloadInProgress = false;
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
    setBrightness(params.toInt());
    Serial.println("OK|Brightness updated");
  }
  else if (command == "TIMELINE") {
    String subcommand = params.substring(0, params.indexOf('|') == -1 ? params.length() : params.indexOf('|'));
    if (subcommand == "LOAD") loadTimelineFromFile();
    else if (subcommand == "PLAY") playTimeline();
    else if (subcommand == "DELETE") deleteTimelineFile();
    else if (subcommand == "LIST") listFiles();
    else if (subcommand == "DOWNLOAD") downloadTimelineToPC();
  }
  else if (command == "UPLOAD_TIMELINE") {
    saveTimelineToFile(params);
  }
  else if (command == "MP3") {
    String subcommand = params.substring(0, params.indexOf('|') == -1 ? params.length() : params.indexOf('|'));
    if (subcommand == "UPLOAD") {
      String data = params.substring(params.indexOf('|') + 1);
      saveMP3FromSerial(data);
    }
    else if (subcommand == "INFO") {
      Serial.print("OK|MP3 Info - Duration: ");
      Serial.print(mp3Info.duration);
      Serial.println("ms");
    }
  }
  else if (command == "OFF") {
    allOff();
    Serial.println("OK|All LEDs off");
  }
  else if (command == "STATUS") {
    String status = "OK|DEVICE:" + String(DEVICE_TYPE) + "|R:" + String(config.currentColor.r) + 
                    "|G:" + String(config.currentColor.g) + "|B:" + String(config.currentColor.b) + 
                    "|BRIGHTNESS:" + String(config.brightness) + "|LEDS:" + String(config.numLeds) + 
                    "|TIMELINE:" + (timelineLoaded ? "loaded" : "empty") + 
                    "|MP3:" + (mp3Loaded ? "loaded" : "empty");
    Serial.println(status);
  }
  else {
    Serial.println("ERROR|Unknown command: " + command);
  }
}

// ==================== SETUP ====================

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  
  Serial.println("\n\n==================== CAPELLÁN LED v3 (MP3 Enhanced) ====================");
  Serial.print("Device: ");
  Serial.println(DEVICE_TYPE);
  Serial.println("✅ MP3 Audio Support");
  Serial.println("✅ Fast Downloads");
  Serial.println("✅ 1000 Events Max");
  Serial.println("=========================================================\n");
  
  initFS();
  loadConfig();
  setupLED();
  setColor(255, 0, 0);
  
  Serial.println("Ready! Commands: COLOR|R|G|B, TIMELINE|PLAY, MP3|UPLOAD, STATUS");
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

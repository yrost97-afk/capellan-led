#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>

// ==================== CONFIGURACIÓN MULTI-PLATAFORMA ====================
// Soporta: ESP8266, ESP32, ESP32-C3, ESP32-S3

#if defined(ESP8266)
  #define LED_PIN 2               // GPIO2 (D4) para ESP8266
  #define PLATFORM_NAME "ESP8266 D1 Mini"
#elif defined(ESP32)
  #if CONFIG_IDF_TARGET_ESP32
    #define LED_PIN 2             // GPIO2 para ESP32 estándar
    #define PLATFORM_NAME "ESP32"
  #elif CONFIG_IDF_TARGET_ESP32C3
    #define LED_PIN 2             // GPIO2 para ESP32-C3
    #define PLATFORM_NAME "ESP32-C3"
  #elif CONFIG_IDF_TARGET_ESP32S3
    #define LED_PIN 2             // GPIO2 para ESP32-S3 (Groom-32)
    #define PLATFORM_NAME "ESP32-S3 (Groom-32)"
  #else
    #define LED_PIN 2
    #define PLATFORM_NAME "ESP32 (Unknown variant)"
  #endif
#else
  #define LED_PIN 2
  #define PLATFORM_NAME "Unknown Platform"
#endif

#define NUM_LEDS 60             // Cantidad de LEDs WS2812
#define SERIAL_BAUD 115200      // Velocidad Serial
#define MAX_TIMELINE_EVENTS 100 // Máximo eventos en timeline

// ==================== VARIABLES GLOBALES ====================
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

struct Color {
  uint8_t r, g, b;
};

struct TimelineEvent {
  unsigned long timestamp;
  uint8_t r, g, b;
  uint8_t brightness;
  String effect;
  unsigned long duration;
};

struct Config {
  uint8_t brightness = 255;
  bool enabled = true;
  Color currentColor = {255, 0, 0};
};

Config config;
TimelineEvent timeline[MAX_TIMELINE_EVENTS];
int timelineCount = 0;
bool isTimelineRunning = false;
unsigned long timelineStartTime = 0;
int currentEventIndex = 0;
unsigned long demoStartTime = 0;
int demoStep = 0;

// ==================== FUNCIONES AUXILIARES MULTI-PLATAFORMA ====================

void platformInit() {
  #if defined(ESP32)
    // Configuración específica para ESP32/C3/S3
    Serial.setTxBufferSize(1024);
  #endif
}

String getPlatformInfo() {
  String info = PLATFORM_NAME;
  #if defined(ESP32)
    #if CONFIG_IDF_TARGET_ESP32
      info += " (Dual-core, 240MHz)";
    #elif CONFIG_IDF_TARGET_ESP32C3
      info += " (Single-core RISC-V, 160MHz)";
    #elif CONFIG_IDF_TARGET_ESP32S3
      info += " (Dual-core RISC-V, 240MHz)";
    #endif
  #endif
  return info;
}

// ==================== FUNCIONES DE LED ====================

void setupLED() {
  strip.begin();
  strip.show();
  strip.setBrightness(config.brightness);
  Serial.println("✓ LED Inicializado en PIN: " + String(LED_PIN));
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

void fade(uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
  for (int brightness = 0; brightness <= 255; brightness += speed) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(
        (r * brightness) / 255,
        (g * brightness) / 255,
        (b * brightness) / 255
      ));
    }
    strip.show();
    delay(30);
  }
}

void strobe(uint8_t r, uint8_t g, uint8_t b, uint8_t times, uint8_t speed) {
  for (int i = 0; i < times; i++) {
    setColor(r, g, b);
    delay(speed);
    allOff();
    delay(speed);
  }
}

void chase(uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
  for (int pos = 0; pos < strip.numPixels(); pos++) {
    strip.clear();
    strip.setPixelColor(pos, strip.Color(r, g, b));
    strip.show();
    delay(speed);
  }
}

void bounce(uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
  for (int i = 0; i < 2; i++) {
    for (int pos = 0; pos < strip.numPixels(); pos++) {
      strip.clear();
      strip.setPixelColor(pos, strip.Color(r, g, b));
      strip.show();
      delay(speed);
    }
    for (int pos = strip.numPixels() - 1; pos >= 0; pos--) {
      strip.clear();
      strip.setPixelColor(pos, strip.Color(r, g, b));
      strip.show();
      delay(speed);
    }
  }
}

void wave(uint8_t r, uint8_t g, uint8_t b, uint8_t speed) {
  for (int cycle = 0; cycle < 2; cycle++) {
    for (int pos = 0; pos < strip.numPixels(); pos++) {
      strip.clear();
      // Onda gaussiana aproximada
      for (int i = 0; i < strip.numPixels(); i++) {
        int distance = abs(i - pos);
        int brightness = 255 - (distance * 255 / 10);
        if (brightness > 0) {
          strip.setPixelColor(i, strip.Color(
            (r * brightness) / 255,
            (g * brightness) / 255,
            (b * brightness) / 255
          ));
        }
      }
      strip.show();
      delay(speed);
    }
  }
}

void allOff() {
  strip.clear();
  strip.show();
}

// ==================== DEMOSTRACIÓN DE EFECTOS ====================

void runDemoSequence() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - demoStartTime;
  
  // Cada efecto dura 8 segundos
  int effectDuration = 8000;
  int currentEffect = (elapsedTime / effectDuration) % 8;
  
  switch(currentEffect) {
    case 0:
      // Efecto 1: Colores básicos (2 segundos cada uno)
      if ((elapsedTime % effectDuration) < 2000) {
        setColor(255, 0, 0);  // Rojo
      } else if ((elapsedTime % effectDuration) < 4000) {
        setColor(0, 255, 0);  // Verde
      } else if ((elapsedTime % effectDuration) < 6000) {
        setColor(0, 0, 255);  // Azul
      } else {
        setColor(255, 255, 0);  // Amarillo
      }
      break;
      
    case 1:
      // Efecto 2: Pulso rojo
      pulse(255, 0, 0, 15);
      break;
      
    case 2:
      // Efecto 3: Teatro verde
      theater(0, 255, 0, 50);
      break;
      
    case 3:
      // Efecto 4: Arco iris
      rainbow(20);
      break;
      
    case 4:
      // Efecto 5: Chase azul
      chase(0, 100, 255, 30);
      break;
      
    case 5:
      // Efecto 6: Bounce naranja
      bounce(255, 165, 0, 25);
      break;
      
    case 6:
      // Efecto 7: Wave púrpura
      wave(200, 0, 255, 15);
      break;
      
    case 7:
      // Efecto 8: Estroboscópico cian
      strobe(0, 255, 255, 5, 100);
      break;
  }
}

// ==================== PROTOCOLO SERIAL UNIFICADO ====================

void processCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  int delimiterIndex = cmd.indexOf('|');
  
  if (delimiterIndex == -1) {
    Serial.println("ERROR|Formato inválido. Usa: COMANDO|param1|param2...");
    return;
  }

  String command = cmd.substring(0, delimiterIndex);
  String params = cmd.substring(delimiterIndex + 1);
  command.toUpperCase();

  // ===== COMANDOS BÁSICOS =====
  if (command == "COLOR") {
    int r = 0, g = 0, b = 0;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    
    if (p1 != -1 && p2 != -1) {
      r = params.substring(0, p1).toInt();
      g = params.substring(p1 + 1, p2).toInt();
      b = params.substring(p2 + 1).toInt();
      
      setColor(r, g, b);
      isTimelineRunning = false;  // Detener demo si estaba corriendo
      Serial.println("OK|Color cambiado a RGB(" + String(r) + "," + String(g) + "," + String(b) + ")");
    } else {
      Serial.println("ERROR|Formato: COLOR|R|G|B");
    }
  }
  else if (command == "BRIGHTNESS") {
    uint8_t brightness = params.toInt();
    setBrightness(brightness);
    Serial.println("OK|Brillo establecido a " + String(brightness));
  }
  else if (command == "OFF") {
    allOff();
    isTimelineRunning = false;
    Serial.println("OK|Todos los LEDs apagados");
  }
  
  // ===== EFECTOS =====
  else if (command == "RAINBOW") {
    isTimelineRunning = false;
    uint8_t speed = params.length() > 0 ? params.toInt() : 20;
    rainbow(speed);
    Serial.println("OK|Efecto Arco Iris ejecutado");
  }
  else if (command == "PULSE") {
    isTimelineRunning = false;
    int r = 0, g = 0, b = 0;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    
    if (p1 != -1 && p2 != -1 && p3 != -1) {
      r = params.substring(0, p1).toInt();
      g = params.substring(p1 + 1, p2).toInt();
      b = params.substring(p2 + 1, p3).toInt();
      uint8_t speed = params.substring(p3 + 1).toInt();
      
      pulse(r, g, b, speed);
      Serial.println("OK|Efecto Pulso ejecutado");
    } else {
      Serial.println("ERROR|Formato: PULSE|R|G|B|speed");
    }
  }
  else if (command == "THEATER") {
    isTimelineRunning = false;
    int r = 0, g = 0, b = 0;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    
    if (p1 != -1 && p2 != -1 && p3 != -1) {
      r = params.substring(0, p1).toInt();
      g = params.substring(p1 + 1, p2).toInt();
      b = params.substring(p2 + 1, p3).toInt();
      uint8_t speed = params.substring(p3 + 1).toInt();
      
      theater(r, g, b, speed);
      Serial.println("OK|Efecto Teatro ejecutado");
    } else {
      Serial.println("ERROR|Formato: THEATER|R|G|B|speed");
    }
  }
  else if (command == "FADE") {
    isTimelineRunning = false;
    int r = 0, g = 0, b = 0;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    
    if (p1 != -1 && p2 != -1 && p3 != -1) {
      r = params.substring(0, p1).toInt();
      g = params.substring(p1 + 1, p2).toInt();
      b = params.substring(p2 + 1, p3).toInt();
      uint8_t speed = params.substring(p3 + 1).toInt();
      
      fade(r, g, b, speed);
      Serial.println("OK|Efecto Transición ejecutado");
    } else {
      Serial.println("ERROR|Formato: FADE|R|G|B|speed");
    }
  }
  else if (command == "STROBE") {
    isTimelineRunning = false;
    int r = 0, g = 0, b = 0;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    int p4 = params.indexOf('|', p3 + 1);
    
    if (p1 != -1 && p2 != -1 && p3 != -1 && p4 != -1) {
      r = params.substring(0, p1).toInt();
      g = params.substring(p1 + 1, p2).toInt();
      b = params.substring(p2 + 1, p3).toInt();
      uint8_t times = params.substring(p3 + 1, p4).toInt();
      uint8_t speed = params.substring(p4 + 1).toInt();
      
      strobe(r, g, b, times, speed);
      Serial.println("OK|Efecto Estroboscópico ejecutado");
    } else {
      Serial.println("ERROR|Formato: STROBE|R|G|B|times|speed");
    }
  }
  else if (command == "CHASE") {
    isTimelineRunning = false;
    int r = 0, g = 0, b = 0;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    
    if (p1 != -1 && p2 != -1 && p3 != -1) {
      r = params.substring(0, p1).toInt();
      g = params.substring(p1 + 1, p2).toInt();
      b = params.substring(p2 + 1, p3).toInt();
      uint8_t speed = params.substring(p3 + 1).toInt();
      
      chase(r, g, b, speed);
      Serial.println("OK|Efecto Chase ejecutado");
    } else {
      Serial.println("ERROR|Formato: CHASE|R|G|B|speed");
    }
  }
  else if (command == "BOUNCE") {
    isTimelineRunning = false;
    int r = 0, g = 0, b = 0;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    
    if (p1 != -1 && p2 != -1 && p3 != -1) {
      r = params.substring(0, p1).toInt();
      g = params.substring(p1 + 1, p2).toInt();
      b = params.substring(p2 + 1, p3).toInt();
      uint8_t speed = params.substring(p3 + 1).toInt();
      
      bounce(r, g, b, speed);
      Serial.println("OK|Efecto Rebote ejecutado");
    } else {
      Serial.println("ERROR|Formato: BOUNCE|R|G|B|speed");
    }
  }
  else if (command == "WAVE") {
    isTimelineRunning = false;
    int r = 0, g = 0, b = 0;
    int p1 = params.indexOf('|');
    int p2 = params.indexOf('|', p1 + 1);
    int p3 = params.indexOf('|', p2 + 1);
    
    if (p1 != -1 && p2 != -1 && p3 != -1) {
      r = params.substring(0, p1).toInt();
      g = params.substring(p1 + 1, p2).toInt();
      b = params.substring(p2 + 1, p3).toInt();
      uint8_t speed = params.substring(p3 + 1).toInt();
      
      wave(r, g, b, speed);
      Serial.println("OK|Efecto Onda ejecutado");
    } else {
      Serial.println("ERROR|Formato: WAVE|R|G|B|speed");
    }
  }
  
  // ===== DEMO =====
  else if (command == "DEMO") {
    isTimelineRunning = true;
    demoStartTime = millis();
    demoStep = 0;
    Serial.println("OK|Demostración iniciada - Presiona DEMO para detener");
  }
  
  // ===== TIMELINE =====
  else if (command == "TIMELINE") {
    String subcommand = params.substring(0, params.indexOf('|') == -1 ? params.length() : params.indexOf('|'));
    subcommand.toUpperCase();
    
    if (subcommand == "PLAY") {
      isTimelineRunning = true;
      timelineStartTime = millis();
      currentEventIndex = 0;
      Serial.println("OK|Reproducción de Timeline iniciada");
    }
    else if (subcommand == "STOP") {
      isTimelineRunning = false;
      allOff();
      Serial.println("OK|Timeline detenido");
    }
    else if (subcommand == "PAUSE") {
      isTimelineRunning = false;
      Serial.println("OK|Timeline pausado");
    }
    else if (subcommand == "CLEAR") {
      timelineCount = 0;
      isTimelineRunning = false;
      allOff();
      Serial.println("OK|Timeline limpiado");
    }
    else {
      Serial.println("ERROR|Subcomandos: PLAY, STOP, PAUSE, CLEAR");
    }
  }
  
  // ===== INFORMACIÓN =====
  else if (command == "STATUS") {
    String status = "OK|";
    status += "PLATFORM:" + getPlatformInfo() + "|";
    status += "PIN:" + String(LED_PIN) + "|";
    status += "LED_COUNT:" + String(NUM_LEDS) + "|";
    status += "R:" + String(config.currentColor.r) + "|";
    status += "G:" + String(config.currentColor.g) + "|";
    status += "B:" + String(config.currentColor.b) + "|";
    status += "BRIGHTNESS:" + String(config.brightness) + "|";
    status += "TIMELINE_EVENTS:" + String(timelineCount) + "|";
    status += "RUNNING:" + String(isTimelineRunning ? "SI" : "NO");
    Serial.println(status);
  }
  else if (command == "INFO") {
    Serial.println("\n╔═════════════════════════════════════════╗");
    Serial.println("║     CAPELLÁN LED - INFORMACIÓN          ║");
    Serial.println("╠═════════════════════════════════════════╣");
    Serial.println("║ Plataforma: " + getPlatformInfo());
    Serial.println("║ Pin de Datos: GPIO" + String(LED_PIN));
    Serial.println("║ Cantidad de LEDs: " + String(NUM_LEDS));
    Serial.println("║ Velocidad Serial: " + String(SERIAL_BAUD));
    Serial.println("║ Versión: 2.0 Multi-Plataforma + DEMO");
    Serial.println("╚═════════════════════════════════════════╝\n");
  }
  else if (command == "HELP") {
    Serial.println("\n╔════════════════════════════════════════════════════════╗");
    Serial.println("║   CAPELLÁN LED v2.0 - COMANDOS DISPONIBLES            ║");
    Serial.println("╠════════════════════════════════════════════════════════╣");
    
    Serial.println("║ === COMANDOS BÁSICOS ===");
    Serial.println("║ COLOR|R|G|B              - Establecer color (0-255)");
    Serial.println("║ BRIGHTNESS|0-255         - Establecer brillo");
    Serial.println("║ OFF                      - Apagar todos los LEDs");
    
    Serial.println("║");
    Serial.println("║ === EFECTOS ===");
    Serial.println("║ RAINBOW|speed            - Arco iris");
    Serial.println("║ PULSE|R|G|B|speed        - Pulso");
    Serial.println("║ THEATER|R|G|B|speed      - Teatro");
    Serial.println("║ FADE|R|G|B|speed         - Transición");
    Serial.println("║ STROBE|R|G|B|times|speed - Estroboscópico");
    Serial.println("║ CHASE|R|G|B|speed        - Persecución");
    Serial.println("║ BOUNCE|R|G|B|speed       - Rebote");
    Serial.println("║ WAVE|R|G|B|speed         - Onda");
    
    Serial.println("║");
    Serial.println("║ === DEMOSTRACIÓN ===");
    Serial.println("║ DEMO                     - Mostrar todos los efectos");
    
    Serial.println("║");
    Serial.println("║ === TIMELINE ===");
    Serial.println("║ TIMELINE|PLAY            - Reproducir");
    Serial.println("║ TIMELINE|STOP            - Detener");
    Serial.println("║ TIMELINE|PAUSE           - Pausar");
    Serial.println("║ TIMELINE|CLEAR           - Limpiar");
    
    Serial.println("║");
    Serial.println("║ === INFORMACIÓN ===");
    Serial.println("║ STATUS                   - Ver estado actual");
    Serial.println("║ INFO                     - Ver información del sistema");
    Serial.println("║ HELP                     - Mostrar esta ayuda");
    
    Serial.println("╚════════════════════════════════════════════════════════╝\n");
  }
  else {
    Serial.println("ERROR|Comando desconocido: " + command + ". Usa HELP para ver los comandos");
  }
}

// ==================== SETUP ====================

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  
  platformInit();
  
  Serial.println("\n\n");
  Serial.println("╔══════════════════════════════════════════════════════╗");
  Serial.println("║    CAPELLÁN LED - FIRMWARE UNIFICADO v2.0           ║");
  Serial.println("║         " + getPlatformInfo());
  Serial.println("║           Pin de Datos: GPIO" + String(LED_PIN) + "                         ║");
  Serial.println("╚══════════════════════════════════════════════════════╝");
  Serial.println();
  
  setupLED();
  
  // ===== DEMOSTRACIÓN AUTOMÁTICA AL ENCENDER =====
  Serial.println("🎆 ¡Iniciando demostración automática de efectos!");
  Serial.println("💡 Puedes interrumpir escribiendo cualquier comando o esperando 60 segundos");
  Serial.println();
  
  isTimelineRunning = true;
  demoStartTime = millis();
  demoStep = 0;
}

// ==================== LOOP ====================

void loop() {
  // Procesar comandos seriales
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    command.trim();
    
    if (command.length() > 0) {
      Serial.print(">>> ");
      Serial.println(command);
      processCommand(command);
    }
  }
  
  // Ejecutar demostración automática
  if (isTimelineRunning) {
    unsigned long currentTime = millis();
    unsigned long demoElapsedTime = currentTime - demoStartTime;
    
    // La demostración dura 60 segundos (8 efectos x 8 segundos cada uno)
    if (demoElapsedTime > 60000) {
      // Terminar demo después de 60 segundos
      isTimelineRunning = false;
      allOff();
      Serial.println("\n✓ Demostración completada. Usa 'DEMO' para iniciar de nuevo");
      Serial.println("¡Ahora puedes usar los comandos! Escribe 'HELP' para ayuda\n");
    } else {
      // Ejecutar secuencia de demo
      runDemoSequence();
    }
  }
  
  // Ejecutar timeline si está corriendo
  if (isTimelineRunning && timelineCount > 0) {
    unsigned long elapsedTime = millis() - timelineStartTime;
    
    // Lógica de reproducción de timeline
    if (currentEventIndex < timelineCount) {
      TimelineEvent& event = timeline[currentEventIndex];
      
      if (elapsedTime >= event.timestamp) {
        // Ejecutar evento
        setColor(event.r, event.g, event.b);
        setBrightness(event.brightness);
        currentEventIndex++;
      }
    } else {
      // Timeline terminado
      isTimelineRunning = false;
      allOff();
    }
  }
  
  delay(10);
}

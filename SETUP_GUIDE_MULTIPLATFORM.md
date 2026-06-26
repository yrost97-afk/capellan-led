# 🎆 Capellán LED - Guía Completa Multi-Plataforma

## 📋 Plataformas Soportadas

- ✅ **ESP8266 D1 Mini**
- ✅ **ESP32** (Estándar, Dual-core)
- ✅ **ESP32-C3** (RISC-V, Single-core)
- ✅ **ESP32-S3 (Groom-32)** (RISC-V, Dual-core)

### Especificaciones

| Plataforma | CPU | Velocidad | RAM | Pin GPIO2 |
|-----------|-----|----------|-----|-----------|
| ESP8266 | Xtensa 1-core | 80/160 MHz | 160 KB | ✅ D4 |
| ESP32 | Xtensa 2-core | 240 MHz | 520 KB | ✅ |
| ESP32-C3 | RISC-V 1-core | 160 MHz | 400 KB | ✅ |
| ESP32-S3 | RISC-V 2-core | 240 MHz | 512 KB | ✅ |

---

## 🚀 Instalación con PlatformIO

### Requisitos
- Visual Studio Code
- Extensión PlatformIO
- Python 3.6+

### Pasos Generales

1. **Abre el proyecto en VS Code**
   ```bash
   code capellan-led
   ```

2. **Abre la carpeta `firmware`**

3. **Selecciona el entorno según tu placa:**
   
   **Para ESP8266 D1 Mini:**
   ```bash
   pio run -e d1_mini -t upload
   ```
   
   **Para ESP32 Estándar:**
   ```bash
   pio run -e esp32 -t upload
   ```
   
   **Para ESP32-C3:**
   ```bash
   pio run -e esp32c3 -t upload
   ```
   
   **Para ESP32-S3 (Groom-32):**
   ```bash
   pio run -e esp32s3 -t upload
   ```

4. **O usa la interfaz gráfica de PlatformIO:**
   - Click en el ícono de PlatformIO en la barra lateral
   - Selecciona el ambiente que necesitas
   - Click en "Build" (compilar)
   - Click en "Upload" (subir)

---

## 🎨 Instalación con Arduino IDE

### Para ESP8266 D1 Mini

1. **Instala la placa ESP8266**
   - Archivo → Preferencias
   - URLs adicionales:
     ```
     http://arduino.esp8266.com/stable/package_esp8266com_index.json
     ```
   - Herramientas → Placa → Gestor de tarjetas → Busca "ESP8266" → Instala

2. **Configura la placa**
   - Herramientas → Placa → "Generic ESP8266 Module"
   - Flash Size: "4M (3M SPIFFS)"
   - Flash Mode: "DIO"
   - Flash Speed: "80 MHz"
   - CPU Speed: "80 MHz"
   - Upload Speed: "921600"

3. **Instala librerías**
   - Adafruit NeoPixel
   - ArduinoJson

4. **Carga el firmware**
   - Abre `capellan-led-unified.ino`
   - Presiona Subir (→)

### Para ESP32 / ESP32-C3 / ESP32-S3

1. **Instala el soporte ESP32**
   - Archivo → Preferencias
   - URLs adicionales:
     ```
     https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
     ```
   - Herramientas → Placa → Gestor de tarjetas → Busca "ESP32" → Instala

2. **Selecciona tu placa específica**
   
   **ESP32 (Generic):**
   - Herramientas → Placa → "ESP32 Dev Module"
   
   **ESP32-C3:**
   - Herramientas → Placa → "ESP32C3 Dev Module"
   
   **ESP32-S3 (Groom-32):**
   - Herramientas → Placa → "ESP32S3 Dev Module"

3. **Configura opciones comunes**
   - CPU Frequency: "240 MHz" (o "160 MHz" para C3)
   - Flash Size: "4MB"
   - Flash Mode: "DIO"
   - Upload Speed: "921600"

4. **Instala librerías**
   - Adafruit NeoPixel
   - ArduinoJson

5. **Carga el firmware**
   - Abre `capellan-led-unified.ino`
   - Presiona Subir (→)

---

## 🔌 Conexión de Hardware

### Esquema Universal (Todas las plataformas)

```
Microcontrolador        WS2812 LED Strip
┌──────────────────────────────────────┐
│ GND ──────────────→ GND (Negro)      │
│ 5V  ──────────────→ +5V (Rojo)       │
│ GPIO2 ─[470Ω]──→ DATA (Verde/Amarillo)
└──────────────────────────────────────┘

⚠️ IMPORTANTE: Usa resistencia 470Ω entre GPIO2 y DATA
⚠️ Asegúrate que GND esté conectado en ambos lados
```

### Identificación de Pines GPIO2

**ESP8266 D1 Mini:**
```
    ┌─────────────────────┐
    │ D0(GPIO16) D1(GPIO5)│
    │ D2(GPIO4)  D3(GPIO0)│
    │ D4(GPIO2)  D5(GPIO14)← ⭐ PIN GPIO2
    │ D6(GPIO12) D7(GPIO13)
    │ D8(GPIO15)          │
    │ GND  5V  RST         │
    └─────────────────────┘
```

**ESP32 / ESP32-C3 / ESP32-S3:**
```
GPIO2 se encuentra en la fila de pines
Busca la etiqueta "2" o "GPIO2" en tu placa

Groom-32 (ESP32-S3):
Tiene GPIO2 accesible en la placa de desarrollo
```

---

## 📡 Comandos Serial

### Conexión Base
- **Baud Rate**: 115200
- **Line Ending**: "\n" (Newline)
- **Puerto**: Automático o manual según tu SO

### Comandos Disponibles

```bash
# === COLORES BÁSICOS ===
COLOR|255|0|0          # Rojo
COLOR|0|255|0          # Verde
COLOR|0|0|255          # Azul
COLOR|255|255|0        # Amarillo
COLOR|255|0|255        # Magenta
COLOR|0|255|255        # Cian
COLOR|255|255|255      # Blanco

# === BRILLO ===
BRIGHTNESS|255         # Máximo (100%)
BRIGHTNESS|128         # Medio (50%)
BRIGHTNESS|64          # Bajo (25%)
BRIGHTNESS|10          # Mínimo (4%)

# === EFECTOS ===
RAINBOW|20             # Arco iris (20ms por paso)
PULSE|255|0|0|10       # Pulso rojo (velocidad 10)
THEATER|0|255|0|50     # Teatro verde (50ms)
FADE|0|100|255|5       # Transición azul suave
STROBE|255|255|0|5|100 # Estroboscópico 5 veces
CHASE|255|100|0|30     # Persecución naranja
BOUNCE|0|255|100|25    # Rebote verde claro
WAVE|100|0|255|15      # Onda púrpura

# === TIMELINE ===
TIMELINE|PLAY          # Reproducir secuencia
TIMELINE|PAUSE         # Pausar
TIMELINE|STOP          # Detener y apagar
TIMELINE|CLEAR         # Limpiar secuencia

# === INFORMACIÓN ===
STATUS                 # Ver estado actual
INFO                   # Ver información del sistema
HELP                   # Mostrar todos los comandos

# === CONTROL ===
OFF                    # Apagar todos los LEDs
```

---

## 🌐 Integración con Web App

La aplicación web en `web-app/index.html` funciona con todas las plataformas:

1. **Abre el archivo en tu navegador**
   ```
   Arrastra y suelta web-app/index.html a tu navegador
   o: Open with → navegador predeterminado
   ```

2. **Conecta el dispositivo**
   - Selecciona el puerto USB
   - Click en "Conectar"
   - Verás "✅ Conectado"

3. **Características disponibles**
   - Selector de colores con vista previa
   - 12+ efectos predefinidos
   - Timeline visual con drag-and-drop
   - Guardar/cargar configuraciones
   - Control de brillo
   - Información en tiempo real

---

## 🔧 Solución de Problemas

### "No se detecta el puerto USB"
```
Soluciones:
1. Verifica que el cable es de datos (no solo carga)
2. Instala drivers CH340 (ESP8266/ESP32)
3. Reinicia Arduino IDE o PlatformIO
4. En Linux: sudo usermod -a -G dialout $USER
```

### "Error: Espera a que se compile"
```
Soluciones:
1. Limpia el proyecto: Ctrl+Alt+C (PlatformIO)
2. O en Arduino IDE: Sketch → Compilar verificación
3. Verifica que las librerías están instaladas
```

### "Los LEDs no se encienden"
```
Soluciones:
1. Verifica conexión del pin GPIO2
2. Confirma alimentación de 5V a los LEDs
3. Prueba comando: COLOR|255|0|0
4. Verifica resistencia 470Ω está presente
5. Intenta cambiar LED_PIN a otro GPIO
```

### "Serial muestra basura"
```
Soluciones:
1. Verifica Baud Rate = 115200
2. Reinicia el dispositivo
3. Desconecta y reconecta USB
4. Intenta otro puerto USB
```

### "Faltan librerías"
```
Para PlatformIO:
pio lib install "Adafruit NeoPixel"
pio lib install "ArduinoJson"

Para Arduino IDE:
Herramientas → Administrador de librerías
Busca e instala cada una
```

---

## 📊 Comparación de Rendimiento

| Característica | ESP8266 | ESP32 | ESP32-C3 | ESP32-S3 |
|---------------|---------|-------|----------|----------|
| Velocidad | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| RAM | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ |
| Precio | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ |
| WiFi | ✅ 2.4GHz | ✅ 2.4GHz | ✅ 2.4GHz | ✅ 2.4/5GHz |
| Bluetooth | ❌ | ✅ v4.2 | ✅ v5.0 | ✅ v5.0 |
| Versatilidad | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ |

---

## 📚 Recursos Adicionales

- [ESP8266 Arduino Core](https://github.com/esp8266/Arduino)
- [ESP32 Arduino Core](https://github.com/espressif/arduino-esp32)
- [Adafruit NeoPixel Library](https://github.com/adafruit/Adafruit_NeoPixel)
- [ArduinoJson](https://arduinojson.org/)
- [PlatformIO Documentation](https://docs.platformio.org/)

---

## 🎯 Próximos Pasos

1. **Selecciona tu plataforma** (ESP8266/ESP32/C3/S3)
2. **Sigue los pasos de instalación** para tu IDE preferido
3. **Carga el firmware unificado** en tu dispositivo
4. **Abre la web app** y conecta por USB
5. **¡Crea efectos increíbles!** 🎆✨

---

## 💡 Consejos de Optimización

**Para máximo rendimiento:**
- ESP32-S3: Ideal para múltiples timelines y WiFi
- ESP32-C3: Buena relación tamaño/precio
- ESP32: Versátil y confiable
- ESP8266: Perfecto para proyectos simples

**Para bajo consumo:**
- Reduce BRIGHTNESS
- Usa menos LEDs en stripe
- Selecciona efectos más simples

---

¡Disfruta tu **Capellán LED** en tu plataforma preferida! 🎆✨

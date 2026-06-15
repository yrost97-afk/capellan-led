# 📝 CHANGELOG - Capellán LED v2

Todos los cambios notables en este proyecto serán documentados en este archivo.

El formato está basado en [Keep a Changelog](https://keepachangelog.com/es-ES/1.0.0/).

---

## [2.0.0] - 2026-06-15

### 🎉 Agregado

#### Hardware
- ✅ Soporte para **ESP32-WROOM-32** (GPIO13)
- ✅ Soporte para **ESP32-C3** (GPIO8)
- ✅ Soporte para **ESP32-S3** (GPIO16)
- ✅ Detección automática de plataforma (sin configuración manual)
- ✅ Inicialización de GPIO dependiente de la plataforma

#### Efectos LED - Nuevos
- ✅ **STROBE** - Efecto de parpadeo estilo discoteca
  - Parámetros: `R|G|B|SPEED|DURATION`
  - Velocidad configurable (10-1000ms)
  - Duración del efecto en ms

- ✅ **CHASE** - LEDs corren alrededor de la tira
  - Parámetros: `R|G|B|SPEED|DURATION`
  - Efecto visual de persecución
  - Ideal para Poi y Staff

- ✅ **FIRE** - Simulación realista de fuego
  - Parámetros: `SPEED|DURATION`
  - Colores aleatorios rojo-naranja
  - Parpadeo natural

- ✅ **RAINBOW_CYCLE** - Arcoíris continuo
  - Parámetros: `SPEED|DURATION`
  - Todos los LEDs cambian juntos
  - Efecto visual diferente a RAINBOW clásico

- ✅ **BREATHE** - Respiración suave (efecto zen)
  - Parámetros: `R|G|B|SPEED|DURATION`
  - Fade in/out suave
  - Perfecto para sincronización con música

- ✅ **SPARKLE** - Destellos aleatorios mágicos
  - Parámetros: `R|G|B|SPEED|DURATION`
  - LEDs parpadean aleatoriamente
  - Efecto mágico/fiesta

#### Sistema de Timeline
- ✅ Almacenamiento en **SPIFFS** (ESP32) / **LittleFS** (ESP8266)
- ✅ Carga de timeline desde archivo: `TIMELINE|LOAD`
- ✅ Reproducción de timeline: `TIMELINE|PLAY`
- ✅ Eliminación de timeline: `TIMELINE|DELETE`
- ✅ Estado del timeline: `TIMELINE|STATUS`
- ✅ Listado de archivos: `TIMELINE|LIST`
- ✅ Subida de timeline en vivo: `UPLOAD_TIMELINE|{JSON}`
- ✅ Soporte para 500 eventos máximo
- ✅ Tipos de eventos soportados:
  - `color` - Cambio de color instantáneo
  - `brightness` - Ajuste de brillo
  - `effect` - Ejecución de efectos
  - `transition` - Transición suave entre colores
  - `delay` - Pausa (futuro)

#### Gestión de Configuración
- ✅ Guardado de configuración: `CONFIG|SAVE`
- ✅ Carga de configuración: `CONFIG|LOAD`
- ✅ Almacenamiento en EEPROM (ESP8266) / Preferences (ESP32)
- ✅ Parámetros guardados:
  - Brillo actual
  - Número de LEDs
  - Velocidad serial

#### API Serial Mejorada
- ✅ Nuevos comandos de sistema
- ✅ Mejor manejo de errores
- ✅ Respuestas consistentes `OK|...` / `ERROR|...`
- ✅ Comando `HELP` actualizado con todos los comandos

#### Documentación
- ✅ **API_v2.md** - Documentación completa de API (400+ líneas)
  - 17 comandos documentados
  - Ejemplos en Python y JavaScript
  - Tabla comparativa de dispositivos
  - Troubleshooting detallado
  - Límites y especificaciones

- ✅ **SETUP_v2.md** - Guía de configuración (350+ líneas)
  - Instalación para Arduino IDE
  - Instalación para PlatformIO
  - Selección de mejor microcontrolador
  - Primeras pruebas paso a paso
  - Solución de problemas completa

- ✅ **FEATURE_v2_README.md** - Resumen de rama
  - Overview de características
  - Checklist de prueba
  - Roadmap futuro
  - Comparativa v1 vs v2

#### Ejemplos
- ✅ **advanced_timeline.py** - Cliente Python v2 (460+ líneas)
  - Clase `CapellanLED` mejorada
  - Métodos para todos los efectos nuevos
  - Sistema de timeline completo
  - Utilidades de construcción de timeline
  - 3 demos incluidas (basic, effects, timeline)
  - Documentación inline completa

- ✅ **timeline_example.json** - Timeline de ejemplo completo
  - Show de festival de 45 segundos
  - Todos los efectos nuevos incluidos
  - Comentarios explicativos
  - Casos de uso reales

#### Detectar Dispositivo
- ✅ Detección automática en `STATUS`:
  ```
  OK|DEVICE:ESP32-C3|R:255|G:0|B:0|BRIGHTNESS:255|LEDS:60|TIMELINE:loaded
  ```
- ✅ Mensaje de inicio con tipo de dispositivo
- ✅ Pin LED automático según plataforma

### 🔄 Cambiado

#### Firmware
- 📝 Renombrado a `capellan-led-v2.ino` (mantiene compatibilidad con v1)
- 🔧 Estructura mejorada con secciones comentadas
- 🔧 Buffer serial aumentado a 512 bytes (de 256)
- 🔧 Función `processCommand()` refactorizada
- 🔧 Mejor manejo de parámetros en comandos

#### API
- 🔄 STATUS ahora incluye tipo de dispositivo
- 🔄 HELP mejorado con más información
- 🔄 Mejora en parsing de comandos con múltiples delimitadores

#### Documentación
- 📚 API.md → API_v2.md (mantener v1 como referencia)
- 📚 SETUP.md → SETUP_v2.md (mantener v1 como referencia)

### 🐛 Corregido

- ✅ Problema de variable shadowing en `theater()` (variable `b` conflictaba)
- ✅ Buffer overflow potencial en serial
- ✅ Manejo incorrecto de JSON vacío
- ✅ Memory leak en eventos de timeline

### 🗑️ Eliminado

- ❌ Dependencia de WiFi (ahora solo serial, más estable)
- ❌ Limitaciones de ESP8266 para ciertos efectos

### ⚠️ Deprecado

- ⚠️ Comando `ANIMATION|ADD|FRAMES` (será reemplazado por sistema timeline)
- ⚠️ Comando `PATTERN|NAME|PARAM1|PARAM2` (pendiente implementación)

---

## [1.0.0] - 2026-06-11

### 🎉 Agregado

- ✅ Soporte inicial para ESP8266 D1 Mini
- ✅ 4 efectos básicos: COLOR, BRIGHTNESS, RAINBOW, PULSE, THEATER
- ✅ Control serial a 115200 baud
- ✅ Web interface básica
- ✅ Cliente Python simple
- ✅ Documentación inicial
- ✅ Sistema de timeline planeado

---

## Comparación de Versiones

| Aspecto | v1 | v2 | Mejora |
|---------|----|----|--------|
| Dispositivos | 1 | 4 | +300% |
| Efectos | 4 | 10 | +150% |
| Almacenamiento | ❌ | ✅ | Nueva función |
| Timeline | Planeado | ✅ | Implementado |
| Líneas de código | 226 | 900+ | +300% |
| Documentación | 2 archivos | 7 archivos | +250% |
| Ejemplos | 2 | 4 | +100% |
| Eventos timeline máx | 100 | 500 | +400% |

---

## 🔍 Detalles de Implementación

### Detección de Plataforma

```cpp
#if defined(ESP32)
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
#elif defined(ESP8266)
  #define DEVICE_TYPE "ESP8266-D1"
  #define LED_PIN D4
#endif
```

### Sistema de Archivos

| Plataforma | Sistema | Capacidad | Uso |
|-----------|---------|-----------|-----|
| ESP8266 | LittleFS | 3MB | Timeline + Config |
| ESP32 | SPIFFS | 1.5MB | Timeline + Config |
| ESP32-C3 | SPIFFS | 1.5MB | Timeline + Config |
| ESP32-S3 | SPIFFS | 3MB | Timeline + Config |

### Estructura de Timeline

```json
{
  "version": "1.0",
  "metadata": {
    "title": "Show Name",
    "duration": 30000,
    "bpm": 128
  },
  "events": [
    {
      "time": 0,
      "type": "color",
      "r": 255,
      "g": 0,
      "b": 0
    }
  ]
}
```

---

## 🎯 Roadmap Futuro

### v2.1 (Próximas semanas)
- [ ] Reproducción no-bloqueante de timeline
- [ ] Búfer de eventos para streaming
- [ ] Múltiples dispositivos simultáneos
- [ ] Mejora de WebSocket

### v2.2 (Próximas semanas)
- [ ] Editor visual de timeline
- [ ] Sincronización con audio
- [ ] Más efectos (rainbow strobe, color fade, matrix)
- [ ] Grabación de shows en vivo

### v2.3 (Próximos meses)
- [ ] Soporte MQTT
- [ ] Integración Home Assistant
- [ ] API REST completa
- [ ] App móvil

### v3.0 (Futuro)
- [ ] Hardware acelerado
- [ ] Soporte para matrices LED (8x8, 16x16)
- [ ] Visualizador de espectro de audio
- [ ] IA para generación de efectos

---

## 📊 Estadísticas

### Tamaño de Código
```
v1.0.0:
  - Firmware: 226 líneas
  - Documentación: ~500 líneas
  - Ejemplos: ~300 líneas
  Total: ~1000 líneas

v2.0.0:
  - Firmware: 900+ líneas (+300%)
  - Documentación: ~2000 líneas (+300%)
  - Ejemplos: ~1200 líneas (+300%)
  Total: ~4100 líneas
```

### Característica por Línea
```
v1: 4 efectos / 226 líneas = 0.018 efectos/línea
v2: 10 efectos / 900 líneas = 0.011 efectos/línea
(Más complejo = menos eficiencia, pero mucho más funcionalidad)
```

---

## 🧪 Testing

### Probado en
- ✅ ESP8266 D1 Mini (4MB)
- ✅ ESP32 WROOM-32 (Simulado)
- ✅ ESP32-C3 (Simulado)
- ✅ Arduino IDE 2.x
- ✅ PlatformIO

### No Probado
- ⚠️ Múltiples dispositivos simultáneos
- ⚠️ Timeline > 1MB
- ⚠️ > 300 eventos en timeline

---

## 🤝 Contribución

Para reportar bugs o sugerir mejoras:

1. Usa [GitHub Issues](https://github.com/yrost97-afk/capellan-led/issues)
2. Incluye versión de firmware
3. Describe comportamiento esperado vs. actual
4. Adjunta logs si es posible

---

## 📄 Licencia

MIT License - Ver LICENSE.md

---

## 👤 Autores

- **yrost97-afk** - Autor principal
- **Comunidad** - Feedback y contribuciones

---

## 🙏 Agradecimientos

- Librería Adafruit NeoPixel
- Librería ArduinoJson
- Comunidad Arduino
- Comunidad Flow Arts

---

## 📞 Contacto

- 📧 Email: yrost97@gmail.com
- 🐙 GitHub: @yrost97-afk
- 💬 Discussions: [Link](https://github.com/yrost97-afk/capellan-led/discussions)

---

**Última actualización**: 2026-06-15
**Versión actual**: 2.0.0
**Estado**: ✅ Estable para testing

# ⚡ Quick Start - Capellán LED v2.0.0

¡Comienza en 5 minutos! Guía rápida para instalar y usar Capellán LED v2.

---

## 🚀 Instalación (3 pasos)

### Paso 1: Descargar Firmware
```bash
git clone https://github.com/yrost97-afk/capellan-led.git
cd capellan-led
git checkout feature/timeline-storage-esp32-effects
```

### Paso 2: Instalar Herramientas
**Arduino IDE:**
- Descarga desde [arduino.cc](https://www.arduino.cc/en/software)
- Instala board support: ESP8266 + ESP32
- Instala librerías: Adafruit NeoPixel + ArduinoJson

**PlatformIO:**
```bash
pip install platformio
```

### Paso 3: Cargar Firmware

**Arduino IDE:**
1. Abre `firmware/capellan-led-v2.ino`
2. Herramientas → Placa → Selecciona tu modelo
3. Herramientas → Puerto → Selecciona COM/USB
4. Clic Subir (Ctrl+U)

**PlatformIO:**
```bash
cd firmware
pio run -t upload
```

✅ **¡Listo!** Verás el mensaje: `Ready! Type 'HELP' for commands`

---

## 💡 Primeros Comandos (30 segundos)

Abre el **Monitor Serial** a **115200 baud** y copia/pega:

```bash
# Rojo brillante
COLOR|255|0|0

# Destello blanco
STROBE|255|255|255|100|2000

# LEDs corren alrededor (chase verde)
CHASE|0|255|0|50|5000

# Ver estado
STATUS

# Apagar
OFF
```

✅ **¡Verás los LEDs reaccionar en tiempo real!**

---

## 🎮 Efectos Disponibles

### Clásicos
```bash
COLOR|R|G|B                      # Color sólido (0-255 cada uno)
BRIGHTNESS|LEVEL                 # Brillo (0-255)
RAINBOW|SPEED                    # Arcoíris (1-100)
PULSE|R|G|B|SPEED               # Pulso
THEATER|R|G|B|SPEED             # Efecto teatro
```

### Nuevos ✨
```bash
STROBE|R|G|B|SPEED|DURATION     # Parpadeo estilo disco
CHASE|R|G|B|SPEED|DURATION      # LEDs corren alrededor
FIRE|SPEED|DURATION              # Fuego realista
RAINBOW_CYCLE|SPEED|DURATION    # Arcoíris continuo
BREATHE|R|G|B|SPEED|DURATION   # Respiración zen
SPARKLE|R|G|B|SPEED|DURATION   # Destellos aleatorios
```

### Ejemplos Listos para Copiar/Pegar

```bash
# Discoteca (blanco rápido)
STROBE|255|255|255|50|5000

# Party (verde brillante)
CHASE|0|255|0|30|8000

# Fuego realista
FIRE|30|5000

# Efecto mágico
SPARKLE|255|215|0|100|5000

# Calma
BREATHE|0|0|255|3|10000

# Arcoíris suave
RAINBOW_CYCLE|50|10000
```

---

## 📱 Usar Timeline (Opcional)

### Opción A: JSON Simple (30 segundos)

1. Copia este JSON en el monitor serial:

```bash
UPLOAD_TIMELINE|{"version":"1.0","metadata":{"title":"Demo"},"events":[{"time":0,"type":"color","r":255,"g":0,"b":0},{"time":2000,"type":"effect","effect":"strobe","r":255,"g":255,"b":255,"speed":100,"duration":2000},{"time":5000,"type":"brightness","level":0}]}
```

2. Presiona Enter
3. Ejecuta: `TIMELINE|PLAY`

✅ El show se reproduce automáticamente

### Opción B: Python (1 minuto)

```bash
cd examples
python advanced_timeline.py effects
```

✅ Verás todos los efectos ejecutados automáticamente

### Opción C: Crear Personalizado (5 minutos)

Archivo: `mi_timeline.json`
```json
{
  "version": "1.0",
  "metadata": {
    "title": "Mi Show",
    "duration": 15000,
    "bpm": 128
  },
  "events": [
    {
      "time": 0,
      "type": "color",
      "r": 255,
      "g": 0,
      "b": 0
    },
    {
      "time": 2000,
      "type": "effect",
      "effect": "strobe",
      "r": 255,
      "g": 255,
      "b": 255,
      "speed": 100,
      "duration": 2000
    },
    {
      "time": 5000,
      "type": "effect",
      "effect": "fire",
      "speed": 30,
      "duration": 4000
    },
    {
      "time": 10000,
      "type": "brightness",
      "level": 0
    }
  ]
}
```

Luego en Python:
```python
from advanced_timeline import CapellanLED
import json

led = CapellanLED('COM3')  # O /dev/ttyUSB0 en Linux/Mac

with open('mi_timeline.json') as f:
    timeline = json.load(f)

led.upload_timeline(timeline)
led.play_timeline()
led.close()
```

✅ Tu timeline personalizado se reproduce

---

## 🎨 Paleta de Colores Rápida

Copia/pega estos comandos para probar colores:

```bash
# Rojos
COLOR|255|0|0          # Rojo puro
COLOR|255|100|0        # Rojo oscuro
COLOR|255|255|0        # Amarillo

# Verdes
COLOR|0|255|0          # Verde puro
COLOR|0|255|255        # Cyan
COLOR|255|0|255        # Magenta

# Azules
COLOR|0|0|255          # Azul puro
COLOR|128|0|255        # Púrpura

# Blancos
COLOR|255|255|255      # Blanco
COLOR|128|128|128      # Gris
COLOR|0|0|0            # Negro (apagado)
```

---

## 🔌 Solución de Problemas Rápida

| Problema | Solución |
|----------|----------|
| **"Puerto no encontrado"** | Instala drivers CH340 (esp8266) o reinstala placa |
| **"Compilación falla"** | Verifica librerías instaladas (Adafruit NeoPixel + ArduinoJson) |
| **"LEDs no encienden"** | Verifica conexión física, voltaje 5V, pin correcto |
| **"Colores errados"** | Agrega resistencia 470Ω entre pin de datos y LEDs |
| **"Monitor serial vacio"** | Verifica velocidad a 115200 baud |

---

## 📱 Dispositivos Soportados

```
┌─────────────────────────────────────┐
│ Tu Dispositivo                      │
├─────────────────────────────────────┤
│ ESP8266 D1 Mini       → Pin D4      │ ✅ Soportado
│ ESP32 WROOM-32        → GPIO13      │ ✅ Soportado
│ ESP32-C3              → GPIO8       │ ✅ Soportado
│ ESP32-S3              → GPIO16      │ ✅ Soportado
└─────────────────────────────────────┘
```

**¿No sabes cuál tienes?** Ver en Arduino IDE:
- Herramientas → Información Placa

---

## 🧪 3 Demos Automáticas

```bash
# Demo 1: Efectos básicos
cd examples
python client_python.py

# Demo 2: Nuevos efectos
python advanced_timeline.py effects

# Demo 3: Timeline completo
python advanced_timeline.py timeline
```

---

## 📚 Documentación Completa

Para más detalles, consulta:

| Documento | Contenido |
|-----------|-----------|
| **API_v2.md** | Todos los comandos (referencia completa) |
| **SETUP_v2.md** | Instalación detallada por plataforma |
| **TIMELINE.md** | Sistema timeline (avanzado) |
| **advanced_timeline.py** | Cliente Python con ejemplos |
| **CHANGELOG.md** | Historial de cambios v1→v2 |

---

## 🎯 Próximos Pasos

1. **Probado básico** → Ir a `docs/API_v2.md`
2. **Quiero más efectos** → Ver lista completa en documentación
3. **Crear shows** → Usar `examples/advanced_timeline.py`
4. **Problemas** → Consultar `docs/SETUP_v2.md` troubleshooting
5. **Sugerir mejoras** → Abre Issue en GitHub

---

## 💬 Preguntas Frecuentes

**P: ¿Puedo conectar múltiples microcontroladores?**  
R: Sí, cada uno por un puerto COM/USB diferente. Próxima versión: sincronización automática.

**P: ¿Cuántos LEDs soporta?**  
R: Hasta 300 LEDs (limitado por RAM). Con 60-120 LEDs tienes máximo rendimiento.

**P: ¿Puedo usar otros pines?**  
R: Sí, edita `#define LED_PIN` en el firmware (línea 7-20).

**P: ¿Es bloqueante durante timeline?**  
R: Sí en v2.0. Próxima versión (v2.1): reproducción no-bloqueante.

**P: ¿Dónde guarda los archivos?**  
R: En almacenamiento interno (SPIFFS/LittleFS) del microcontrolador.

---

## 🎉 Cheat Sheet - Comandos Favoritos

```bash
# Empezar simple
COLOR|255|0|0          # Rojo
BRIGHTNESS|128         # Brillo 50%
OFF                    # Apagar

# Efectos rápidos
STROBE|255|255|255|100|2000  # Blanco parpadeante
FIRE|30|5000                  # Fuego 5s
BREATHE|0|0|255|5|5000       # Azul respiración

# Timeline
UPLOAD_TIMELINE|{JSON}  # Cargar timeline
TIMELINE|PLAY           # Reproducir
TIMELINE|DELETE         # Eliminar

# Sistema
STATUS                  # Ver estado
HELP                    # Ver todos comandos
CONFIG|SAVE            # Guardar configuración
```

---

## ✅ Checklist de Instalación

- [ ] Descargué el repositorio
- [ ] Instalé Arduino IDE o PlatformIO
- [ ] Instalé librerías (NeoPixel + ArduinoJson)
- [ ] Seleccioné la placa correcta
- [ ] Seleccioné el puerto COM correcto
- [ ] Cargué el firmware exitosamente
- [ ] Abrí el monitor serial a 115200 baud
- [ ] Envié `STATUS` y vi respuesta
- [ ] Envié `COLOR|255|0|0` y vi LEDs rojo
- [ ] Probé un efecto nuevo (ej: FIRE)
- [ ] Leí los documentos principales
- [ ] Probé un timeline JSON

🎉 **¡Si marcaste todo, estás listo!**

---

## 🚀 Siguientes Hitos

1. **Hoy**: Instala y prueba efectos básicos ✅
2. **Mañana**: Crea tu primer timeline
3. **Esta semana**: Construye shows personalizados
4. **Próxima semana**: Integra con música
5. **Futuro**: Sincroniza múltiples dispositivos

---

## 📞 Necesitas Ayuda?

- 📖 Lee `docs/SETUP_v2.md` (solución de problemas)
- 💬 Abre Issue en GitHub (describe el problema)
- 📧 Email: yrost97@gmail.com
- 🔗 GitHub: https://github.com/yrost97-afk/capellan-led

---

## 📄 Versión

**Capellán LED v2.0.0**  
**Última actualización**: 2026-06-15  
**Rama**: `feature/timeline-storage-esp32-effects`  

---

## 🙏 Agradecimientos

- Librería Adafruit NeoPixel
- Librería ArduinoJson
- Comunidad Arduino
- Comunidad Flow Arts

---

**¡Feliz creación de luces! 🎆**

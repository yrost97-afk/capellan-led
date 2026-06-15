# 🎆 Capellán LED v2 - Feature Branch

Rama de desarrollo con soporte multi-dispositivo, sistema de timeline avanzado y nuevos efectos LED.

## ✨ Nuevas Características

### 1. Soporte Multi-ESP
- ✅ **ESP8266 D1 Mini** (GPIO2)
- ✅ **ESP32 WROOM-32** (GPIO13)
- ✅ **ESP32-C3** (GPIO8)
- ✅ **ESP32-S3** (GPIO16)

**Detección automática** - sin necesidad de configuración manual

### 2. Sistema de Timeline Almacenado
- 📁 Almacenamiento en **SPIFFS/LittleFS**
- ⬆️ Descarga de timeline directo al ESP
- ⏱️ Reproducción sincronizada
- 🎯 Soporte para 500 eventos máximo
- 💾 Guardado automático de configuración

### 3. Nuevos Efectos LED

#### Clásicos (mantenidos)
- `COLOR|R|G|B` - Color sólido
- `RAINBOW|speed` - Arcoíris secuencial
- `PULSE|R|G|B|speed` - Pulso de brillo
- `THEATER|R|G|B|speed` - Efecto teatro

#### Nuevos Efectos
- **STROBE** - Parpadeo rápido estilo discoteca
- **CHASE** - LEDs corren alrededor
- **FIRE** - Simulación realista de fuego
- **RAINBOW_CYCLE** - Arcoíris continuo para todos
- **BREATHE** - Respiración suave zen
- **SPARKLE** - Destellos aleatorios mágicos

### 4. API Serial Mejorada
```
COMANDO|PARAM1|PARAM2|...\n
```

Nuevos comandos:
- `TIMELINE|LOAD` - Cargar desde almacenamiento
- `TIMELINE|PLAY` - Reproducir
- `TIMELINE|DELETE` - Eliminar
- `TIMELINE|STATUS` - Ver estado
- `TIMELINE|LIST` - Listar archivos
- `UPLOAD_TIMELINE|{JSON}` - Subir timeline
- `CONFIG|SAVE` - Guardar configuración
- `CONFIG|LOAD` - Cargar configuración

---

## 📁 Archivos Modificados/Agregados

### Firmware
```
firmware/
├── capellan-led-v2.ino (NUEVO - 900+ líneas)
│   ├── Soporte multi-ESP automático
│   ├── Sistema de timeline completo
│   ├── 6 nuevos efectos
│   ├── Gestión de archivos SPIFFS/LittleFS
│   └── Almacenamiento de configuración
└── platformio.ini (actualizado)
```

### Documentación
```
docs/
├── API_v2.md (NUEVO - documentación completa)
│   ├── 17 comandos documentados
│   ├── Ejemplos Python y JavaScript
│   ├── Troubleshooting detallado
│   └── Límites y especificaciones
├── SETUP_v2.md (NUEVO - guía instalación)
│   ├── Soporte multi-dispositivo
│   ├── Arduino IDE + PlatformIO
│   ├── Primeras pruebas
│   ├── Sistema timeline
│   └── Solución de problemas
└── TIMELINE.md (existente, mantener)
```

### Ejemplos
```
examples/
├── advanced_timeline.py (NUEVO - cliente Python v2)
│   ├── 6 métodos de efectos nuevos
│   ├── Sistema timeline completo
│   ├── 3 demos incluidas
│   └── Utilidades para crear timelines
├── timeline_example.json (NUEVO)
│   └── Show de festival completo
└── client_python.py (existente)
```

---

## 🚀 Inicio Rápido

### 1. Instalar Firmware

```bash
# Arduino IDE
1. Abre firmware/capellan-led-v2.ino
2. Herramientas → Placa → "ESP32-C3 Dev Module" (o tu modelo)
3. Clic Subir (Ctrl+U)

# O con PlatformIO
cd firmware
pio run -t upload
```

### 2. Probar Efectos Básicos

```bash
# Monitor serial a 115200 baud
COLOR|255|0|0          # Rojo
STROBE|255|255|255|100|2000  # Destello blanco 2s
CHASE|0|255|0|50|5000  # Chase verde 5s
FIRE|30|3000           # Fuego 3s
BREATHE|0|0|255|5|5000 # Respiración azul
SPARKLE|255|215|0|100|5000  # Destellos dorados
STATUS                 # Ver estado
```

### 3. Usar Timeline

**Python:**
```bash
python examples/advanced_timeline.py timeline
```

**Crear timeline personalizado:**
```python
import json

timeline = {
    "version": "1.0",
    "metadata": {"title": "Mi Show", "bpm": 128},
    "events": [
        {"time": 0, "type": "color", "r": 255, "g": 0, "b": 0},
        {"time": 2000, "type": "effect", "effect": "strobe", 
         "r": 255, "g": 255, "b": 255, "speed": 100, "duration": 2000},
        {"time": 5000, "type": "effect", "effect": "chase",
         "r": 0, "g": 255, "b": 0, "speed": 50, "duration": 5000}
    ]
}

with open('timeline.json', 'w') as f:
    json.dump(timeline, f, indent=2)
```

---

## 📊 Comparación v1 vs v2

| Característica | v1 | v2 |
|---|---|---|
| Dispositivos soportados | ESP8266 | 4 (ESP8266, ESP32 x3) |
| Efectos disponibles | 4 | 10 |
| Sistema de timeline | Planeado | ✅ Implementado |
| Almacenamiento | No | ✅ SPIFFS/LittleFS |
| Configuración guardada | No | ✅ EEPROM/Preferences |
| Eventos timeline máximo | 100 | 500 |
| Documentación | 2 archivos | 7 archivos |
| Ejemplos | 2 | 4 |
| Líneas de firmware | 226 | 900+ |

---

## 🔧 Especificaciones Técnicas

### Hardware Compatible

| Modelo | Flash | RAM | Pin LED | Precio |
|--------|-------|-----|---------|--------|
| ESP8266 D1 | 4MB | 160KB | D4 | $5 |
| ESP32 WROOM | 4MB | 520KB | GPIO13 | $10 |
| ESP32-C3 | 4MB | 400KB | GPIO8 | $7 |
| ESP32-S3 | 8MB | 520KB | GPIO16 | $9 |

### Límites de Software

- **Máximo eventos timeline**: 500
- **Máximo LEDs**: 300 (limitado por RAM)
- **Duración de efecto**: 65535 ms
- **Resolución de tiempo**: 1 ms
- **Velocidad serial**: 115200 baud (fijo)

---

## 📋 Checklist de Prueba

- [ ] Compilar firmware sin errores
- [ ] Cargar en ESP8266
- [ ] Cargar en ESP32-WROOM
- [ ] Cargar en ESP32-C3
- [ ] Probar COLOR
- [ ] Probar STROBE
- [ ] Probar CHASE
- [ ] Probar FIRE
- [ ] Probar RAINBOW_CYCLE
- [ ] Probar BREATHE
- [ ] Probar SPARKLE
- [ ] Probar TIMELINE|LOAD
- [ ] Probar TIMELINE|PLAY
- [ ] Probar UPLOAD_TIMELINE
- [ ] Probar CONFIG|SAVE
- [ ] Probar archivo JSON timeline
- [ ] Probar cliente Python
- [ ] Probar en navegador

---

## 🐛 Problemas Conocidos

### ESP8266
- ⚠️ RAM limitada, máximo ~100-150 eventos en timeline
- ⚠️ Velocidad más lenta que ESP32
- ✅ Totalmente funcional para usos básicos

### Timeline
- ⚠️ Bloqueante durante reproducción (no se pueden enviar comandos)
- ✅ Usar OFF para cancelar
- ✅ Próximo: Reproducción no-bloqueante

### Almacenamiento
- ⚠️ SPIFFS en ESP32 requiere formateo inicial
- ✅ Herramientas → ESP32 Sketch Data Upload

---

## 🎯 Próximas Mejoras (Roadmap)

### v2.1
- [ ] Reproducción no-bloqueante de timeline
- [ ] Múltiples dispositivos simultáneos
- [ ] WebSocket para control remoto

### v2.2
- [ ] Editor visual de timeline
- [ ] Sincronización de audio
- [ ] Más efectos (rainbow strobe, color fade, etc.)

### v2.3
- [ ] Soporte MQTT
- [ ] Control desde Home Assistant
- [ ] API REST

### v3.0
- [ ] Hardware acelerado
- [ ] Soporte para matrices LED
- [ ] Visualizador de espectro

---

## 📚 Documentación Relacionada

- **API_v2.md** - Referencia completa de comandos
- **SETUP_v2.md** - Instalación paso a paso
- **TIMELINE.md** - Sistema timeline (existente)
- **examples/advanced_timeline.py** - Cliente Python v2
- **examples/timeline_example.json** - Timeline de ejemplo

---

## 🤝 Contribuir

Para contribuir a esta rama:

1. Prueba todas las características
2. Reporta bugs en Issues
3. Sugiere mejoras en Discussions
4. Envía PRs con código limpio

---

## 📝 Licencia

MIT - Libre para usar y modificar

---

## 👤 Autor

**yrost97-afk**

Versión: **2.0.0-beta**
Fecha: **2026-06-15**

---

## 🎉 Agradecimientos

- Librería **Adafruit NeoPixel**
- Librería **ArduinoJson**
- Comunidad de Flow Arts
- Testers y contribuidores

---

## 📞 Soporte

- 📖 Documentación: `/docs`
- 💬 Discussions: GitHub Discussions
- 🐛 Bugs: GitHub Issues
- 💡 Ideas: GitHub Issues (label: enhancement)

---

**Estado de Rama**: 🟢 Stables for Testing

Próximo paso: **Pull Request → Main Branch**

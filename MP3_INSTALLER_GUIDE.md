# 🎆 Capellán LED v3 - Mejoras MP3 & Descargas Rápidas

## ✨ Nuevas Características

### 1. **Soporte MP3 Completo**
- ✅ Carga de archivos MP3 directamente en ESP32
- ✅ Sincronización automática de timeline con música
- ✅ Metadata MP3 (duración, bitrate, canales)
- ✅ Reproducción sincronizada LED + Audio

### 2. **Descargas Rápidas sin Errores**
- ✅ Protocolo mejorado a **921600 baud** (8x más rápido)
- ✅ Compresión automática de datos
- ✅ Verificación CRC para integridad
- ✅ Reintentos automáticos en caso de error
- ✅ Progreso en tiempo real

### 3. **Installer EXE para Windows**
- ✅ Instalador profesional NSIS
- ✅ Versión portátil sin instalación
- ✅ Interfaz gráfica completa
- ✅ Gestor de firmware integrado
- ✅ Monitor serial incorporado

---

## 🚀 Cambios en Firmware

### Archivo: `firmware/capellan-led-mp3-enhanced.ino`

**Nuevas funciones:**
```cpp
// MP3 Management
void parseMP3Metadata(const String& filename)
void uploadMP3Chunk(uint32_t offset, uint32_t chunkSize)
void saveMP3FromSerial(const String& data)

// Enhanced Downloads
void downloadTimelineToPC()  // 8x faster
void processCommand(String cmd) // Improved protocol

// Storage
#define MAX_TIMELINE_EVENTS 1000  // Up from 500
#define MP3_FILE "/audio.mp3"
#define CHUNK_SIZE 2048
#define BAUD_RATE_HIGH 921600
```

**Nuevos comandos:**
```
MP3|UPLOAD|{hex_data}    - Subir MP3 por serial
MP3|INFO                 - Ver info del MP3
TIMELINE|DOWNLOAD        - Descargar timeline rápido a PC
```

---

## 📥 Instalación del App

### Windows

#### Opción 1: Instalador NSIS
```bash
cd installer
npm install
npm run build:installer
# Ejecutar Capellan-LED-Setup-1.0.0.exe
```

#### Opción 2: Portátil (sin instalación)
```bash
cd installer
npm install
npm run build:portable
# Ejecutar Capellan-LED-1.0.0-portable.exe
```

#### Opción 3: Desde código
```bash
cd installer
npm install
npm run electron:dev
```

---

## 💻 Uso del App

### Panel de Control
1. **Dashboard** - Estado general del sistema
2. **Timeline Editor** - Crear/editar timelines MP3
3. **Firmware** - Descargar y gestionar firmware
4. **Ayuda** - Documentación y guías

### Flujo Principal
```
1. Conecta ESP32 por USB
2. Abre Capellán LED App
3. Firmware → Descargar v3.0
4. Copia código en Arduino IDE
5. Carga en tu dispositivo
6. Timeline Editor → Selecciona MP3
7. Crea eventos sincronizados
8. Sube a ESP32
9. ¡Reproduce el show!
```

---

## 🔧 Configuración Arduino IDE

### Para ESP32-C3:
```
Board: ESP32-C3 Dev Module
Flash Size: 4MB
Partition Scheme: Default 4MB
```

### Para ESP8266:
```
Board: NodeMCU 1.0 (ESP-12E Module)
Flash Size: 4M (FS: 3M, OTA: ~500KB)
```

---

## 📊 Especificaciones v3.0

| Característica | v2.0 | v3.0 | Mejora |
|---|---|---|---|
| Events por timeline | 500 | 1000 | ✅ 2x |
| Velocidad descarga | 115200 baud | 921600 baud | ✅ 8x |
| Soporte MP3 | ❌ | ✅ | ✅ Nuevo |
| App gráfica | ❌ | ✅ | ✅ Nuevo |
| Compresión datos | ❌ | ✅ | ✅ Nuevo |
| Reintentos auto | ❌ | ✅ | ✅ Nuevo |

---

## 📝 Comandos Serial v3.0

### Color & Efectos (igual que v2)
```
COLOR|R|G|B
BRIGHTNESS|0-255
STROBE|R|G|B|speed|duration
CHASE|R|G|B|speed|duration
```

### Timeline (mejorado)
```
TIMELINE|LOAD                    # Cargar desde almacenamiento
TIMELINE|PLAY                    # Reproducir
TIMELINE|DOWNLOAD                # Descargar a PC (8x rápido)
UPLOAD_TIMELINE|{JSON}           # Subir timeline
```

### MP3 (NUEVO)
```
MP3|UPLOAD|{chunk_hex}           # Subir MP3 por partes
MP3|INFO                         # Ver info MP3 cargado
MP3|SYNC|{bpm}                   # Sincronizar con BPM
```

---

## 🐛 Resolución de Problemas

### La descarga es lenta
✅ **Solución**: Usa BAUD_RATE_HIGH (921600) - mucho más rápida

### Error al subir MP3
✅ **Solución**: 
1. Verifica el tamaño (máx 2MB recomendado)
2. Usa formato MP3 comprimido
3. Reinicia el dispositivo

### Timeline no sincroniza con MP3
✅ **Solución**:
1. Ve a Firmware → Monitor Serial
2. Envía: `MP3|INFO` para verificar carga
3. Verifica los tiempos en millisegundos

---

## 🎯 Próximas Mejoras (Roadmap)

- [ ] v3.1: Soporte para WAV y FLAC
- [ ] v3.2: Grabación de video sincronizado
- [ ] v3.5: App para Linux y macOS
- [ ] v4.0: Editor visual arrastra/suelta
- [ ] v4.5: Comunidad de timelines compartidos

---

## 📧 Soporte

**GitHub**: [yrost97-afk/capellan-led](https://github.com/yrost97-afk/capellan-led)
**Issues**: [Reportar problema](https://github.com/yrost97-afk/capellan-led/issues)
**Email**: yrost97@gmail.com

---

**Capellán LED v3.0** - ¡Ahora con MP3 y más rápido! 🎵⚡

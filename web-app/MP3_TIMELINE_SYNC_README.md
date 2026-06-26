# 🎵 MP3 Player + Timeline Sync - Guía Completa

## ✨ Nuevas Funcionalidades Agregadas

Tu app **Capellán LED - MP3 Manager Pro** ahora incluye:

### **1. 🎵 Reproductor MP3 Integrado**
- **Web Audio API** para reproducir MP3 en tiempo real
- **Sincronización automática** entre MP3 y eventos del timeline
- Control de reproducción: Play, Pause, Stop, Seek
- Barra de progreso visual con tiempo actual/total
- Vista previa LED animada en tiempo real

### **2. 📤 Descarga de Timeline al Dispositivo**
- Envío de timelines completos al ESP32 vía USB
- Almacenamiento en SPIFFS/LittleFS del microcontrolador
- Gestión de múltiples timelines en el dispositivo
- Lista, reproducir, eliminar timelines guardados

### **3. 🔄 Reproducción Sincronizada**
- **3 modos de reproducción:**
  - Solo Web: MP3 + timeline en navegador
  - Solo Dispositivo: Ejecuta timeline guardado en ESP32
  - Sincronizado: Web + dispositivo reproducen juntos

---

## 📁 Archivos Nuevos Creados

```
web-app/
├── audio-sync-player.js          # Motor de reproducción MP3 + sync
├── device-timeline-manager.js    # Gestión de timelines en ESP32
├── mp3-playback-controller.js    # Controlador de reproducción
└── mp3-player-styles.css         # Estilos del reproductor MP3
```

---

## 🚀 Cómo Usar

### **Paso 1: Cargar un MP3**

```javascript
// Opción A: Desde archivo local
const file = document.getElementById('mp3Input').files[0];
await loadMP3ForPlayback(file);

// Opción B: Desde IndexedDB (MP3s guardados)
await loadMP3ForPlayback('mi-cancion.mp3');
```

### **Paso 2: Crear Timeline**

Arrastra colores y efectos al timeline como siempre. Ejemplo de estructura interna:

```javascript
timelineEvents = [
    {
        time: 0,           // ms desde inicio
        type: 'color',
        r: 255, g: 0, b: 0
    },
    {
        time: 2000,
        type: 'effect',
        effectName: 'FIRE',
        speed: 30,
        duration: 5000
    }
];
```

### **Paso 3: Reproducir en Web**

```javascript
// Reproduce MP3 + timeline sincronizados en navegador
await playMP3WithTimeline();

// Controles
pauseMP3WithTimeline();    // Pausa
resumeMP3WithTimeline();   // Reanuda
stopMP3WithTimeline();     // Detiene
seekMP3Time(5000);         // Busca a 5 segundos
```

### **Paso 4: Enviar Timeline al Dispositivo**

```javascript
// Subir timeline al ESP32
const success = await uploadTimelineToDevice(timelineData, 'mi-show');

// Listar timelines en dispositivo
const timelines = await listDeviceTimelines();
// [
//   {name: 'mi-show', duration: 15000, size: 2048},
//   {name: 'otra-show', duration: 10000, size: 1536}
// ]

// Ejecutar timeline en dispositivo
await playDeviceTimeline('mi-show');

// Detener
await stopDeviceTimeline();

// Eliminar
await deleteDeviceTimeline('mi-show');

// Descargar de vuelta a PC
await downloadTimelineFromDevice('mi-show');
```

### **Paso 5: Reproducción Sincronizada (Web + Dispositivo)**

```javascript
// Reproduce simultáneamente en ambos lados
await syncPlayOnDevice();
// 1. Sube timeline al ESP32
// 2. Reproduce MP3 en web
// 3. Ejecuta timeline en dispositivo (sincronizado)
```

---

## 🎯 Protocolo Serial (ESP32)

Los nuevos comandos disponibles en el firmware:

```bash
# Subir timeline
TIMELINE|UPLOAD|{json}
TIMELINE|START|nombre
TIMELINE|CHUNK|índice|datos
TIMELINE|END|cantidad_chunks

# Gestión
TIMELINE|LIST              # Listar todos
TIMELINE|INFO|nombre       # Obtener info
TIMELINE|DOWNLOAD|nombre   # Descargar

# Reproducción
TIMELINE|PLAY|nombre       # Ejecutar específico
TIMELINE|STOP              # Detener

# Eliminación
TIMELINE|DELETE|nombre     # Borrar

# Sincronización
TIMELINE|SYNC              # Modo sincronizado
```

---

## 📊 Estructura de Datos - Timeline

### **Formato completo guardado:**

```json
{
  "name": "mi-show",
  "duration": 15000,
  "version": "2.0",
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
      "effectName": "STROBE",
      "r": 255,
      "g": 255,
      "b": 255,
      "speed": 100,
      "duration": 2000
    },
    {
      "time": 5000,
      "type": "brightness",
      "level": 200
    },
    {
      "time": 7000,
      "type": "transition",
      "r": 0,
      "g": 100,
      "b": 255
    }
  ]
}
```

---

## 🔌 Callbacks y Eventos

### **Escuchar eventos del timeline:**

```javascript
// Registrar callback para cada evento
onTimelineEvent((event, currentTimeMs) => {
    console.log(`Evento en ${currentTimeMs}ms:`, event);
    
    // Aquí puedes:
    // - Actualizar UI
    // - Hacer cosas adicionales
    // - Registrar el timeline
});
```

### **Eventos disponibles:**

```javascript
// Durante reproducción web
updateLEDPreview(event);              // Anima vista previa
updateTimelineProgressIndicator(ms);  // Actualiza tiempo

// Durante sincronización con dispositivo
executeTimelineEvent(event);          // Envía al ESP32
playTimelineOnDevice(events);         // Ejecuta en dispositivo
```

---

## 📱 Características Especiales

### **1. Carga de archivos grandes**

Para timelines > 2KB, se envían en chunks:
- Tamaño: 1KB por chunk
- Espera entre chunks: 50ms
- Progreso en tiempo real

### **2. Sincronización de audio-video**

- Utiliza `requestAnimationFrame` para máxima precisión
- Corrección automática de desfase
- Tolerancia: ±50ms

### **3. Gestión de IndexedDB**

Los MP3s se guardan en IndexedDB para:
- Uso sin conexión
- Acceso rápido
- Persistencia local

### **4. Presets de reproducción**

```javascript
MP3_PLAYBACK_PRESETS = {
    'web-only': 'Solo Web',
    'device-only': 'Solo Dispositivo', 
    'sync': 'Sincronizado'
};

// Seleccionar
selectPlaybackMode('sync');
```

---

## 🎨 Ejemplo Completo: Show Sincronizado

```javascript
// 1. Cargar MP3
const file = document.getElementById('mp3Input').files[0];
await loadMP3ForPlayback(file);

// 2. Crear timeline (arrastrando en la UI)
// timelineEvents ya tiene los eventos del usuario

// 3. Reproducir en web primero (preview)
await playMP3WithTimeline();

// 4. Después, sincronizar con dispositivo
await syncPlayOnDevice();

// ¡Resultado: Luces + sonido perfectamente sincronizados!
```

---

## 🔧 Integración con HTML

Para usar estas nuevas funciones, incluir en `index.html`:

```html
<!-- Scripts -->
<script src="audio-sync-player.js"></script>
<script src="device-timeline-manager.js"></script>
<script src="mp3-playback-controller.js"></script>
<link rel="stylesheet" href="mp3-player-styles.css">
```

---

## 📋 Checklist de Implementación

- ✅ Motor de reproducción MP3 (Web Audio API)
- ✅ Sincronización timeline + audio
- ✅ Gestor de timelines del dispositivo
- ✅ Protocolo serial mejorado
- ✅ Exportar/importar timelines
- ✅ 3 modos de reproducción
- ✅ Vista previa LED en tiempo real
- ✅ Manejo de archivos grandes
- ✅ Interfaz de usuario (CSS)
- ✅ Callbacks y eventos

---

## 🐛 Troubleshooting

### **"Audio no se reproduce"**
- Verificar que el navegador soporta Web Audio API
- Comprobar permisos de sitio seguro (HTTPS o localhost)
- Revisar consola del navegador (F12)

### **"Timeline no sincroniza"**
- Comprobar que `timelineEvents` no está vacío
- Verificar que el MP3 se decodificó correctamente
- Revisar tiempos de los eventos (no deben exceder duración del audio)

### **"No se sube al dispositivo"**
- Verificar conexión USB (puerto COM abierto)
- Comprobar baud rate: 115200
- Revisar tamaño del timeline (no exceder 50KB)

### **"Sincronización desfasada"**
- Reducir velocidad de envío de eventos
- Aumentar tolerancia a ±100ms
- Verificar velocidad USB (cable defectuoso)

---

## 📚 Recursos

| Archivo | Función |
|---------|---------|
| `audio-sync-player.js` | Motor de reproducción |
| `device-timeline-manager.js` | Gestor de timelines ESP32 |
| `mp3-playback-controller.js` | Controlador de UI |
| `mp3-player-styles.css` | Estilos visuales |
| `timeline-executor.js` | Ejecución de eventos |

---

## 🎉 ¡Listo!

Ahora tu **Capellán LED - MP3 Manager Pro** tiene:
- ✨ Reproducción MP3 sincronizada
- 📤 Descarga de timelines al dispositivo
- 🔄 Sincronización web + dispositivo
- 🎨 Vista previa LED en tiempo real
- 📋 Gestión completa de timelines

**¡A crear shows increíbles!** 🎆

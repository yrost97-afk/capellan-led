# 🎵 MP3 Upload & Timeline Manager - Guía de Uso

## 📋 Descripción

Sistema completo para cargar archivos MP3 y crear timelines sincronizados con control de LEDs en tiempo real. 

**Características principales:**
- ✅ Carga de MP3 sin necesidad de conectar dispositivo USB
- ✅ Almacenamiento local en el navegador (IndexedDB)
- ✅ Control de tiempo preciso: Minutos : Segundos : Milisegundos
- ✅ Timeline visual con keyframes sincronizados
- ✅ Descarga rápida de archivos JSON
- ✅ Sincronización con dispositivo ESP8266/D1 cuando está conectado
- ✅ Modo offline completo

---

## 🚀 Inicio Rápido

### 1️⃣ Acceder a la Web

Abre el archivo `web-app/mp3-upload.html` en tu navegador:

```bash
# Opción 1: Abrir directamente
file:///ruta/a/capellan-led/web-app/mp3-upload.html

# Opción 2: Con servidor local (recomendado)
cd capellan-led/web-app
python -m http.server 8000
# Luego abre: http://localhost:8000/mp3-upload.html
```

### 2️⃣ Cargar un MP3

1. **Arrastra tu archivo MP3** al área de carga (o haz clic para seleccionar)
2. El archivo se guardará **automáticamente en el navegador**
3. Verás:
   - 📁 Nombre del archivo
   - 📊 Tamaño en MB
   - ⏱️ Duración estimada
   - 📈 Barra de progreso

### 3️⃣ Reproducir el MP3

Una vez cargado:
- Haz clic en **▶ Play** para reproducir
- El tiempo se actualiza en tiempo real
- La barra de progreso avanza sincronizadamente

---

## ⏱️ Crear Keyframes (Puntos de Control)

Los keyframes son puntos en el timeline donde ocurren cambios de color/efectos.

### Entrada de Tiempo Precisa

Usa los tres inputs para máxima precisión:

```
┌─────────────────────────────────────┐
│ ⏱️ Tiempo                            │
├─────────────────────────────────────┤
│ Minutos: [0] | Segundos: [30] | ms: [500] │
│ Resultado: 00:30.500              │
└─────────────────────────────────────┘
```

**Ejemplo:**
- Para `1:45.250` (1 minuto, 45 segundos, 250 milisegundos):
  - Minutos: `1`
  - Segundos: `45`
  - Milisegundos: `250`

### Agregar un Keyframe

1. **Establece el tiempo** (minutos, segundos, milisegundos)
2. **Selecciona el color** con el color picker
3. **Elige el efecto:**
   - 🎨 **Sólido** - Color fijo
   - 💓 **Pulso** - Pulsación de color
   - ⚡ **Estrobo** - Parpadeo rápido
   - 🌈 **Arcoíris** - Ciclo de colores
   - 🔥 **Fuego** - Efecto fuego
   - 🌬️ **Respiración** - Respiración suave
   - 🎯 **Chase** - Efecto corredor

4. **Ajusta el brillo** (0-100%)
5. Haz clic en **➕ Agregar Keyframe**

**Resultado:**
```
┌─────────────────────────────────────┐
│ 00:30.500  [Color]  Pulso • 85%  [✕] │
│ 01:45.250  [Color]  Fuego • 100% [✕] │
└─────────────────────────────────────┘
```

---

## 📂 Gestor de Archivos Locales

### Ver Archivos Guardados

Haz clic en **📂 Ver archivos guardados** para:
- Listar todos los MP3s guardados
- Ver tamaño y duración
- Cargar un archivo para editar
- Eliminar archivos

### Ejemplo:

```
📁 mi-cancion.mp3
   5.43 MB • 03:45
   guardado el 15 jun, 14:30
   [Editar] [✕]
```

---

## 💾 Guardar Timeline

### Descargar como JSON

1. Crea tus keyframes
2. Haz clic en **💾 Guardar**
3. Se descargará un archivo `cancion_timeline.json`

### Estructura del archivo:

```json
{
  "name": "mi-cancion.mp3_timeline",
  "mp3_file": "mi-cancion.mp3",
  "mp3_duration": 225.5,
  "keyframes": [
    {
      "timestamp": 30500,
      "time_formatted": "00:30.500",
      "r": 255,
      "g": 0,
      "b": 0,
      "brightness": 85,
      "effect": "pulse",
      "duration": 1000
    },
    {
      "timestamp": 105250,
      "time_formatted": "01:45.250",
      "r": 255,
      "g": 165,
      "b": 0,
      "brightness": 100,
      "effect": "fire",
      "duration": 1000
    }
  ],
  "created": "2026-06-15T14:30:00Z"
}
```

---

## 📂 Cargar Timeline Existente

1. Haz clic en **📂 Cargar**
2. Selecciona un archivo JSON guardado anteriormente
3. Los keyframes se cargarán automáticamente
4. Puedes editar o agregar más keyframes

---

## 📤 Sincronizar con Dispositivo

Si tienes un **ESP8266/D1 conectado por USB:**

1. El estado debe mostrar **🟢 Conectado**
2. Crea tu timeline en la web
3. Haz clic en **📤 Sincronizar**
4. Los datos se enviarán al dispositivo instantáneamente

**Si NO está conectado:**
- El botón estará deshabilitado
- Sigue usando la web normalmente
- Descarga el JSON cuando termines

---

## 🔄 Flujo de Trabajo Recomendado

### Para crear un show completo:

```
1. 📁 Cargar MP3
   ↓
2. ▶ Reproducir (para escuchar)
   ↓
3. ⏸ Pausar cuando quieras agregar un keyframe
   ↓
4. ⏱️ Establece tiempo exacto
   ↓
5. 🎨 Selecciona color y efecto
   ↓
6. ➕ Agrega keyframe
   ↓
7. Repite pasos 2-6 para cada cambio
   ↓
8. 💾 Guarda el timeline JSON
   ↓
9. 📤 Sincroniza con dispositivo (si está conectado)
```

---

## 📊 Estadísticas en Tiempo Real

Se actualiza automáticamente:

```
┌──────────────────────────────────────┐
│ 📊 Estadísticas                      │
├──────────────────────────────────────┤
│ Tamaño:     5.43 MB                  │
│ Duración:   03:45                    │
│ Keyframes:  12                       │
│ Velocidad:  2.5 MB/s                 │
└──────────────────────────────────────┘
```

---

## 🎬 Timeline Visual

La línea de tiempo muestra:
- 📏 Escala temporal (marcas en segundos)
- 🔴 Marcadores de keyframes (con color del efecto)
- 📍 Posición en tiempo real durante reproducción

```
0s          5s         10s        15s        20s
├───────────┼───────────┼───────────┼───────────┤
           [🔴]                          [🟡]
          Pulso Rojo                    Fuego Naranja
```

---

## ⚙️ Especificaciones Técnicas

### Almacenamiento Local

- **Base de datos:** IndexedDB
- **Capacidad:** ~50GB por navegador (depende del sistema)
- **Persistencia:** Los archivos se guardan indefinidamente
- **Acceso:** Solo en este dominio/archivo

### Formato de Tiempo

```
Formato: MM:SS.mmm

MM  = Minutos (00-59)
SS  = Segundos (00-59)
mmm = Milisegundos (000-999)

Ejemplos:
00:05.250 = 5 segundos y 250 ms
01:30.000 = 1 minuto 30 segundos
03:45.999 = 3 minutos 45 segundos 999 ms
```

### Valores de Brillo

```
0%   = Apagado
50%  = Medio
100% = Máximo brillo
```

### Colores RGB

El color picker utiliza valores RGB:
```
Rojo:       R:255 G:0   B:0
Verde:      R:0   G:255 B:0
Azul:       R:0   G:0   B:255
Blanco:     R:255 G:255 B:255
Amarillo:   R:255 G:255 B:0
```

---

## 🐛 Solución de Problemas

### El MP3 no se guarda
- ✅ Verifica que el archivo sea válido (extensión .mp3)
- ✅ Revisa que tenga menos de 100MB
- ✅ Limpia el caché del navegador

### No puedo reproducir
- ✅ Asegúrate de haber cargado un MP3 primero
- ✅ Presiona ▶ Play
- ✅ Verifica que el audio no esté mutead

### Los keyframes no se guardan
- ✅ Verifica que el tiempo sea válido
- ✅ Comprueba que sea menor que la duración del MP3
- ✅ Intenta refrescar la página

### Sincronización con dispositivo falla
- ✅ Conecta el dispositivo USB
- ✅ Verifica que el puerto COM sea el correcto
- ✅ Reinicia el dispositivo y la página web

### Bajo rendimiento
- ✅ Limpia archivos locales (eliminando MP3s viejos)
- ✅ Intenta con un archivo MP3 más pequeño
- ✅ Reinicia el navegador

---

## 📚 Archivos Incluidos

```
capellan-led/
├── web-app/
│   ├── mp3-upload.html          # Interfaz principal
│   ├── mp3-upload-v2.js         # Lógica y almacenamiento
│   └── mp3-styles.css           # Estilos (incluido en HTML)
│
├── firmware/
│   ├── audio_handler.h          # Librería ESP8266 para MP3
│   └── capellan-led-v2.ino      # Firmware principal
│
└── docs/
    └── MP3_TIMELINE_GUIDE.md    # Esta guía
```

---

## 🔗 Integración con Firmware

Para sincronizar con tu ESP8266, el firmware debe incluir:

```cpp
#include "audio_handler.h"

AudioHandler audioHandler;

void setup() {
    audioHandler.begin();
}

void loop() {
    // Recibir timeline desde serial
    if (Serial.available()) {
        String data = Serial.readStringUntil('\n');
        if (data.startsWith("TIMELINE|UPLOAD|")) {
            String json = data.substring(16);
            audioHandler.loadTimeline(json.c_str());
        }
    }
}
```

Ver `firmware/audio_handler.h` para más detalles.

---

## 💡 Tips y Trucos

### Para sincronización perfecta:

1. **Crea keyframes cada 0.5-1 segundo** para cambios suaves
2. **Usa milisegundos** para cambios más precisos (0.250, 0.500, etc.)
3. **Prueba con reproducción** antes de sincronizar
4. **Guarda versiones** de tus timelines (mp3_v1.json, mp3_v2.json)

### Para mejor rendimiento:

1. **Comprime tu MP3** a 128kbps o menos
2. **Evita archivos muy largos** (>10 minutos)
3. **Limpia archivos viejos** del almacenamiento local
4. **Usa navegadores modernos** (Chrome, Firefox, Edge)

### Para shows complejos:

1. **Crea submuestras** de la canción
2. **Edita cada parte por separado**
3. **Combina los timelines** después
4. **Prueba en dispositivo real** antes de usar en vivo

---

## 📞 Soporte

Para reportar problemas o sugerir mejoras:
1. Abre una Issue en GitHub
2. Incluye:
   - Navegador y versión
   - Tamaño del MP3
   - Pasos para reproducir
   - Captura de pantalla si es posible

---

## 📄 Licencia

MIT - Libre para usar y modificar

---

**Última actualización:** 15 de Junio de 2026  
**Versión:** 2.0

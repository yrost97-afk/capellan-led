# ⏱️ Timeline MP3 - Capellán LED

## Descripción General

El sistema de Timeline permite sincronizar efectos de LED con archivos MP3. Los efectos se reproducen automáticamente en momentos específicos de la canción.

**Estado**: 🔨 En Desarrollo (v0.1)

---

## Formato del Timeline (JSON)

### Estructura Base

```json
{
  "version": "1.0",
  "metadata": {
    "title": "Mi Canción",
    "artist": "Artista",
    "duration": 180,
    "bpm": 120,
    "audioFile": "cancion.mp3"
  },
  "events": [
    {
      "time": 0,
      "type": "color",
      "params": {
        "r": 255,
        "g": 0,
        "b": 0
      }
    },
    {
      "time": 5000,
      "type": "effect",
      "params": {
        "name": "pulse",
        "r": 0,
        "g": 255,
        "b": 0,
        "speed": 5
      }
    }
  ]
}
```

---

## Tipos de Eventos

### 1. Color - Cambio de Color

```json
{
  "time": 1000,
  "type": "color",
  "params": {
    "r": 255,
    "g": 128,
    "b": 0
  }
}
```

**Parámetros**:
- `time` (ms): Momento en el que ocurre el evento
- `r` (0-255): Rojo
- `g` (0-255): Verde
- `b` (0-255): Azul

---

### 2. Brightness - Ajuste de Brillo

```json
{
  "time": 5000,
  "type": "brightness",
  "params": {
    "level": 200,
    "duration": 1000
  }
}
```

**Parámetros**:
- `time` (ms): Momento
- `level` (0-255): Nivel de brillo
- `duration` (ms, opcional): Tiempo de transición

---

### 3. Effect - Efectos Especiales

```json
{
  "time": 10000,
  "type": "effect",
  "params": {
    "name": "rainbow",
    "speed": 20
  }
}
```

**Efectos disponibles**:

#### Rainbow
```json
{
  "name": "rainbow",
  "speed": 20
}
```

#### Pulse
```json
{
  "name": "pulse",
  "r": 255,
  "g": 0,
  "b": 0,
  "speed": 5
}
```

#### Theater
```json
{
  "name": "theater",
  "r": 255,
  "g": 255,
  "b": 255,
  "speed": 50
}
```

---

### 4. Transición - Transición Suave entre Colores

```json
{
  "time": 15000,
  "type": "transition",
  "params": {
    "fromR": 255,
    "fromG": 0,
    "fromB": 0,
    "toR": 0,
    "toG": 255,
    "toB": 0,
    "duration": 2000
  }
}
```

**Parámetros**:
- `fromR/G/B`: Color inicial
- `toR/G/B`: Color final
- `duration`: Tiempo de transición en ms

---

## Ejemplo Completo

```json
{
  "version": "1.0",
  "metadata": {
    "title": "Demo LED Show",
    "artist": "LED Master",
    "duration": 30,
    "bpm": 128,
    "audioFile": "demo.mp3"
  },
  "events": [
    {
      "time": 0,
      "type": "color",
      "params": {
        "r": 255,
        "g": 0,
        "b": 0
      }
    },
    {
      "time": 2000,
      "type": "effect",
      "params": {
        "name": "pulse",
        "r": 255,
        "g": 0,
        "b": 0,
        "speed": 5
      }
    },
    {
      "time": 7000,
      "type": "transition",
      "params": {
        "fromR": 255,
        "fromG": 0,
        "fromB": 0,
        "toR": 0,
        "toG": 255,
        "toB": 0,
        "duration": 2000
      }
    },
    {
      "time": 9500,
      "type": "effect",
      "params": {
        "name": "theater",
        "r": 0,
        "g": 255,
        "b": 0,
        "speed": 50
      }
    },
    {
      "time": 15000,
      "type": "effect",
      "params": {
        "name": "rainbow",
        "speed": 20
      }
    },
    {
      "time": 25000,
      "type": "brightness",
      "params": {
        "level": 100,
        "duration": 3000
      }
    },
    {
      "time": 28000,
      "type": "color",
      "params": {
        "r": 0,
        "g": 0,
        "b": 0
      }
    }
  ]
}
```

---

## Uso en la Interfaz Web

### 1. Cargar Timeline

```html
<input type="file" id="timelineFile" accept=".json">
<button onclick="loadTimeline()">Cargar Timeline</button>
```

### 2. Reproducir

```javascript
function playTimeline() {
    // Inicia reproducción de MP3 + ejecución de timeline
}
```

---

## Herramientas para Crear Timelines

### Opción 1: Editor Manual
- Editar JSON directamente en editor de texto
- Usa Audacity para medir tiempos en la canción

### Opción 2: Timeline Editor (Futuro)
- Interfaz visual arrastra y suelta
- Visualización de onda de audio
- Sincronización en tiempo real

### Opción 3: Script Python (Futuro)
```python
from capellan_led import Timeline

timeline = Timeline("cancion.mp3")
timeline.add_color(0, 255, 0, 0)        # Rojo en t=0
timeline.add_effect(2000, "pulse", ...)  # Pulso en t=2s
timeline.save("timeline.json")
```

---

## Sincronización

### Cálculo de Tiempos

Para una canción a 120 BPM:
- 1 beat = 500ms
- 1 bar (4 beats) = 2000ms

**Fórmula**:
```
tiempo_ms = (beat_number * 60000) / BPM
```

**Ejemplo**:
- BPM = 128
- Quiero efecto en beat 16
- tiempo_ms = (16 * 60000) / 128 = 7500ms = 7.5s

---

## Limitaciones Actuales

- ⏳ Máximo 100 eventos por timeline
- ⏳ Duración máxima 10 minutos
- ⏳ No hay loop automático
- ⏳ No hay vista previa
- ⏳ Sin editor visual

---

## Roadmap

- [ ] v0.2: Timeline Editor visual
- [ ] v0.3: Soporte para múltiples pistas
- [ ] v0.4: Sincronización de audio en tiempo real
- [ ] v0.5: Exportación a video
- [ ] v1.0: Soportar 1000+ eventos

---

## Ejemplos de Uso

### Festival / Performance
```
- 0s: Blanco brillante (introducción)
- 8s: Rojo pulsante (verso)
- 16s: Transición a azul (pre-coro)
- 20s: Efecto arcoíris (coro)
- 28s: Efecto teatro blanco (puente)
- 35s: Rojo intenso (final)
```

### DJ Set
```
- Cambio de color en cada drop
- Pulso sincronizado con beat
- Transiciones suaves entre canciones
- Efectos especiales en momentos clave
```

### Flow Arts Performance
```
- Colores coordinados con movimientos
- Brillo que responde a la energía
- Patrones que coinciden con el ritmo
```

---

**Última actualización**: 2026-06-11

**Estado**: En Desarrollo - Esperando implementación

# 📡 API de Comandos Serial - Capellán LED

## Protocolo de Comunicación

### Formato General
```
COMANDO|PARAM1|PARAM2|...\n
```

- **Delimitador**: Pipe `|`
- **Terminador**: Newline `\n` (LF)
- **Velocidad**: 115200 baud
- **Timeout**: 1000ms

### Respuestas

**Éxito**:
```
OK|[Descripción]
```

**Error**:
```
ERROR|[Descripción del error]
```

---

## Comandos Disponibles

### 1. COLOR - Cambiar Color

**Sintaxis**:
```
COLOR|R|G|B
```

**Parámetros**:
- `R` (0-255): Intensidad de rojo
- `G` (0-255): Intensidad de verde
- `B` (0-255): Intensidad de azul

**Ejemplos**:
```
COLOR|255|0|0       → Rojo puro
COLOR|0|255|0       → Verde puro
COLOR|0|0|255       → Azul puro
COLOR|255|255|0     → Amarillo
COLOR|255|0|255     → Magenta
COLOR|0|255|255     → Cyan
COLOR|255|255|255   → Blanco
COLOR|0|0|0         → Apagado
COLOR|128|128|128   → Gris
```

**Respuesta**:
```
OK|Color changed
```

---

### 2. BRIGHTNESS - Ajustar Brillo

**Sintaxis**:
```
BRIGHTNESS|LEVEL
```

**Parámetros**:
- `LEVEL` (0-255): Nivel de brillo

**Ejemplos**:
```
BRIGHTNESS|255  → Brillo máximo
BRIGHTNESS|128  → Brillo medio
BRIGHTNESS|50   → Brillo bajo
BRIGHTNESS|0    → Apagado (sin consumo)
```

**Respuesta**:
```
OK|Brightness updated
```

**Nota**: El brillo se aplica a todos los LEDs activos. No apaga los LEDs, solo reduce intensidad.

---

### 3. RAINBOW - Efecto Arcoíris

**Sintaxis**:
```
RAINBOW|SPEED
```

**Parámetros**:
- `SPEED` (1-100): Velocidad del efecto
  - 1-20: Muy lento
  - 20-50: Normal
  - 50-100: Muy rápido

**Ejemplos**:
```
RAINBOW|10   → Arcoíris lento
RAINBOW|20   → Arcoíris normal
RAINBOW|50   → Arcoíris rápido
```

**Respuesta**:
```
OK|Rainbow effect
```

**Descripción**: Cicla a través de todos los colores del espectro HSV. El efecto dura aproximadamente 5 ciclos.

---

### 4. PULSE - Efecto Pulso

**Sintaxis**:
```
PULSE|R|G|B|SPEED
```

**Parámetros**:
- `R` (0-255): Intensidad de rojo
- `G` (0-255): Intensidad de verde
- `B` (0-255): Intensidad de azul
- `SPEED` (1-100): Velocidad de pulso

**Ejemplos**:
```
PULSE|255|0|0|5     → Pulso rojo lento
PULSE|0|255|0|10    → Pulso verde normal
PULSE|0|0|255|20    → Pulso azul rápido
PULSE|255|255|255|3 → Pulso blanco muy lento
```

**Respuesta**:
```
OK|Pulse effect
```

**Descripción**: El color especificado pulsa desde 0 a 255 de brillo y regresa. Ciclo: fade in → fade out.

---

### 5. THEATER - Efecto Teatro

**Sintaxis**:
```
THEATER|R|G|B|SPEED
```

**Parámetros**:
- `R` (0-255): Intensidad de rojo
- `G` (0-255): Intensidad de verde
- `B` (0-255): Intensidad de azul
- `SPEED` (10-100): Velocidad en ms por frame

**Ejemplos**:
```
THEATER|255|255|255|50   → Efecto teatro blanco
THEATER|255|0|0|30       → Efecto teatro rojo rápido
THEATER|0|255|255|100    → Efecto teatro cyan lento
```

**Respuesta**:
```
OK|Theater effect
```

**Descripción**: Efecto de "corredor de luces" estilo teatro antiguo. 3 patrones que avanzan.

---

### 6. OFF - Apagar Todos los LEDs

**Sintaxis**:
```
OFF
```

**Parámetros**: Ninguno

**Ejemplos**:
```
OFF
```

**Respuesta**:
```
OK|All LEDs off
```

**Descripción**: Apaga inmediatamente todos los LEDs. No espera a finalizar efecto.

---

### 7. STATUS - Obtener Estado Actual

**Sintaxis**:
```
STATUS
```

**Parámetros**: Ninguno

**Respuesta**:
```
OK|R:255|G:0|B:0|BRIGHTNESS:255|LEDS:60
```

**Contenido**:
- `R`: Valor rojo actual (0-255)
- `G`: Valor verde actual (0-255)
- `B`: Valor azul actual (0-255)
- `BRIGHTNESS`: Brillo actual (0-255)
- `LEDS`: Cantidad de LEDs configurados

---

### 8. HELP - Mostrar Ayuda

**Sintaxis**:
```
HELP
```

**Parámetros**: Ninguno

**Respuesta**:
```
COMMANDS:
  COLOR|R|G|B - Set color (0-255 each)
  BRIGHTNESS|0-255 - Set brightness
  RAINBOW|speed - Rainbow effect
  PULSE|R|G|B|speed - Pulse effect
  THEATER|R|G|B|speed - Theater effect
  OFF - Turn all off
  STATUS - Get current status
```

---

## Ejemplos de Uso

### Python
```python
import serial
import time

# Conectar al puerto serial
ser = serial.Serial('COM3', 115200, timeout=1)
time.sleep(2)  # Esperar a que se establezca conexión

# Cambiar a rojo
ser.write(b'COLOR|255|0|0\n')
print(ser.readline().decode())  # OK|Color changed

# Establecer brillo
ser.write(b'BRIGHTNESS|200\n')
print(ser.readline().decode())  # OK|Brightness updated

# Efecto arcoíris
ser.write(b'RAINBOW|20\n')
print(ser.readline().decode())  # OK|Rainbow effect
time.sleep(5)  # Esperar a que termine

# Obtener estado
ser.write(b'STATUS\n')
print(ser.readline().decode())  # OK|R:...|G:...|B:...|...

# Cerrar
ser.close()
```

### JavaScript (Web Serial API)
```javascript
// Ver web-app/js/app.js para implementación completa

async function setRedColor() {
    await writer.write(new TextEncoder().encode('COLOR|255|0|0\n'));
}

async function pulseBlue() {
    await writer.write(new TextEncoder().encode('PULSE|0|0|255|5\n'));
}
```

### cURL (si hubiera interfaz REST, para referencia futura)
```bash
# Estos son ejemplos de cómo se vería si tuviera API REST
curl -X POST http://localhost:3000/color -d '{"r":255,"g":0,"b":0}'
curl -X POST http://localhost:3000/brightness -d '{"level":200}'
curl -X GET http://localhost:3000/status
```

---

## Notas Importantes

### Velocidad de Transmisión
- Asegurar 115200 baud en todos los clientes
- El dispositivo no responde a otras velocidades

### Timing
- Cada comando se procesa en ~10-50ms
- Los efectos son bloqueantes (no se pueden enviar comandos durante un efecto)
- Para cancelar un efecto, usa `OFF`

### Límites
- Máximo 60 LEDs (configurable, afecta RAM)
- Color máximo: (255, 255, 255) - White
- Consumo máximo: ~3.6A a 5V con todos los LEDs en blanco

### Extensibilidad
Para agregar nuevos comandos, edita la función `processCommand()` en `firmware/capellan-led.ino`

---

## Comandos Futuros (Planificado)

```
PATTERN|NAME|PARAM1|PARAM2   → Patrones personalizados
TIMELINE|LOAD|FILE           → Cargar timeline MP3
TIMELINE|PLAY                → Reproducir timeline
TIMELINE|PAUSE               → Pausar timeline
ANIMATION|ADD|FRAMES         → Agregar animación
```

---

**Última actualización**: 2026-06-11

**Versión del Firmware**: 1.0.0

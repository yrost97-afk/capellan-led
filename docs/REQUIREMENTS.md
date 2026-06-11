# Capellán LED - Librerías Requeridas

## Hardware

- **ESP8266 D1 Mini**: Microcontrolador
- **LEDs WS2812 (NeoPixel)**: Tira de LEDs RGB direccionables
- **Cable USB Micro**: Para programación y comunicación
- **Resistencia 470Ω**: Protección de pin de datos
- **Condensador 1000µF**: Estabilidad de alimentación

## Software Arduino

```
Adafruit NeoPixel >= 1.11.0
ESP8266WiFi (incluido en core de ESP8266)
```

## Instalación en Arduino IDE

1. **Instalar Board Support Package para ESP8266**:
   - Archivo → Preferencias
   - URLs adicionales de Gestor de tarjetas: `http://arduino.esp8266.com/stable/package_esp8266com_index.json`
   - Herramientas → Placa → Gestor de placas → Instalar "esp8266"

2. **Instalar Librería Adafruit NeoPixel**:
   - Herramientas → Gestionar librerías
   - Buscar "Adafruit NeoPixel"
   - Click en "Instalar"

## Cliente Python (Opcional)

```bash
pip install pyserial
```

Uso:
```python
import serial

ser = serial.Serial('COM3', 115200, timeout=1)
ser.write(b'COLOR|255|0|0\n')
print(ser.readline().decode())
```

## Cliente Web

- Navegador moderno con soporte Web Serial API (Chrome, Edge, Brave)
- Servidor local (opcional): `python -m http.server 8000`

---

**Versión**: 1.0.0
**Última actualización**: 2026-06-11

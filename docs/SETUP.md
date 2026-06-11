# 📖 Guía de Configuración - Capellán LED

## Requisitos Hardware

### ESP8266 D1 Mini
- **Microcontrolador**: ESP8266 (Tensilica L106 32-bit)
- **Memoria Flash**: 4MB
- **RAM**: 160KB
- **Voltaje**: 3.3V
- **Pines**: 11 (GPIO disponibles)

### LEDs WS2812 (NeoPixel)
- **Voltaje**: 5V
- **Protocolo**: One-Wire Digital
- **Consumo**: ~60mA por LED a máximo brillo (blanco)
- **Cantidad**: 60 LEDs recomendados (configurable)

### Componentes Adicionales
- Cable USB-A a Micro-USB (para ESP8266 D1)
- Resistencia 470Ω (protección pin de datos)
- Condensador 1000µF 6.3V (estabilidad de alimentación)
- Fuente 5V 10A mínimo (recomendado)

## Esquema de Conexión

```
ESP8266 D1 Mini          LEDs WS2812
    ┌────────────┐            ┌──────┐
    │   D4 (GPIO2)├─┬──────→  │DI    │
    │            │ │ 470Ω    │      │
    │   GND      ├─┴─────────→│GND   │
    │            │            │      │
    │            │            └──────┘
    │            │
    │   5V (USB) ├─────────────→ 5V (Fuente Externa)
    └────────────┘

Nota: Usar fuente externa 5V para LEDs
```

## Instalación del Firmware

### 1. Preparar Arduino IDE

```bash
# En Arduino IDE:
1. Ir a Archivo → Preferencias
2. URLs adicionales de Gestor de tarjetas:
   http://arduino.esp8266.com/stable/package_esp8266com_index.json

3. Ir a Herramientas → Placa → Gestor de placas
4. Buscar "ESP8266" e instalar
5. Herramientas → Placa → Seleccionar "WeMos D1 Mini"
```

### 2. Instalar Librerías

```bash
# En Arduino IDE: Herramientas → Gestionar librerías
- Buscar e instalar: Adafruit NeoPixel
- Buscar e instalar: ESP8266WiFi (incluida por defecto)
```

### 3. Cargar el Firmware

```bash
1. Abre: firmware/capellan-led.ino
2. Selecciona Herramientas → Puerto → COM[X]
3. Selecciona Herramientas → Velocidad (Baud): 115200
4. Clic en "Subir" o Ctrl+U
5. Espera a "Subida completada"
```

## Configuración de Software

### Variables Personalizables (capellan-led.ino)

```cpp
#define LED_PIN D4          // Pin para datos LED (cambiar si es necesario)
#define NUM_LEDS 60         // Cantidad de LEDs (cambiar según tu setup)
#define SERIAL_BAUD 115200  // Velocidad serial (no cambiar)
```

## Uso de la Interfaz Web

### 1. Acceder a la Interfaz

```bash
# Opción 1: Abrir directamente
- Abre: web-app/index.html en tu navegador

# Opción 2: Servir localmente
- python -m http.server 8000
- Abre: http://localhost:8000/web-app
```

### 2. Conectar el Dispositivo

1. Conecta ESP8266 D1 por USB
2. Abre web-app/index.html
3. Haz clic en "Conectar"
4. Selecciona el puerto COM de tu ESP8266
5. Haz clic en "Conectar"

### 3. Primeras Pruebas

```bash
# Test 1: Color Rojo
- Clic en botón "Rojo" bajo Colores Preestablecidos

# Test 2: Cambiar Brillo
- Usa el slider de Brillo
- Clic en "Aplicar"

# Test 3: Efecto Arcoíris
- Clic en botón "Arcoíris" bajo Efectos

# Test 4: Verificar Estado
- Clic en "Actualizar Estado"
- Verifica en consola serial
```

## Solución de Problemas

### El ESP8266 no se detecta

**Problema**: "No hay puertos disponibles"

**Soluciones**:
1. Instala drivers CH340 (https://github.com/MarlinFirmware/Marlin/wiki/Windows-Build-Guide#get-drivers)
2. Prueba otro cable USB (algunos son solo de carga)
3. Reinicia el navegador
4. Reconecta el dispositivo

### La conexión web serial falla

**Problema**: "Web Serial API no disponible"

**Soluciones**:
1. Usa navegador Chromium (Chrome, Edge, Opera, Brave)
2. Firefox aún no soporta Web Serial API
3. Safari no soporta Web Serial API (usa Chrome en Mac)

### Los LEDs no encienden

**Problema**: "Los LEDs no responden a comandos"

**Soluciones**:
1. Verifica conexión física del pin D4
2. Chequea la alimentación 5V de los LEDs
3. Intenta aumentar el valor de `NUM_LEDS` en el código
4. Abre Monitor Serial (115200 baud) y envía: `STATUS`

### Parpadeo o colores incorrectos

**Problema**: "Los LEDs parpadean o muestran colores errados"

**Soluciones**:
1. Añade la resistencia 470Ω entre D4 y DI
2. Verifica la polaridad de conexión
3. Aumenta el condensador 1000µF
4. Reduce el brillo si consume demasiada corriente

## Próximos Pasos

- [ ] Implementar Timeline MP3
- [ ] Agregar soporte para múltiples grupos de LEDs
- [ ] Crear presets guardables
- [ ] Añadir visualizador de audio
- [ ] Soporte para POIs y Staff

---

**Última actualización**: 2026-06-11

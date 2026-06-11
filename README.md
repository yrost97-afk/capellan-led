# 🎆 Capellan LED

Control software para LEDs WS2812 con timeline sincronizado a MP3. Perfecto para flow arts: trajes LED, pois, staff, levid wand y más.

## ✨ Características

- 🎨 **Control de colores** - Paleta amplia con presets
- ⏱️ **Timeline sincronizado** - Edita keyframes y sincroniza con MP3
- 🔌 **Control vía USB** - Conexión serial directa (sin IP)
- 📦 **Múltiples dispositivos** - Controla varios ESP8266/D1 simultáneamente
- 🛠️ **Escalable** - Agrega nuevos tipos de dispositivos fácilmente
- 🎯 **WS2812 Neopixel** - Soporte completo para tiras LED inteligentes

## 🏗️ Estructura del Proyecto

```
capellan-led/
├── firmware/                 # Código ESP8266/D1
│   ├── src/
│   │   ├── main.cpp         # Punto de entrada
│   │   ├── led_controller.h # Control de LEDs
│   │   └── usb_protocol.h   # Protocolo serial
│   └── platformio.ini       # Configuración PlatformIO
├── app/                     # Aplicación Desktop (Electron + React)
│   ├── src/
│   │   ├── components/      # Componentes React
│   │   ├── pages/           # Páginas principales
│   │   ├── services/        # Servicios (USB, audio)
│   │   └── App.jsx
│   ├── public/
│   └── package.json
├── docs/                    # Documentación
│   ├── SETUP.md
│   ├── PROTOCOL.md
│   └── EXTENSIONS.md
└── package.json             # Root package
```

## 🚀 Quick Start

### Hardware
- ESP8266 D1 Mini
- Tira WS2812B Neopixel
- Cable USB

### Software
1. Instala [PlatformIO](https://platformio.org/)
2. Compila y carga el firmware
3. Abre la aplicación desktop
4. Conecta el dispositivo USB
5. ¡Empieza a crear!

## 📱 Dispositivos Soportados

- ✅ Trajes LED
- ✅ Pois
- ✅ Staff
- ✅ Levid Wand
- 🔄 Más dispositivos (extensible)

## 🔧 Tecnología

- **Firmware**: Arduino C++ + PlatformIO
- **App**: Electron + React
- **Audio**: Web Audio API
- **Comunicación**: Serial/USB

## 📚 Documentación

- [Setup Guide](./docs/SETUP.md) - Instalación y configuración
- [Protocol](./docs/PROTOCOL.md) - Protocolo USB/Serial
- [Extensions](./docs/EXTENSIONS.md) - Agregar nuevos dispositivos

## 📄 Licencia

MIT - Libre para usar y modificar

## 🤝 Contribuciones

¡Las contribuciones son bienvenidas! Abre un issue o PR.

---

**Capellan LED** - Controla la luz, crea magia ✨

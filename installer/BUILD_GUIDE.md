# 🎆 Compilación de Instalador Capellán LED

## Requisitos

```bash
# Windows, macOS, Linux
Node.js >= 14.0.0
npm >= 6.0.0
git
```

## Pasos para Compilar

### 1. Preparar ambiente
```bash
cd installer
npm install
```

### 2. Compilar versión de desarrollo
```bash
npm run electron:dev
```
Esto abrirá la app con el servidor de desarrollo.

### 3. Compilar para producción

#### a) Instalador NSIS (recomendado)
```bash
npm run build:installer
```
Crea: `dist/Capellan-LED-Setup-1.0.0.exe` (~150MB)

#### b) Versión portátil
```bash
npm run build:portable
```
Crea: `dist/Capellan-LED-1.0.0-portable.exe` (~180MB)

#### c) Ambas versiones
```bash
npm run build:exe
```

---

## Estructura del Proyecto

```
installer/
├── public/
│   └── index.html           # HTML entrada
├── src/
│   ├── index.js             # React entrada
│   ├── App.js               # Componente principal
│   └── App.css              # Estilos
├── main.js                  # Proceso principal Electron
├── preload.js               # Puente de seguridad
├── package.json             # Configuración npm/Electron
└── README.md                # Este archivo
```

---

## Características del Instalador

✅ **Instalador NSIS**
- Interfaz gráfica profesional
- Instala en Program Files
- Crea accesos directos en Desktop y Menú Inicio
- Desinstala limpio

✅ **Versión Portátil**
- Sin instalación
- Ejecutable directamente
- Perfecto para USB

✅ **App Integrada**
- Editor de timelines visuales
- Gestor de firmware
- Monitor serial
- Carga/descarga de proyectos

---

## Personalización

### Cambiar icono
Reemplaza `assets/icon.png` (256x256)

### Cambiar nombre
Edita `package.json` → `"productName": "Capellán LED"`

### Cambiar versión
Edita `package.json` → `"version": "1.0.0"`

---

## Distribución

### Compartir instalador
```bash
# Encontrar en:
dist/Capellan-LED-Setup-1.0.0.exe
dist/Capellan-LED-1.0.0-portable.exe
```

### Upload a GitHub Releases
```bash
gh release create v1.0.0 dist/*.exe
```

---

## Troubleshooting

### Error: "electron not found"
```bash
npm install --save-dev electron
```

### Error: "electron-builder not found"
```bash
npm install --save-dev electron-builder
```

### El app no inicia
```bash
npm run electron:dev
# Ver errores en consola
```

### Icono no aparece
```bash
# Asegúrate que existe:
assets/icon.png (256x256)
```

---

## Desarrollo Futuro

- [ ] Actualización automática
- [ ] Sincronización en la nube
- [ ] Comunidad de timelines
- [ ] Plugins personalizados
- [ ] Versiones para macOS y Linux

---

**Capellán LED Installer v1.0.0**
Built with Electron + React

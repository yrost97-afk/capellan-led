# 🤝 Contribuir a Capellán LED

¡Gracias por tu interés en contribuir a Capellán LED! Este documento te ayudará a entender cómo hacerlo.

## Tipos de Contribuciones

### 🐛 Reportar Bugs
1. Abre una **Issue** describiendo el problema
2. Incluye:
   - Sistema operativo y navegador
   - Pasos para reproducir
   - Resultado esperado vs. actual
   - Capturas de pantalla si es posible

### ✨ Sugerir Características
1. Abre una **Issue** con la etiqueta `enhancement`
2. Describe el caso de uso
3. Proporciona ejemplos

### 🔧 Enviar Código

#### Paso 1: Fork y Clone
```bash
git clone https://github.com/yrost97-afk/capellan-led.git
cd capellan-led
```

#### Paso 2: Crear Rama
```bash
git checkout -b feature/tu-caracteristica
# o
git checkout -b fix/tu-bug
```

#### Paso 3: Hacer Cambios
- Edita los archivos necesarios
- Sigue las convenciones de código
- Añade comentarios si es complejo

#### Paso 4: Testear
```bash
# Para firmware: Carga en Arduino IDE y verifica
# Para web: Abre en navegador y prueba

python examples/client_python.py  # Si aplica
```

#### Paso 5: Commit
```bash
git add .
git commit -m "descripción clara del cambio"
```

#### Paso 6: Push y Pull Request
```bash
git push origin feature/tu-caracteristica
```
1. Ve a GitHub y abre un Pull Request
2. Describe qué cambiaste y por qué

## Estándares de Código

### Arduino/C++
```cpp
// Nombres de variables: camelCase
int ledPin = D4;

// Nombres de funciones: camelCase
void setColor(uint8_t r, uint8_t g, uint8_t b) {
    // Implementación
}

// Comentarios para funciones complejas
// Evita código sin documentar
```

### JavaScript
```javascript
// Nombres: camelCase
const colorPicker = document.getElementById("colorPicker");

// Funciones: camelCase
function updateBrightnessValue() {
    // Implementación
}

// Usa const por defecto, let si necesitas reasignar
```

### Python
```python
# Sigue PEP 8
# Nombres: snake_case
def set_color(r, g, b):
    """Descripción de la función"""
    # Implementación
```

## Áreas de Contribución

### 🎯 Alta Prioridad
- [ ] Timeline MP3 (implementación)
- [ ] Editor visual de timeline
- [ ] Interfaz mejorada
- [ ] Más patrones de efectos

### 📊 Media Prioridad
- [ ] Soporte para múltiples grupos de LEDs
- [ ] Presets guardables
- [ ] Visualizador de audio
- [ ] Documentación mejorada

### 🔮 Futuro
- [ ] Soporte para POIs
- [ ] Soporte para LED Staff
- [ ] Soporte para Levid Wand
- [ ] App de escritorio (Electron)

## Proceso de Review

1. Un maintainer revisará tu PR
2. Pueden pedir cambios
3. Una vez aprobado, se hará merge
4. ¡Tu nombre irá en la lista de contribuidores!

## Licencia

Al contribuir, aceptas que tu código se licencie bajo MIT.

## Preguntas

- **Issues**: Para bugs y características
- **Discussions**: Para preguntas generales
- **Email**: yrost97@gmail.com

---

¡Gracias por hacer que Capellán LED sea mejor! 🌟

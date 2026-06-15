#!/usr/bin/env python3
"""
Capellán LED - Cliente Python Avanzado v2
Control de LEDs con Timeline, nuevos efectos y múltiples dispositivos
"""

import serial
import time
import json
import sys
from pathlib import Path
from typing import List, Dict, Optional

class CapellanLED:
    """Cliente para Capellán LED firmware v2"""
    
    def __init__(self, port='COM3', baudrate=115200, timeout=1):
        """Inicializar conexión serial"""
        try:
            self.ser = serial.Serial(port, baudrate, timeout=timeout)
            time.sleep(2)
            
            # Obtener info del dispositivo
            self.ser.write(b'STATUS\n')
            response = self.ser.readline().decode().strip()
            print(f"✓ Conectado a {port}")
            print(f"  Respuesta: {response}")
            
            self.device_info = response
        except serial.SerialException as e:
            print(f"✗ Error de conexión: {e}")
            self.ser = None
    
    def send_command(self, command: str) -> str:
        """Enviar comando y obtener respuesta"""
        if not self.ser:
            print("✗ No conectado")
            return None
        
        try:
            self.ser.write((command + '\n').encode())
            response = self.ser.readline().decode().strip()
            print(f"→ {command}")
            print(f"← {response}\n")
            return response
        except Exception as e:
            print(f"✗ Error: {e}")
            return None
    
    # ==================== COMANDOS CLÁSICOS ====================
    
    def set_color(self, r: int, g: int, b: int) -> str:
        """Cambiar color"""
        return self.send_command(f"COLOR|{r}|{g}|{b}")
    
    def set_brightness(self, level: int) -> str:
        """Ajustar brillo (0-255)"""
        return self.send_command(f"BRIGHTNESS|{level}")
    
    def rainbow(self, speed: int = 20) -> str:
        """Efecto arcoíris clásico"""
        return self.send_command(f"RAINBOW|{speed}")
    
    def pulse(self, r: int, g: int, b: int, speed: int = 5) -> str:
        """Efecto pulso clásico"""
        return self.send_command(f"PULSE|{r}|{g}|{b}|{speed}")
    
    def theater(self, r: int, g: int, b: int, speed: int = 50) -> str:
        """Efecto teatro clásico"""
        return self.send_command(f"THEATER|{r}|{g}|{b}|{speed}")
    
    # ==================== NUEVOS EFECTOS ====================
    
    def strobe(self, r: int, g: int, b: int, speed: int = 100, duration: int = 2000) -> str:
        """Efecto estrobo rápido"""
        return self.send_command(f"STROBE|{r}|{g}|{b}|{speed}|{duration}")
    
    def chase(self, r: int, g: int, b: int, speed: int = 50, duration: int = 5000) -> str:
        """Efecto persecución de LEDs"""
        return self.send_command(f"CHASE|{r}|{g}|{b}|{speed}|{duration}")
    
    def fire(self, speed: int = 50, duration: int = 5000) -> str:
        """Efecto fuego realista"""
        return self.send_command(f"FIRE|{speed}|{duration}")
    
    def rainbow_cycle(self, speed: int = 50, duration: int = 10000) -> str:
        """Ciclo de arcoíris continuo"""
        return self.send_command(f"RAINBOW_CYCLE|{speed}|{duration}")
    
    def breathe(self, r: int, g: int, b: int, speed: int = 5, duration: int = 5000) -> str:
        """Efecto respiración suave"""
        return self.send_command(f"BREATHE|{r}|{g}|{b}|{speed}|{duration}")
    
    def sparkle(self, r: int, g: int, b: int, speed: int = 100, duration: int = 5000) -> str:
        """Efecto destellos aleatorios"""
        return self.send_command(f"SPARKLE|{r}|{g}|{b}|{speed}|{duration}")
    
    # ==================== TIMELINE ====================
    
    def upload_timeline(self, timeline: Dict) -> str:
        """Subir timeline en formato JSON"""
        json_str = json.dumps(timeline)
        
        # Limitar tamaño si es necesario
        if len(json_str) > 8000:
            print("⚠ Timeline muy grande, puede fallar en ESP8266")
        
        return self.send_command(f"UPLOAD_TIMELINE|{json_str}")
    
    def load_timeline_from_file(self, filepath: str) -> str:
        """Cargar timeline desde archivo JSON"""
        try:
            with open(filepath, 'r') as f:
                timeline = json.load(f)
            return self.upload_timeline(timeline)
        except FileNotFoundError:
            print(f"✗ Archivo no encontrado: {filepath}")
            return None
    
    def play_timeline(self) -> str:
        """Reproducir timeline cargado"""
        return self.send_command("TIMELINE|PLAY")
    
    def load_timeline_from_device(self) -> str:
        """Cargar timeline desde almacenamiento del dispositivo"""
        return self.send_command("TIMELINE|LOAD")
    
    def delete_timeline(self) -> str:
        """Eliminar timeline del almacenamiento"""
        return self.send_command("TIMELINE|DELETE")
    
    def timeline_status(self) -> str:
        """Ver estado del timeline"""
        return self.send_command("TIMELINE|STATUS")
    
    def list_files(self) -> str:
        """Listar archivos en el dispositivo"""
        return self.send_command("TIMELINE|LIST")
    
    # ==================== SISTEMA ====================
    
    def turn_off(self) -> str:
        """Apagar todos los LEDs"""
        return self.send_command("OFF")
    
    def get_status(self) -> str:
        """Obtener estado completo del dispositivo"""
        return self.send_command("STATUS")
    
    def save_config(self) -> str:
        """Guardar configuración actual"""
        return self.send_command("CONFIG|SAVE")
    
    def load_config(self) -> str:
        """Cargar configuración guardada"""
        return self.send_command("CONFIG|LOAD")
    
    def help(self) -> str:
        """Mostrar ayuda"""
        return self.send_command("HELP")
    
    def close(self):
        """Cerrar conexión"""
        if self.ser:
            self.ser.close()
            print("✓ Conexión cerrada")
    
    # ==================== UTILIDADES ====================
    
    def create_timeline(self, title: str, bpm: int = 128) -> Dict:
        """Crear estructura base de timeline"""
        return {
            "version": "1.0",
            "metadata": {
                "title": title,
                "bpm": bpm,
                "duration": 0
            },
            "events": []
        }
    
    def add_color_event(self, timeline: Dict, time_ms: int, r: int, g: int, b: int):
        """Agregar evento de color al timeline"""
        timeline["events"].append({
            "time": time_ms,
            "type": "color",
            "r": r,
            "g": g,
            "b": b
        })
    
    def add_effect_event(self, timeline: Dict, time_ms: int, effect: str, 
                        duration: int, **kwargs):
        """Agregar evento de efecto al timeline"""
        event = {
            "time": time_ms,
            "type": "effect",
            "effect": effect,
            "duration": duration
        }
        event.update(kwargs)
        timeline["events"].append(event)
    
    def add_brightness_event(self, timeline: Dict, time_ms: int, level: int):
        """Agregar evento de brillo al timeline"""
        timeline["events"].append({
            "time": time_ms,
            "type": "brightness",
            "level": level
        })


def demo_basic_effects():
    """Demo de efectos básicos"""
    print("\n" + "="*50)
    print("DEMO: Efectos Básicos")
    print("="*50 + "\n")
    
    led = CapellanLED()
    
    if not led.ser:
        return
    
    try:
        # Color rojo
        print("--- Test 1: Color Rojo ---")
        led.set_color(255, 0, 0)
        time.sleep(2)
        
        # Color azul
        print("\n--- Test 2: Color Azul ---")
        led.set_color(0, 0, 255)
        time.sleep(2)
        
        # Brillo reducido
        print("\n--- Test 3: Brillo Reducido ---")
        led.set_brightness(100)
        time.sleep(1)
        
        # Brillo normal
        print("\n--- Test 4: Brillo Normal ---")
        led.set_brightness(255)
        time.sleep(1)
        
        # Apagar
        print("\n--- Test 5: Apagar ---")
        led.turn_off()
        
    except KeyboardInterrupt:
        print("\n\n⏹ Cancelado por usuario")
    finally:
        led.close()


def demo_new_effects():
    """Demo de nuevos efectos"""
    print("\n" + "="*50)
    print("DEMO: Nuevos Efectos v2")
    print("="*50 + "\n")
    
    led = CapellanLED()
    
    if not led.ser:
        return
    
    try:
        # Strobe
        print("--- Efecto 1: Strobe Blanco ---")
        led.strobe(255, 255, 255, 100, 3000)
        time.sleep(1)
        
        # Chase
        print("\n--- Efecto 2: Chase Verde ---")
        led.chase(0, 255, 0, 50, 5000)
        time.sleep(1)
        
        # Fire
        print("\n--- Efecto 3: Fuego ---")
        led.fire(30, 3000)
        time.sleep(1)
        
        # Rainbow Cycle
        print("\n--- Efecto 4: Rainbow Cycle ---")
        led.rainbow_cycle(50, 5000)
        time.sleep(1)
        
        # Breathe
        print("\n--- Efecto 5: Respiración ---")
        led.breathe(128, 0, 255, 5, 4000)
        time.sleep(1)
        
        # Sparkle
        print("\n--- Efecto 6: Destellos ---")
        led.sparkle(255, 215, 0, 100, 3000)
        time.sleep(1)
        
        # Apagar
        led.turn_off()
        
    except KeyboardInterrupt:
        print("\n\n⏹ Cancelado por usuario")
    finally:
        led.close()


def demo_timeline():
    """Demo con Timeline"""
    print("\n" + "="*50)
    print("DEMO: Timeline")
    print("="*50 + "\n")
    
    led = CapellanLED()
    
    if not led.ser:
        return
    
    try:
        # Crear timeline
        print("--- Creando Timeline ---")
        timeline = led.create_timeline("Python Demo", bpm=128)
        
        led.add_color_event(timeline, 0, 255, 0, 0)  # Rojo
        led.add_effect_event(timeline, 2000, "strobe", 2000, 
                           r=255, g=255, b=255, speed=100)
        led.add_effect_event(timeline, 5000, "chase", 5000,
                           r=0, g=255, b=0, speed=50)
        led.add_effect_event(timeline, 11000, "fire", 3000, speed=30)
        led.add_brightness_event(timeline, 15000, 128)
        led.add_effect_event(timeline, 17000, "sparkle", 5000,
                           r=255, g=215, b=0, speed=100)
        led.add_color_event(timeline, 23000, 0, 0, 0)  # Apagar
        
        timeline["metadata"]["duration"] = 24000
        
        # Mostrar timeline
        print("Timeline creado con {} eventos".format(len(timeline["events"])))
        print(json.dumps(timeline, indent=2))
        
        # Subir timeline
        print("\n--- Subiendo Timeline ---")
        led.upload_timeline(timeline)
        time.sleep(1)
        
        # Ver estado
        print("\n--- Estado del Timeline ---")
        led.timeline_status()
        time.sleep(1)
        
        # Reproducir
        print("\n--- Reproduciendo Timeline ---")
        led.play_timeline()
        
        # Esperar a que termine
        time.sleep(25)
        
        # Ver estado final
        print("\n--- Timeline Completado ---")
        led.timeline_status()
        
    except KeyboardInterrupt:
        print("\n\n⏹ Cancelado por usuario")
    finally:
        led.close()


def main():
    """Menú principal"""
    print("""
    ╔════════════════════════════════════════╗
    ║   Capellán LED - Cliente Python v2    ║
    ║   Advanced Effects & Timeline Demo    ║
    ╚════════════════════════════════════════╝
    """)
    
    if len(sys.argv) > 1:
        demo = sys.argv[1].lower()
        
        if demo == "basic":
            demo_basic_effects()
        elif demo == "effects":
            demo_new_effects()
        elif demo == "timeline":
            demo_timeline()
        else:
            print(f"✗ Demo desconocido: {demo}")
            print("Usos disponibles: basic, effects, timeline")
    else:
        print("Usos disponibles:")
        print("  python advanced_timeline.py basic     # Efectos básicos")
        print("  python advanced_timeline.py effects   # Nuevos efectos")
        print("  python advanced_timeline.py timeline  # Sistema Timeline")
        print("\nEjemplo:")
        print("  python advanced_timeline.py effects")


if __name__ == "__main__":
    main()

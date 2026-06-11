#!/usr/bin/env python3
"""
Capellán LED - Cliente Python
Control de LEDs WS2812 via ESP8266 D1
"""

import serial
import time
import json
import sys
from pathlib import Path

class CapellanLED:
    def __init__(self, port='COM3', baudrate=115200, timeout=1):
        """Inicializar conexión serial"""
        try:
            self.ser = serial.Serial(port, baudrate, timeout=timeout)
            time.sleep(2)  # Esperar a que se estabilice
            print(f"✓ Conectado a {port} ({baudrate} baud)")
        except serial.SerialException as e:
            print(f"✗ Error de conexión: {e}")
            self.ser = None
    
    def send_command(self, command):
        """Enviar comando y obtener respuesta"""
        if not self.ser:
            print("✗ No conectado")
            return None
        
        try:
            self.ser.write((command + '\n').encode())
            response = self.ser.readline().decode().strip()
            print(f"→ {command}")
            print(f"← {response}")
            return response
        except Exception as e:
            print(f"✗ Error: {e}")
            return None
    
    def set_color(self, r, g, b):
        """Cambiar color"""
        return self.send_command(f"COLOR|{r}|{g}|{b}")
    
    def set_brightness(self, level):
        """Ajustar brillo (0-255)"""
        return self.send_command(f"BRIGHTNESS|{level}")
    
    def rainbow(self, speed=20):
        """Efecto arcoíris"""
        return self.send_command(f"RAINBOW|{speed}")
    
    def pulse(self, r, g, b, speed=5):
        """Efecto pulso"""
        return self.send_command(f"PULSE|{r}|{g}|{b}|{speed}")
    
    def theater(self, r, g, b, speed=50):
        """Efecto teatro"""
        return self.send_command(f"THEATER|{r}|{g}|{b}|{speed}")
    
    def turn_off(self):
        """Apagar todos los LEDs"""
        return self.send_command("OFF")
    
    def get_status(self):
        """Obtener estado actual"""
        return self.send_command("STATUS")
    
    def help(self):
        """Mostrar ayuda"""
        return self.send_command("HELP")
    
    def close(self):
        """Cerrar conexión"""
        if self.ser:
            self.ser.close()
            print("✓ Conexión cerrada")


def main():
    """Ejemplos de uso"""
    
    # Detectar puerto automáticamente
    print("🔍 Detectando puertos seriales disponibles...")
    ports = [p.device for p in serial.tools.list_ports.comports()]
    
    if not ports:
        print("✗ No se encontraron puertos seriales")
        return
    
    print(f"Puertos disponibles: {ports}")
    port = ports[0]
    
    # Conectar
    led = CapellanLED(port)
    
    if not led.ser:
        return
    
    try:
        # Test 1: Color rojo
        print("\n--- Test 1: Color Rojo ---")
        led.set_color(255, 0, 0)
        time.sleep(2)
        
        # Test 2: Brillo
        print("\n--- Test 2: Ajustar Brillo ---")
        led.set_brightness(150)
        time.sleep(1)
        
        # Test 3: Efecto pulso
        print("\n--- Test 3: Efecto Pulso Rojo ---")
        led.pulse(255, 0, 0, 5)
        time.sleep(6)
        
        # Test 4: Color verde
        print("\n--- Test 4: Color Verde ---")
        led.set_color(0, 255, 0)
        time.sleep(2)
        
        # Test 5: Efecto teatro
        print("\n--- Test 5: Efecto Teatro ---")
        led.theater(0, 255, 0, 50)
        time.sleep(6)
        
        # Test 6: Color azul
        print("\n--- Test 6: Color Azul ---")
        led.set_color(0, 0, 255)
        time.sleep(2)
        
        # Test 7: Efecto arcoíris
        print("\n--- Test 7: Efecto Arcoíris ---")
        led.rainbow(20)
        time.sleep(10)
        
        # Test 8: Estado
        print("\n--- Test 8: Estado ---")
        led.get_status()
        
        # Apagar
        print("\n--- Apagando ---")
        led.turn_off()
        
    except KeyboardInterrupt:
        print("\n\n⏹ Cancelado por usuario")
    finally:
        led.close()


if __name__ == "__main__":
    main()

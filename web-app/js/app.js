// ==================== CAPELLÁN LED - APP.JS ====================

// Variables globales
let port = null;
let reader = null;
let writer = null;
let isConnected = false;

// ==================== CONEXIÓN SERIAL ====================

async function connectSerial() {
    try {
        port = await navigator.serial.requestPort();
        await port.open({ baudRate: 115200 });
        
        reader = port.readable.getReader();
        writer = port.writable.getWriter();
        
        isConnected = true;
        updateConnectionStatus(true);
        logConsole("✓ Conectado al puerto serial");
        
        // Iniciar lectura
        readSerialData();
    } catch (error) {
        logConsole("✗ Error de conexión: " + error.message);
    }
}

async function disconnectSerial() {
    if (reader) {
        await reader.cancel();
    }
    if (writer) {
        await writer.close();
    }
    if (port) {
        await port.close();
    }
    
    isConnected = false;
    updateConnectionStatus(false);
    logConsole("✗ Desconectado");
}

async function readSerialData() {
    while (isConnected) {
        try {
            const { value, done } = await reader.read();
            
            if (done) break;
            
            const text = new TextDecoder().decode(value);
            logConsole(text);
        } catch (error) {
            if (error.name !== 'AbortError') {
                logConsole("Error: " + error.message);
            }
            break;
        }
    }
}

async function sendCommand(command) {
    if (!isConnected || !writer) {
        logConsole("✗ No conectado");
        return;
    }
    
    try {
        const msg = command + "\n";
        await writer.write(new TextEncoder().encode(msg));
        logConsole("→ " + command);
    } catch (error) {
        logConsole("✗ Error al enviar: " + error.message);
    }
}

function updateConnectionStatus(connected) {
    const status = document.getElementById("status");
    const connectBtn = document.getElementById("connectBtn");
    const disconnectBtn = document.getElementById("disconnectBtn");
    
    if (connected) {
        status.textContent = "✓ Conectado";
        status.className = "status connected";
        connectBtn.disabled = true;
        disconnectBtn.disabled = false;
    } else {
        status.textContent = "✗ Desconectado";
        status.className = "status disconnected";
        connectBtn.disabled = false;
        disconnectBtn.disabled = true;
    }
}

// ==================== CONTROL DE COLORES ====================

function setColor(r, g, b) {
    sendCommand(`COLOR|${r}|${g}|${b}`);
    document.getElementById("redSlider").value = r;
    document.getElementById("greenSlider").value = g;
    document.getElementById("blueSlider").value = b;
    updateRGBPreview();
}

function applyColorFromPicker() {
    const color = document.getElementById("colorPicker").value;
    const r = parseInt(color.slice(1, 3), 16);
    const g = parseInt(color.slice(3, 5), 16);
    const b = parseInt(color.slice(5, 7), 16);
    setColor(r, g, b);
}

function applyRGBSliders() {
    const r = parseInt(document.getElementById("redSlider").value);
    const g = parseInt(document.getElementById("greenSlider").value);
    const b = parseInt(document.getElementById("blueSlider").value);
    setColor(r, g, b);
}

function updateRGBPreview() {
    const r = document.getElementById("redSlider").value;
    const g = document.getElementById("greenSlider").value;
    const b = document.getElementById("blueSlider").value;
    
    document.getElementById("redValue").textContent = r;
    document.getElementById("greenValue").textContent = g;
    document.getElementById("blueValue").textContent = b;
}

// ==================== CONTROL DE BRILLO ====================

function updateBrightnessValue() {
    const brightness = document.getElementById("brightnessSlider").value;
    document.getElementById("brightnessValue").textContent = brightness;
}

function applyBrightness() {
    const brightness = document.getElementById("brightnessSlider").value;
    sendCommand(`BRIGHTNESS|${brightness}`);
}

// ==================== CONTROL DE EFECTOS ====================

function rainbow() {
    sendCommand("RAINBOW|20");
}

function pulseEffect(r, g, b) {
    sendCommand(`PULSE|${r}|${g}|${b}|5`);
}

function theaterEffect() {
    sendCommand("THEATER|255|255|255|50");
}

function turnOff() {
    sendCommand("OFF");
}

// ==================== ESTADO ====================

function getStatus() {
    sendCommand("STATUS");
}

// ==================== CONSOLA ====================

function logConsole(message) {
    const output = document.getElementById("consoleOutput");
    const timestamp = new Date().toLocaleTimeString();
    output.textContent += `[${timestamp}] ${message}\n`;
    output.parentElement.scrollTop = output.parentElement.scrollHeight;
}

function sendCustomCommand() {
    const input = document.getElementById("commandInput");
    const command = input.value.trim();
    
    if (command) {
        sendCommand(command);
        input.value = "";
    }
}

// ==================== TIMELINE (EN DESARROLLO) ====================

function loadTimeline() {
    logConsole("Timeline: En desarrollo...");
}

function playTimeline() {
    logConsole("Timeline: En desarrollo...");
}

// ==================== INICIALIZACIÓN ====================

document.addEventListener("DOMContentLoaded", function() {
    // Botones de conexión
    document.getElementById("connectBtn").addEventListener("click", connectSerial);
    document.getElementById("disconnectBtn").addEventListener("click", disconnectSerial);
    
    // Verificar si el navegador soporta Web Serial API
    if (!navigator.serial) {
        logConsole("✗ Web Serial API no disponible en este navegador");
        document.getElementById("connectBtn").disabled = true;
    }
    
    // Evento para enviar comando con Enter
    document.getElementById("commandInput").addEventListener("keypress", function(e) {
        if (e.key === "Enter") {
            sendCustomCommand();
        }
    });
    
    logConsole("Capellán LED - Listo");
});

// ==================== UTILIDADES ====================

// Exportar timeline a JSON
function exportTimeline() {
    // TODO: Implementar exportación de timeline
}

// Importar timeline desde JSON
function importTimeline() {
    // TODO: Implementar importación de timeline
}

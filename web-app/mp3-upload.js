// ==================== MP3 UPLOAD & TIMELINE MANAGER ====================

let port = null;
let reader = null;
let writer = null;
let currentMP3 = null;
let timelineData = [];
let isPlaying = false;
let playbackStartTime = 0;
let uploadSpeed = 0;

// ==================== INICIALIZACIÓN ====================

document.addEventListener('DOMContentLoaded', () => {
    setupEventListeners();
    setupSerialConnection();
});

function setupEventListeners() {
    // Upload area
    const uploadArea = document.getElementById('uploadArea');
    const mp3Input = document.getElementById('mp3Input');

    uploadArea.addEventListener('click', () => mp3Input.click());
    uploadArea.addEventListener('dragover', (e) => {
        e.preventDefault();
        uploadArea.classList.add('dragover');
    });
    uploadArea.addEventListener('dragleave', () => {
        uploadArea.classList.remove('dragover');
    });
    uploadArea.addEventListener('drop', (e) => {
        e.preventDefault();
        uploadArea.classList.remove('dragover');
        const files = e.dataTransfer.files;
        if (files.length > 0) {
            handleMP3Upload(files[0]);
        }
    });

    mp3Input.addEventListener('change', (e) => {
        if (e.target.files.length > 0) {
            handleMP3Upload(e.target.files[0]);
        }
    });

    // Controles de reproducción
    document.getElementById('playBtn').addEventListener('click', playMP3);
    document.getElementById('pauseBtn').addEventListener('click', pauseMP3);
    document.getElementById('stopBtn').addEventListener('click', stopMP3);

    // Timeline
    document.getElementById('addKeyframeBtn').addEventListener('click', addKeyframe);
    document.getElementById('saveTimelineBtn').addEventListener('click', saveTimeline);
    document.getElementById('loadTimelineBtn').addEventListener('click', loadTimeline);
    document.getElementById('clearTimelineBtn').addEventListener('click', clearTimeline);
}

// ==================== CONEXIÓN SERIAL ====================

async function setupSerialConnection() {
    try {
        const ports = await navigator.serial.getPorts();
        if (ports.length > 0) {
            await connectToPort(ports[0]);
        }
    } catch (error) {
        console.log('Serial API no disponible:', error.message);
        showNotification('info', 'Conecta un dispositivo USB primero');
    }
}

async function connectToPort(selectedPort) {
    try {
        port = selectedPort;
        await port.open({ baudRate: 115200 });
        reader = port.readable.getReader();
        writer = port.writable.getWriter();
        showNotification('success', '✓ Conectado al dispositivo');
        updateDeviceStatus(true);
    } catch (error) {
        showNotification('error', '✗ Error de conexión: ' + error.message);
    }
}

async function sendCommand(command) {
    if (!writer) {
        showNotification('error', 'No conectado al dispositivo');
        return null;
    }

    try {
        const encoder = new TextEncoder();
        await writer.write(encoder.encode(command + '\n'));

        // Leer respuesta
        const response = await readSerialResponse();
        return response;
    } catch (error) {
        showNotification('error', 'Error enviar comando: ' + error.message);
        return null;
    }
}

async function readSerialResponse() {
    if (!reader) return null;

    try {
        const { value, done } = await reader.read();
        if (done) return null;

        const decoder = new TextDecoder();
        return decoder.decode(value).trim();
    } catch (error) {
        return null;
    }
}

// ==================== CARGA DE MP3 ====================

async function handleMP3Upload(file) {
    // Validar
    if (!file.type.includes('audio')) {
        showNotification('error', 'Solo archivos de audio permitidos');
        return;
    }

    if (file.size > 50 * 1024 * 1024) {  // 50MB límite
        showNotification('error', 'Archivo muy grande (máx 50MB)');
        return;
    }

    currentMP3 = {
        name: file.name,
        size: file.size,
        type: file.type
    };

    // Actualizar UI
    updateFileInfo();

    // Mostrar progreso de carga
    showUploadProgress();

    // Enviar al dispositivo en chunks
    await uploadMP3ToDevice(file);
}

async function uploadMP3ToDevice(file) {
    if (!writer) {
        showNotification('error', 'No conectado al dispositivo');
        return;
    }

    try {
        const CHUNK_SIZE = 4096;  // 4KB chunks (ajustable según tu dispositivo)
        const totalChunks = Math.ceil(file.size / CHUNK_SIZE);
        let uploadedBytes = 0;
        let lastTime = Date.now();

        // Comando de inicio
        await sendCommand(`MP3_UPLOAD|${file.size}`);

        const reader = file.stream().getReader();
        let chunkIndex = 0;

        while (true) {
            const { done, value } = await reader.read();
            if (done) break;

            // Enviar chunk
            await writer.write(value);
            uploadedBytes += value.length;
            chunkIndex++;

            // Calcular velocidad y actualizar progreso
            const currentTime = Date.now();
            const elapsedSeconds = (currentTime - lastTime) / 1000;
            uploadSpeed = (value.length / elapsedSeconds / 1024).toFixed(1);

            const progress = (uploadedBytes / file.size) * 100;
            updateUploadProgress(progress);

            // Pequeña pausa para evitar saturar el puerto
            await new Promise(resolve => setTimeout(resolve, 10));
        }

        showNotification('success', '✓ MP3 subido correctamente');
        hideUploadProgress();
        updateStats();

    } catch (error) {
        showNotification('error', 'Error upload: ' + error.message);
    }
}

// ==================== REPRODUCCIÓN ====================

async function playMP3() {
    const response = await sendCommand('TIMELINE|PLAY');
    if (response?.includes('OK')) {
        isPlaying = true;
        playbackStartTime = Date.now();
        updatePlaybackControls();
        updatePlaybackProgress();
        showNotification('success', '▶ Reproduciendo');
    }
}

async function pauseMP3() {
    const response = await sendCommand('TIMELINE|PAUSE');
    if (response?.includes('OK')) {
        isPlaying = false;
        updatePlaybackControls();
        showNotification('info', '⏸ En pausa');
    }
}

async function stopMP3() {
    const response = await sendCommand('TIMELINE|STOP');
    if (response?.includes('OK')) {
        isPlaying = false;
        playbackStartTime = 0;
        updatePlaybackControls();
        updatePlaybackProgress();
        showNotification('info', '⏹ Detenido');
    }
}

function updatePlaybackProgress() {
    if (!currentMP3 || !isPlaying) return;

    const elapsed = Date.now() - playbackStartTime;
    const duration = currentMP3.size * 8 / 128000;  // Aproximado en segundos

    const progressPercent = Math.min((elapsed / (duration * 1000)) * 100, 100);
    document.getElementById('playbackProgress').style.width = progressPercent + '%';

    const currentTime = formatTime(elapsed / 1000);
    const totalTime = formatTime(duration);
    document.getElementById('timeDisplay').textContent = `${currentTime} / ${totalTime}`;

    if (isPlaying) {
        requestAnimationFrame(updatePlaybackProgress);
    }
}

// ==================== TIMELINE KEYFRAMES ====================

function addKeyframe() {
    const time = parseInt(document.getElementById('keyframeTime').value) || 0;
    const color = document.getElementById('keyframeColor').value;
    const effect = document.getElementById('keyframeEffect').value;
    const brightness = parseInt(document.getElementById('keyframeBrightness').value);

    // Convertir hex a RGB
    const rgb = hexToRgb(color);

    const keyframe = {
        timestamp: time,
        r: rgb.r,
        g: rgb.g,
        b: rgb.b,
        brightness: brightness,
        effect: effect,
        duration: 1000  // 1 segundo por defecto
    };

    timelineData.push(keyframe);
    timelineData.sort((a, b) => a.timestamp - b.timestamp);

    renderKeyframeList();
    showNotification('success', '✓ Keyframe agregado');

    // Limpiar inputs
    document.getElementById('keyframeTime').value = '';
}

function removeKeyframe(index) {
    timelineData.splice(index, 1);
    renderKeyframeList();
    showNotification('info', 'Keyframe eliminado');
}

function renderKeyframeList() {
    const list = document.getElementById('keyframeList');

    if (timelineData.length === 0) {
        list.innerHTML = '<p style="color: #999; text-align: center;">No hay keyframes aún</p>';
        return;
    }

    list.innerHTML = timelineData.map((kf, index) => `
        <div class="keyframe-item">
            <div class="keyframe-time">${formatTime(kf.timestamp / 1000)}</div>
            <div class="keyframe-color" style="background: rgb(${kf.r}, ${kf.g}, ${kf.b});"></div>
            <div class="keyframe-effect">
                <strong>${kf.effect}</strong>
                <span style="color: #999;"> • Brillo: ${Math.round(kf.brightness / 255 * 100)}%</span>
            </div>
            <button class="btn-delete" onclick="removeKeyframe(${index})">✕</button>
        </div>
    `).join('');

    // Actualizar estadística
    document.getElementById('statKeyframes').textContent = timelineData.length;
}

// ==================== GUARDAR/CARGAR TIMELINE ====================

async function saveTimeline() {
    if (timelineData.length === 0) {
        showNotification('error', 'No hay keyframes para guardar');
        return;
    }

    const timeline = {
        name: currentMP3?.name || 'timeline',
        keyframes: timelineData,
        created: new Date().toISOString()
    };

    // Crear archivo JSON
    const json = JSON.stringify(timeline, null, 2);
    const blob = new Blob([json], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `timeline_${Date.now()}.json`;
    a.click();

    // También enviar al dispositivo
    if (writer) {
        await sendCommand(`TIMELINE|UPLOAD|${JSON.stringify(timeline)}`);
        showNotification('success', '💾 Timeline guardado');
    }
}

async function loadTimeline() {
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = '.json';

    input.onchange = async (e) => {
        const file = e.target.files[0];
        if (!file) return;

        const text = await file.text();
        try {
            const timeline = JSON.parse(text);
            timelineData = timeline.keyframes || [];
            renderKeyframeList();
            showNotification('success', '📂 Timeline cargado');
        } catch (error) {
            showNotification('error', 'Error al leer archivo JSON');
        }
    };

    input.click();
}

function clearTimeline() {
    if (confirm('¿Eliminar todos los keyframes?')) {
        timelineData = [];
        renderKeyframeList();
        showNotification('info', '🗑️ Timeline limpiado');
    }
}

// ==================== UTILIDADES ====================

function updateFileInfo() {
    document.getElementById('fileInfo').classList.add('active');
    document.getElementById('fileName').textContent = `📁 ${currentMP3.name}`;
    document.getElementById('fileSize').textContent = `Tamaño: ${(currentMP3.size / 1024 / 1024).toFixed(2)} MB`;

    // Estimar duración (aproximado para MP3 a 128kbps)
    const estimatedDuration = (currentMP3.size * 8) / 128000;
    document.getElementById('fileDuration').textContent = `Duración: ${formatTime(estimatedDuration)}`;

    updateStats();
}

function updateStats() {
    if (currentMP3) {
        document.getElementById('statSize').textContent = (currentMP3.size / 1024 / 1024).toFixed(2) + ' MB';
        const duration = (currentMP3.size * 8) / 128000;
        document.getElementById('statDuration').textContent = formatTime(duration);
        document.getElementById('statSpeed').textContent = uploadSpeed + ' KB/s';
    }
}

function updatePlaybackControls() {
    document.getElementById('playBtn').disabled = !currentMP3 || isPlaying;
    document.getElementById('pauseBtn').disabled = !isPlaying;
    document.getElementById('stopBtn').disabled = !currentMP3;
}

function updateDeviceStatus(connected) {
    // Actualizar UI si es necesario
    document.getElementById('playBtn').disabled = !connected || !currentMP3;
}

function showUploadProgress() {
    document.getElementById('uploadProgress').classList.add('active');
}

function hideUploadProgress() {
    document.getElementById('uploadProgress').classList.remove('active');
}

function updateUploadProgress(percent) {
    document.getElementById('uploadProgressFill').style.width = percent + '%';
    document.getElementById('uploadPercent').textContent = Math.round(percent) + '%';
}

function showNotification(type, message) {
    const notification = document.createElement('div');
    notification.className = `notification ${type}`;
    notification.textContent = message;
    document.body.appendChild(notification);

    setTimeout(() => {
        notification.remove();
    }, 3000);
}

function formatTime(seconds) {
    const mins = Math.floor(seconds / 60);
    const secs = Math.floor(seconds % 60);
    return `${mins.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}`;
}

function hexToRgb(hex) {
    const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : { r: 255, g: 0, b: 0 };
}

// Renderizar lista inicial
renderKeyframeList();
updatePlaybackControls();

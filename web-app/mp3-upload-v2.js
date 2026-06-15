// ==================== MP3 UPLOAD V2 - LOCAL STORAGE + DEVICE SYNC ====================

let port = null;
let reader = null;
let writer = null;
let currentMP3 = null;
let timelineData = [];
let isPlaying = false;
let playbackStartTime = 0;
let playbackDuration = 0;
let localMP3Files = {};
let dbInstance = null;
let uploadSpeed = 0;

const STORAGE_KEY = 'capellan_mp3_data';
const DB_NAME = 'CapellanLED';
const STORE_NAME = 'mp3files';

// ==================== INICIALIZACIÓN ====================

document.addEventListener('DOMContentLoaded', async () => {
    await initIndexedDB();
    loadLocalMP3Files();
    setupEventListeners();
    setupSerialConnection();
    updatePlaybackControls();
    updateDeviceStatus(false);
});

// ==================== INDEXED DB (Local Storage) ====================

async function initIndexedDB() {
    return new Promise((resolve, reject) => {
        const request = indexedDB.open(DB_NAME, 1);

        request.onerror = () => reject(request.error);
        request.onsuccess = () => {
            dbInstance = request.result;
            resolve();
        };

        request.onupgradeneeded = (event) => {
            const db = event.target.result;
            if (!db.objectStoreNames.contains(STORE_NAME)) {
                db.createObjectStore(STORE_NAME, { keyPath: 'id' });
            }
        };
    });
}

async function saveMP3ToIndexedDB(filename, fileData, size, duration) {
    return new Promise((resolve, reject) => {
        const transaction = dbInstance.transaction([STORE_NAME], 'readwrite');
        const store = transaction.objectStore(STORE_NAME);

        const mp3Object = {
            id: filename,
            name: filename,
            data: fileData,
            size: size,
            duration: duration,
            createdAt: new Date().toISOString()
        };

        const request = store.put(mp3Object);
        request.onerror = () => reject(request.error);
        request.onsuccess = () => {
            resolve(mp3Object);
        };
    });
}

async function loadMP3FromIndexedDB(filename) {
    return new Promise((resolve, reject) => {
        const transaction = dbInstance.transaction([STORE_NAME], 'readonly');
        const store = transaction.objectStore(STORE_NAME);
        const request = store.get(filename);

        request.onerror = () => reject(request.error);
        request.onsuccess = () => resolve(request.result);
    });
}

async function getAllMP3s() {
    return new Promise((resolve, reject) => {
        const transaction = dbInstance.transaction([STORE_NAME], 'readonly');
        const store = transaction.objectStore(STORE_NAME);
        const request = store.getAll();

        request.onerror = () => reject(request.error);
        request.onsuccess = () => resolve(request.result);
    });
}

async function deleteMP3FromIndexedDB(filename) {
    return new Promise((resolve, reject) => {
        const transaction = dbInstance.transaction([STORE_NAME], 'readwrite');
        const store = transaction.objectStore(STORE_NAME);
        const request = store.delete(filename);

        request.onerror = () => reject(request.error);
        request.onsuccess = () => {
            resolve();
        };
    });
}

async function loadLocalMP3Files() {
    try {
        const files = await getAllMP3s();
        updateMP3SelectOptions(files);
    } catch (error) {
        console.error('Error loading local files:', error);
    }
}

function updateMP3SelectOptions(files) {
    const select = document.getElementById('mp3SelectEdit');
    select.innerHTML = '<option value="">Selecciona un MP3</option>';

    files.forEach(file => {
        const option = document.createElement('option');
        option.value = file.name;
        option.textContent = `${file.name} (${formatTime(file.duration)})`;
        select.appendChild(option);
    });
}

// ==================== EVENT LISTENERS ====================

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
    document.getElementById('syncDeviceBtn').addEventListener('click', syncToDevice);

    // Local files
    document.getElementById('viewLocalFilesBtn').addEventListener('click', showLocalFilesModal);

    // Selector de MP3
    document.getElementById('mp3SelectEdit').addEventListener('change', selectMP3ForEdit);

    // Inputs de tiempo en tiempo real
    document.getElementById('keyframeMinutes').addEventListener('input', updateTimeSynchronized);
    document.getElementById('keyframeSeconds').addEventListener('input', updateTimeSynchronized);
    document.getElementById('keyframeMillis').addEventListener('input', updateTimeSynchronized);
}

// ==================== CARGA DE MP3 ====================

async function handleMP3Upload(file) {
    // Validar
    if (!file.type.includes('audio')) {
        showNotification('error', '❌ Solo archivos de audio permitidos');
        return;
    }

    if (file.size > 100 * 1024 * 1024) {  // 100MB límite
        showNotification('error', '❌ Archivo muy grande (máx 100MB)');
        return;
    }

    currentMP3 = {
        name: file.name,
        size: file.size,
        type: file.type
    };

    // Calcular duración estimada (aproximado para MP3 a 128kbps)
    const estimatedDuration = (file.size * 8) / (128 * 1000);
    currentMP3.duration = estimatedDuration;

    // Actualizar UI
    updateFileInfo();

    // Mostrar progreso
    showUploadProgress();

    // Guardar en IndexedDB SIN necesidad de conexión USB
    await saveMP3LocallyWithProgress(file);
}

async function saveMP3LocallyWithProgress(file) {
    try {
        let uploadedBytes = 0;
        const CHUNK_SIZE = 1024 * 1024;  // 1MB chunks
        let fileData = new Uint8Array();
        let lastTime = Date.now();

        const reader = file.stream().getReader();

        while (true) {
            const { done, value } = await reader.read();
            if (done) break;

            // Acumular datos
            const newData = new Uint8Array(fileData.length + value.length);
            newData.set(fileData);
            newData.set(new Uint8Array(value), fileData.length);
            fileData = newData;

            uploadedBytes += value.length;

            // Calcular velocidad
            const currentTime = Date.now();
            const elapsedSeconds = (currentTime - lastTime) / 1000;
            if (elapsedSeconds > 0) {
                uploadSpeed = (value.length / elapsedSeconds / 1024).toFixed(1);
            }

            // Actualizar progreso
            const progress = (uploadedBytes / file.size) * 100;
            updateUploadProgress(progress);

            await new Promise(resolve => setTimeout(resolve, 10));
        }

        // Guardar en IndexedDB
        await saveMP3ToIndexedDB(
            file.name,
            fileData,
            file.size,
            currentMP3.duration
        );

        // Actualizar lista de archivos
        await loadLocalMP3Files();

        hideUploadProgress();
        updateStats();
        showNotification('success', `✓ MP3 guardado localmente (${(file.size / 1024 / 1024).toFixed(2)}MB)`);

    } catch (error) {
        showNotification('error', '❌ Error al guardar: ' + error.message);
    }
}

// ==================== MODAL: ARCHIVOS LOCALES ====================

async function showLocalFilesModal() {
    const modal = document.getElementById('localFilesModal');
    const filesList = document.getElementById('localFilesList');

    try {
        const files = await getAllMP3s();

        if (files.length === 0) {
            filesList.innerHTML = '<p style="color: #999; text-align: center;">No hay archivos guardados</p>';
            modal.classList.add('active');
            return;
        }

        filesList.innerHTML = files.map(file => `
            <div class="local-file-item">
                <div class="local-file-info">
                    <div class="local-file-name">📁 ${file.name}</div>
                    <div class="local-file-size">
                        ${(file.size / 1024 / 1024).toFixed(2)} MB • ${formatTime(file.duration)}
                    </div>
                    <div style="color: #999; font-size: 0.75em;">
                        ${new Date(file.createdAt).toLocaleDateString('es-ES', {
                            month: 'short',
                            day: 'numeric',
                            hour: '2-digit',
                            minute: '2-digit'
                        })}
                    </div>
                </div>
                <div style="display: flex; gap: 5px;">
                    <button class="btn-primary btn-small" onclick="loadMP3ForEdit('${file.name}')">Editar</button>
                    <button class="btn-secondary btn-small" onclick="deleteMP3File('${file.name}')">✕</button>
                </div>
            </div>
        `).join('');

        modal.classList.add('active');
    } catch (error) {
        showNotification('error', '❌ Error al cargar archivos');
    }
}

function closeLocalFilesModal() {
    document.getElementById('localFilesModal').classList.remove('active');
}

async function loadMP3ForEdit(filename) {
    try {
        const mp3 = await loadMP3FromIndexedDB(filename);
        if (mp3) {
            currentMP3 = {
                name: mp3.name,
                size: mp3.size,
                duration: mp3.duration,
                data: mp3.data
            };
            updateFileInfo();
            updateStats();
            closeLocalFilesModal();
            showNotification('success', `✓ MP3 cargado: ${filename}`);
        }
    } catch (error) {
        showNotification('error', '❌ Error al cargar MP3');
    }
}

async function deleteMP3File(filename) {
    if (confirm(`¿Eliminar "${filename}"?`)) {
        try {
            await deleteMP3FromIndexedDB(filename);
            await loadLocalMP3Files();
            showLocalFilesModal();
            showNotification('info', '🗑️ Archivo eliminado');
        } catch (error) {
            showNotification('error', '❌ Error al eliminar');
        }
    }
}

function selectMP3ForEdit() {
    const filename = document.getElementById('mp3SelectEdit').value;
    if (filename) {
        loadMP3ForEdit(filename);
    }
}

// ==================== REPRODUCTOR DE AUDIO ====================

async function playMP3() {
    if (!currentMP3) {
        showNotification('error', '❌ Carga un MP3 primero');
        return;
    }

    isPlaying = true;
    playbackStartTime = Date.now();
    playbackDuration = currentMP3.duration * 1000;
    updatePlaybackControls();
    updatePlaybackProgress();
    showNotification('success', '▶ Reproduciendo');
}

async function pauseMP3() {
    isPlaying = false;
    updatePlaybackControls();
    showNotification('info', '⏸ En pausa');
}

async function stopMP3() {
    isPlaying = false;
    playbackStartTime = 0;
    updatePlaybackControls();
    updatePlaybackProgress();
    showNotification('info', '⏹ Detenido');
}

function updatePlaybackProgress() {
    if (!currentMP3 || !isPlaying) return;

    const elapsed = Date.now() - playbackStartTime;
    const progressPercent = Math.min((elapsed / playbackDuration) * 100, 100);
    
    document.getElementById('playbackProgress').style.width = progressPercent + '%';

    const currentTime = formatTime(elapsed / 1000);
    const totalTime = formatTime(currentMP3.duration);
    document.getElementById('timeDisplay').textContent = `${currentTime} / ${totalTime}`;

    if (isPlaying && elapsed < playbackDuration) {
        requestAnimationFrame(updatePlaybackProgress);
    }
}

// ==================== TIEMPO CON SINCRONIZACIÓN EN TIEMPO REAL ====================

function updateTimeSynchronized() {
    const minutes = parseInt(document.getElementById('keyframeMinutes').value) || 0;
    const seconds = parseInt(document.getElementById('keyframeSeconds').value) || 0;
    const millis = parseInt(document.getElementById('keyframeMillis').value) || 0;

    const totalMs = (minutes * 60 + seconds) * 1000 + millis;
    const formatted = formatTimeMillis(totalMs);

    document.getElementById('timeDisplaySync').textContent = formatted;
}

function formatTimeMillis(ms) {
    const totalSeconds = Math.floor(ms / 1000);
    const minutes = Math.floor(totalSeconds / 60);
    const seconds = totalSeconds % 60;
    const millis = ms % 1000;

    return `${minutes.toString().padStart(2, '0')}:${seconds.toString().padStart(2, '0')}.${millis.toString().padStart(3, '0')}`;
}

// ==================== TIMELINE KEYFRAMES ====================

function addKeyframe() {
    const minutes = parseInt(document.getElementById('keyframeMinutes').value) || 0;
    const seconds = parseInt(document.getElementById('keyframeSeconds').value) || 0;
    const millis = parseInt(document.getElementById('keyframeMillis').value) || 0;

    const timeMs = (minutes * 60 + seconds) * 1000 + millis;
    const color = document.getElementById('keyframeColor').value;
    const effect = document.getElementById('keyframeEffect').value;
    const brightness = parseInt(document.getElementById('keyframeBrightness').value);

    // Validar que esté dentro de la duración del MP3
    if (currentMP3 && timeMs > currentMP3.duration * 1000) {
        showNotification('error', `❌ Tiempo excede la duración del MP3 (${formatTime(currentMP3.duration)})`);
        return;
    }

    const rgb = hexToRgb(color);

    const keyframe = {
        timestamp: timeMs,
        time_formatted: formatTimeMillis(timeMs),
        r: rgb.r,
        g: rgb.g,
        b: rgb.b,
        brightness: brightness,
        effect: effect,
        duration: 1000
    };

    timelineData.push(keyframe);
    timelineData.sort((a, b) => a.timestamp - b.timestamp);

    renderKeyframeList();
    renderTimelineVisual();
    showNotification('success', `✓ Keyframe en ${formatTimeMillis(timeMs)}`);

    // Limpiar inputs
    document.getElementById('keyframeMinutes').value = '0';
    document.getElementById('keyframeSeconds').value = '0';
    document.getElementById('keyframeMillis').value = '0';
    updateTimeSynchronized();
}

function removeKeyframe(index) {
    const time = timelineData[index].time_formatted;
    timelineData.splice(index, 1);
    renderKeyframeList();
    renderTimelineVisual();
    showNotification('info', `Keyframe ${time} eliminado`);
}

function renderKeyframeList() {
    const list = document.getElementById('keyframeList');

    if (timelineData.length === 0) {
        list.innerHTML = '<p style="color: #999; text-align: center;">No hay keyframes aún</p>';
        document.getElementById('statKeyframes').textContent = '0';
        return;
    }

    list.innerHTML = timelineData.map((kf, index) => `
        <div class="keyframe-item">
            <div class="keyframe-time">${kf.time_formatted}</div>
            <div class="keyframe-color" style="background: rgb(${kf.r}, ${kf.g}, ${kf.b});"></div>
            <div class="keyframe-details">
                <div class="keyframe-info">
                    <div class="keyframe-effect">
                        <strong>${kf.effect.charAt(0).toUpperCase() + kf.effect.slice(1)}</strong>
                        <span style="color: #999;"> • Brillo: ${kf.brightness}%</span>
                    </div>
                </div>
                <button class="btn-delete" onclick="removeKeyframe(${index})">✕</button>
            </div>
        </div>
    `).join('');

    document.getElementById('statKeyframes').textContent = timelineData.length;
}

function renderTimelineVisual() {
    const visual = document.getElementById('timelineVisual');
    visual.innerHTML = '';

    if (timelineData.length === 0 || !currentMP3) return;

    timelineData.forEach(kf => {
        const percent = (kf.timestamp / (currentMP3.duration * 1000)) * 100;
        const marker = document.createElement('div');
        marker.className = 'keyframe-marker';
        marker.style.left = percent + '%';
        marker.style.background = `rgb(${kf.r}, ${kf.g}, ${kf.b})`;
        marker.title = `${kf.time_formatted} - ${kf.effect}`;
        visual.appendChild(marker);
    });
}

// ==================== GUARDAR/CARGAR TIMELINE ====================

async function saveTimeline() {
    if (timelineData.length === 0) {
        showNotification('error', '❌ No hay keyframes para guardar');
        return;
    }

    if (!currentMP3) {
        showNotification('error', '❌ Selecciona un MP3 primero');
        return;
    }

    const timeline = {
        name: `${currentMP3.name}_timeline`,
        mp3_file: currentMP3.name,
        mp3_duration: currentMP3.duration,
        keyframes: timelineData,
        created: new Date().toISOString()
    };

    // Descargar como JSON
    const json = JSON.stringify(timeline, null, 2);
    const blob = new Blob([json], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `${currentMP3.name.replace('.mp3', '')}_timeline.json`;
    a.click();

    showNotification('success', '💾 Timeline descargado');
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
            renderTimelineVisual();
            showNotification('success', '📂 Timeline cargado');
        } catch (error) {
            showNotification('error', '❌ Error al leer archivo JSON');
        }
    };

    input.click();
}

function clearTimeline() {
    if (confirm('¿Eliminar todos los keyframes?')) {
        timelineData = [];
        renderKeyframeList();
        renderTimelineVisual();
        showNotification('info', '🗑️ Timeline limpiado');
    }
}

// ==================== SINCRONIZAR CON DISPOSITIVO ====================

async function syncToDevice() {
    if (!port) {
        showNotification('error', '❌ No conectado al dispositivo');
        return;
    }

    if (timelineData.length === 0) {
        showNotification('error', '❌ No hay keyframes para sincronizar');
        return;
    }

    try {
        const timeline = {
            mp3_file: currentMP3?.name || 'audio.mp3',
            keyframes: timelineData
        };

        const command = `TIMELINE|UPLOAD|${JSON.stringify(timeline)}\n`;
        await writer.write(new TextEncoder().encode(command));

        showNotification('success', '📤 Timeline sincronizado con dispositivo');
    } catch (error) {
        showNotification('error', '❌ Error sincronizar: ' + error.message);
    }
}

// ==================== CONEXIÓN SERIAL ====================

async function setupSerialConnection() {
    try {
        const ports = await navigator.serial.getPorts();
        if (ports.length > 0) {
            await connectToPort(ports[0]);
        }
    } catch (error) {
        console.log('Serial API no disponible - Modo Local');
    }
}

async function connectToPort(selectedPort) {
    try {
        port = selectedPort;
        await port.open({ baudRate: 115200 });
        reader = port.readable.getReader();
        writer = port.writable.getWriter();
        updateDeviceStatus(true);
        showNotification('success', '✓ Dispositivo conectado');
    } catch (error) {
        console.error('Error de conexión:', error.message);
        updateDeviceStatus(false);
    }
}

// ==================== UTILIDADES ====================

function updateFileInfo() {
    document.getElementById('fileInfo').classList.add('active');
    document.getElementById('fileName').textContent = `📁 ${currentMP3.name}`;
    document.getElementById('fileSize').textContent = `Tamaño: ${(currentMP3.size / 1024 / 1024).toFixed(2)} MB`;
    document.getElementById('fileDuration').textContent = `Duración: ${formatTime(currentMP3.duration)}`;
    updateStats();
}

function updateStats() {
    if (currentMP3) {
        document.getElementById('statSize').textContent = (currentMP3.size / 1024 / 1024).toFixed(2) + ' MB';
        document.getElementById('statDuration').textContent = formatTime(currentMP3.duration);
        document.getElementById('statSpeed').textContent = uploadSpeed + ' KB/s';
    }
}

function updatePlaybackControls() {
    document.getElementById('playBtn').disabled = !currentMP3 || isPlaying;
    document.getElementById('pauseBtn').disabled = !isPlaying;
    document.getElementById('stopBtn').disabled = !currentMP3;
}

function updateDeviceStatus(connected) {
    const status = document.getElementById('deviceStatus');
    if (connected) {
        status.textContent = '🟢 Conectado';
        status.className = 'device-status connected';
        document.getElementById('syncDeviceBtn').disabled = false;
    } else {
        status.textContent = '🔴 Desconectado (Modo Local)';
        status.className = 'device-status disconnected';
        document.getElementById('syncDeviceBtn').disabled = true;
    }
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

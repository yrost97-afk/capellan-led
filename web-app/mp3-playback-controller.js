// ==================== REPRODUCTOR MP3 INTEGRADO + TIMELINE SYNC ====================

/**
 * Inicialización del reproductor MP3 + Timeline
 */

// Variables de reproducción
let currentMP3 = null;
let currentMP3Events = [];
let isMP3Playing = false;

/**
 * Interfaz para cargar y reproducir MP3 con timeline
 */
async function loadMP3ForPlayback(fileOrName) {
    try {
        // Si es nombre de archivo, cargar de IndexedDB
        if (typeof fileOrName === 'string') {
            const mp3Data = await loadMP3FromIndexedDB(fileOrName);
            if (!mp3Data) {
                showNotification('❌ Archivo MP3 no encontrado', 'error');
                return false;
            }
            currentMP3 = mp3Data;
        } else {
            // Si es File object
            currentMP3 = {
                name: fileOrName.name,
                size: fileOrName.size,
                data: fileOrName
            };
        }

        updateMP3DisplayInfo();
        showNotification(`✅ MP3 cargado: ${currentMP3.name}`, 'success');
        return true;

    } catch (error) {
        showNotification(`❌ Error: ${error.message}`, 'error');
        return false;
    }
}

/**
 * Reproduce MP3 con timeline sincronizado
 */
async function playMP3WithTimeline() {
    if (!currentMP3) {
        showNotification('❌ Carga un MP3 primero', 'error');
        return;
    }

    if (!timelineEvents || timelineEvents.length === 0) {
        showNotification('⚠️ No hay eventos de timeline', 'warning');
        return;
    }

    try {
        // Preparar timeline
        const timeline = {
            name: currentMP3.name.replace('.mp3', ''),
            duration: getAudioDuration() * 1000,
            events: timelineEvents.map(e => ({
                time: e.time,
                type: e.type,
                effectName: e.effectName,
                r: e.r,
                g: e.g,
                b: e.b,
                brightness: e.brightness,
                speed: e.speed,
                duration: e.duration
            }))
        };

        // Callback para animar vista previa mientras se reproduce
        onTimelineEvent((event, currentTime) => {
            updateLEDPreview(event);
            updateTimelineProgressIndicator(currentTime);
        });

        // Iniciar reproducción
        await playTimelineWithAudio(timeline, currentMP3.data);
        isMP3Playing = true;
        updateMP3ControlButtons();

    } catch (error) {
        showNotification(`❌ Error: ${error.message}`, 'error');
    }
}

/**
 * Pausa reproducción
 */
function pauseMP3WithTimeline() {
    if (isMP3Playing) {
        pauseTimelineWithAudio();
        isMP3Playing = false;
        updateMP3ControlButtons();
    }
}

/**
 * Reanuda reproducción
 */
function resumeMP3WithTimeline() {
    if (!isMP3Playing && audioBuffer) {
        resumeTimelineWithAudio();
        isMP3Playing = true;
        updateMP3ControlButtons();
    }
}

/**
 * Detiene reproducción
 */
function stopMP3WithTimeline() {
    if (isMP3Playing || audioBuffer) {
        stopTimelineWithAudio();
        isMP3Playing = false;
        updateMP3ControlButtons();
        
        // Resetear vista previa
        if (document.getElementById('ledStripPreview')) {
            document.querySelectorAll('.led-dot').forEach(dot => {
                dot.style.background = '#444';
                dot.style.boxShadow = '0 0 10px rgba(100, 100, 100, 0.3)';
            });
        }
    }
}

/**
 * Busca a un tiempo específico en el MP3
 * @param {number} timeMs - Tiempo en milisegundos
 */
function seekMP3Time(timeMs) {
    if (!audioBuffer) return;
    seekAudioTime(timeMs);
}

/**
 * Actualiza información de MP3 en la UI
 */
function updateMP3DisplayInfo() {
    if (!currentMP3) return;

    const nameEl = document.getElementById('mp3NameDisplay');
    const sizeEl = document.getElementById('mp3SizeDisplay');
    const durationEl = document.getElementById('mp3DurationDisplay');

    if (nameEl) nameEl.textContent = `📁 ${currentMP3.name}`;
    if (sizeEl && currentMP3.size) {
        sizeEl.textContent = `${(currentMP3.size / 1024 / 1024).toFixed(2)} MB`;
    }
    if (durationEl && audioBuffer) {
        durationEl.textContent = formatTime(audioBuffer.duration);
    }
}

/**
 * Actualiza botones de control
 */
function updateMP3ControlButtons() {
    const playBtn = document.getElementById('mp3PlayBtn');
    const pauseBtn = document.getElementById('mp3PauseBtn');
    const stopBtn = document.getElementById('mp3StopBtn');

    if (playBtn) playBtn.disabled = isMP3Playing;
    if (pauseBtn) pauseBtn.disabled = !isMP3Playing;
    if (stopBtn) stopBtn.disabled = !isMP3Playing && !audioBuffer;
}

/**
 * Actualiza vista previa LED en tiempo real
 * @param {object} event - Evento del timeline
 */
function updateLEDPreview(event) {
    const leds = document.querySelectorAll('.led-dot');
    if (!leds || leds.length === 0) return;

    // Obtener color del evento
    let color = '#444';
    let opacity = 1;

    if (event.type === 'color') {
        color = `rgb(${event.r}, ${event.g}, ${event.b})`;
    } else if (event.type === 'brightness') {
        const brightness = event.level || 128;
        color = `rgb(${brightness}, ${brightness}, ${brightness})`;
    } else if (event.type === 'effect') {
        color = `rgb(${event.r || 100}, ${event.g || 100}, ${event.b || 100})`;
        opacity = 0.8;
    }

    // Aplicar animación a los LEDs
    leds.forEach((led, index) => {
        const delay = (index / leds.length) * 0.1;
        led.style.background = color;
        led.style.opacity = opacity;
        led.style.boxShadow = `0 0 20px ${color}`;
        led.style.animation = `ledFlash 0.3s ease-in-out ${delay}s`;
    });
}

/**
 * Actualiza indicador de progreso del timeline visual
 */
function updateTimelineProgressIndicator(currentTimeMs) {
    const progressDisplay = document.getElementById('playbackTimeDisplay');
    if (!progressDisplay) return;

    const currentSec = currentTimeMs / 1000;
    const totalSec = audioBuffer ? audioBuffer.duration : 0;

    progressDisplay.textContent = `${formatTime(currentSec)} / ${formatTime(totalSec)}`;
}

/**
 * Sincroniza reproducción web con dispositivo ESP32
 * Reproduce en ambos simultáneamente
 */
async function syncPlayOnDevice() {
    if (!serialConnected) {
        showNotification('❌ Dispositivo no conectado', 'error');
        return;
    }

    if (!timelineEvents || timelineEvents.length === 0) {
        showNotification('❌ No hay timeline para enviar', 'error');
        return;
    }

    try {
        const timelineName = `web-sync-${Date.now()}`;
        
        const timeline = {
            name: timelineName,
            duration: audioBuffer ? audioBuffer.duration * 1000 : 0,
            events: timelineEvents
        };

        // Primero subir timeline al dispositivo
        showNotification('📤 Enviando timeline al dispositivo...', 'info');
        await uploadTimelineToDevice(timeline, timelineName);

        // Pequeño delay
        await new Promise(r => setTimeout(r, 500));

        // Luego reproducir en web
        showNotification('▶️ Reproduciendo en web + dispositivo sincronizados', 'success');
        await playMP3WithTimeline();

        // Y ejecutar en dispositivo
        await playDeviceTimeline(timelineName);

    } catch (error) {
        showNotification(`❌ Error: ${error.message}`, 'error');
    }
}

/**
 * Presets de uso común
 */
const MP3_PLAYBACK_PRESETS = {
    'web-only': {
        label: 'Solo Web',
        description: 'Reproduce MP3 + timeline en navegador'
    },
    'device-only': {
        label: 'Solo Dispositivo',
        description: 'Ejecuta timeline guardado en ESP32'
    },
    'sync': {
        label: 'Sincronizado',
        description: 'Web + dispositivo reproducen juntos'
    }
};

/**
 * Selecciona modo de reproducción
 */
function selectPlaybackMode(mode) {
    const modeDisplay = document.getElementById('playbackModeDisplay');
    if (modeDisplay) {
        modeDisplay.textContent = MP3_PLAYBACK_PRESETS[mode]?.label || mode;
    }
    
    console.log(`🎵 Modo seleccionado: ${mode}`);
}

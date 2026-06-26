// ==================== AUDIO SYNC PLAYER - Reproduce MP3 con Timeline ====================

let audioContext = null;
let audioSource = null;
let audioBuffer = null;
let audioStartTime = 0;
let isAudioPlaying = false;
let timelineStartTime = 0;
let currentTimeline = null;
let playbackCallbacks = [];

/**
 * Inicializa el contexto de audio
 */
function initAudioContext() {
    if (!audioContext) {
        audioContext = new (window.AudioContext || window.webkitAudioContext)();
    }
    return audioContext;
}

/**
 * Carga un archivo MP3 desde local storage o IndexedDB
 * @param {string} filename - Nombre del archivo MP3
 * @returns {Promise<ArrayBuffer>} - Buffer de audio
 */
async function loadMP3File(filename) {
    try {
        // Si es un File object, leerlo directamente
        if (filename instanceof File) {
            return await filename.arrayBuffer();
        }

        // Si está en IndexedDB (por mp3-upload-v2.js)
        try {
            const mp3 = await loadMP3FromIndexedDB(filename);
            if (mp3 && mp3.data) {
                return mp3.data;
            }
        } catch (e) {
            console.log('No en IndexedDB, buscando localmente...');
        }

        // Como último recurso, intentar fetch
        const response = await fetch(filename);
        if (!response.ok) throw new Error('MP3 no encontrado');
        return await response.arrayBuffer();

    } catch (error) {
        showNotification(`❌ Error cargando MP3: ${error.message}`, 'error');
        throw error;
    }
}

/**
 * Prepara el contexto de audio decodificando el MP3
 * @param {ArrayBuffer} arrayBuffer - Buffer de audio
 */
async function prepareAudioBuffer(arrayBuffer) {
    try {
        initAudioContext();
        audioBuffer = await audioContext.decodeAudioData(arrayBuffer);
        console.log(`✅ Audio decodificado: ${(audioBuffer.duration).toFixed(2)}s`);
        return audioBuffer;
    } catch (error) {
        console.error('Error decodificando audio:', error);
        throw error;
    }
}

/**
 * Inicia la reproducción de MP3 + Timeline sincronizados
 * @param {object} timeline - Datos del timeline con eventos
 * @param {ArrayBuffer|File} audioFile - Archivo MP3
 */
async function playTimelineWithAudio(timeline, audioFile) {
    try {
        // Detener reproducción anterior si hay
        stopTimelineWithAudio();

        showNotification('⏳ Preparando audio y timeline...', 'info');

        // Cargar y decodificar audio
        const arrayBuffer = await loadMP3File(audioFile);
        await prepareAudioBuffer(arrayBuffer);

        // Guardar timeline actual
        currentTimeline = timeline;
        timelineStartTime = Date.now();

        // Crear source y reproducir
        initAudioContext();
        audioSource = audioContext.createBufferSource();
        audioSource.buffer = audioBuffer;
        audioSource.connect(audioContext.destination);

        audioStartTime = audioContext.currentTime;
        audioSource.start(audioContext.currentTime);
        isAudioPlaying = true;

        showNotification('▶️ Reproduciendo MP3 + Timeline sincronizado', 'success');
        console.log('▶️ Iniciada reproducción de audio + timeline');

        // Iniciar loop de sincronización de eventos
        syncTimelineWithAudio();

    } catch (error) {
        showNotification(`❌ Error: ${error.message}`, 'error');
        console.error('Error en playTimelineWithAudio:', error);
    }
}

/**
 * Loop que sincroniza eventos del timeline con el audio en reproducción
 */
function syncTimelineWithAudio() {
    if (!isAudioPlaying || !currentTimeline) return;

    const currentAudioTime = (audioContext.currentTime - audioStartTime) * 1000; // en ms

    // Procesar eventos que coincidan con el tiempo actual
    if (currentTimeline.events && currentTimeline.events.length > 0) {
        currentTimeline.events.forEach((event, index) => {
            // Si el evento está a 50ms o menos del tiempo actual y no ha sido ejecutado
            if (
                event.time <= currentAudioTime &&
                event.time > currentAudioTime - 50 &&
                !event._executed
            ) {
                event._executed = true;
                console.log(`🎬 [${(event.time / 1000).toFixed(2)}s] Ejecutando:`, event);

                // Ejecutar callback visual en la web
                if (playbackCallbacks.length > 0) {
                    playbackCallbacks.forEach(cb => cb(event, currentAudioTime));
                }

                // Si hay dispositivo conectado, enviar comando
                if (serialConnected && serialWriter) {
                    executeTimelineEvent(event).catch(err => 
                        console.warn('Error enviando evento al dispositivo:', err)
                    );
                }
            }
        });
    }

    // Actualizar UI de reproducción
    updatePlaybackDisplay(currentAudioTime);

    // Continuar loop
    if (isAudioPlaying) {
        requestAnimationFrame(syncTimelineWithAudio);
    }
}

/**
 * Detiene la reproducción de MP3 y timeline
 */
function stopTimelineWithAudio() {
    if (isAudioPlaying && audioSource) {
        audioSource.stop();
        isAudioPlaying = false;
        showNotification('⏹️ Reproducción detenida', 'warning');
        console.log('⏹️ Audio + Timeline detenido');
    }
}

/**
 * Pausa la reproducción
 */
function pauseTimelineWithAudio() {
    if (audioContext && isAudioPlaying) {
        audioContext.suspend();
        isAudioPlaying = false;
        showNotification('⏸️ En pausa', 'info');
    }
}

/**
 * Reanuda la reproducción
 */
function resumeTimelineWithAudio() {
    if (audioContext && audioContext.state === 'suspended') {
        audioContext.resume();
        isAudioPlaying = true;
        syncTimelineWithAudio();
        showNotification('▶️ Reanudando...', 'info');
    }
}

/**
 * Registra un callback para eventos del timeline
 * @param {function} callback - Función(event, currentTime)
 */
function onTimelineEvent(callback) {
    playbackCallbacks.push(callback);
}

/**
 * Actualiza display de reproducción (tiempo, progreso, etc)
 */
function updatePlaybackDisplay(currentTimeMs) {
    const displayTime = document.getElementById('playbackTimeDisplay');
    const progressBar = document.getElementById('playbackProgress');

    if (displayTime && audioBuffer) {
        const currentSec = currentTimeMs / 1000;
        const totalSec = audioBuffer.duration;
        const percent = (currentSec / totalSec) * 100;

        displayTime.textContent = `${formatTime(currentSec)} / ${formatTime(totalSec)}`;

        if (progressBar) {
            progressBar.style.width = Math.min(percent, 100) + '%';
        }

        // Detener si terminó
        if (currentSec >= totalSec) {
            stopTimelineWithAudio();
            showNotification('✅ Reproducción completada', 'success');
        }
    }
}

/**
 * Busca un tiempo en la reproducción de audio
 * @param {number} timeMs - Tiempo en milisegundos
 */
function seekAudioTime(timeMs) {
    if (!audioBuffer || !audioContext) return;

    try {
        // Parar reproducción actual
        if (isAudioPlaying && audioSource) {
            audioSource.stop();
        }

        // Crear nuevo source en el tiempo indicado
        audioSource = audioContext.createBufferSource();
        audioSource.buffer = audioBuffer;
        audioSource.connect(audioContext.destination);

        const seekSeconds = timeMs / 1000;
        audioStartTime = audioContext.currentTime - seekSeconds;
        audioSource.start(audioContext.currentTime, seekSeconds);

        isAudioPlaying = true;
        syncTimelineWithAudio();
        console.log(`⏩ Buscando a ${formatTime(seekSeconds)}`);

    } catch (error) {
        console.error('Error en seek:', error);
    }
}

/**
 * Utilidad para formatear tiempo
 */
function formatTime(seconds) {
    const mins = Math.floor(seconds / 60);
    const secs = Math.floor(seconds % 60);
    const ms = Math.floor((seconds % 1) * 1000);
    return `${mins.toString().padStart(2, '0')}:${secs.toString().padStart(2, '0')}.${ms.toString().padStart(3, '0')}`;
}

/**
 * Obtiene la duración del audio cargado
 */
function getAudioDuration() {
    return audioBuffer ? audioBuffer.duration : 0;
}

/**
 * Obtiene el tiempo de reproducción actual en ms
 */
function getCurrentPlaybackTime() {
    if (!audioBuffer || !isAudioPlaying) return 0;
    return (audioContext.currentTime - audioStartTime) * 1000;
}

// ==================== DEVICE TIMELINE MANAGER - Subir/Descargar timelines al ESP32 ====================

/**
 * Estructura de protocolo para enviar timelines al ESP32
 * Formato: TIMELINE|UPLOAD|{json_comprimido}
 */

/**
 * Carga completa de timeline al dispositivo ESP32
 * @param {object} timeline - Objeto timeline con estructura: {name, events, mp3_duration}
 * @param {string} timelineName - Nombre para guardar en el dispositivo
 */
async function uploadTimelineToDevice(timeline, timelineName = 'default-timeline') {
    if (!serialConnected || !serialWriter) {
        showNotification('❌ Dispositivo no conectado', 'error');
        return false;
    }

    try {
        showNotification(`📤 Enviando timeline "${timelineName}" al ESP32...`, 'info');
        
        // Preparar datos del timeline
        const timelineData = {
            name: timelineName,
            duration: timeline.duration || (timeline.events ? Math.max(...timeline.events.map(e => e.time)) : 0),
            events: timeline.events || [],
            version: '2.0'
        };

        // Serializar a JSON
        const jsonString = JSON.stringify(timelineData);
        console.log(`📊 Timeline size: ${(jsonString.length / 1024).toFixed(2)} KB`);

        // Comando en protocolo serial
        const command = `TIMELINE|UPLOAD|${jsonString}\n`;
        
        // Enviar en chunks si es muy grande
        if (command.length > 2048) {
            await sendLargeTimeline(timelineData, timelineName);
        } else {
            const encoder = new TextEncoder();
            await serialWriter.write(encoder.encode(command));
        }

        showNotification(`✅ Timeline "${timelineName}" enviado al ESP32`, 'success');
        console.log('✅ Timeline cargado en dispositivo');
        return true;

    } catch (error) {
        showNotification(`❌ Error: ${error.message}`, 'error');
        console.error('Error subiendo timeline:', error);
        return false;
    }
}

/**
 * Envía timelines grandes en múltiples chunks
 * @param {object} timelineData - Datos del timeline
 * @param {string} timelineName - Nombre del timeline
 */
async function sendLargeTimeline(timelineData, timelineName) {
    try {
        const encoder = new TextEncoder();
        const CHUNK_SIZE = 1024; // 1KB por chunk
        
        // Iniciar transferencia
        const startCmd = `TIMELINE|START|${timelineName}\n`;
        await serialWriter.write(encoder.encode(startCmd));
        await new Promise(resolve => setTimeout(resolve, 100));

        // Enviar datos en chunks
        const jsonString = JSON.stringify(timelineData);
        const chunks = [];
        
        for (let i = 0; i < jsonString.length; i += CHUNK_SIZE) {
            chunks.push(jsonString.slice(i, i + CHUNK_SIZE));
        }

        for (let i = 0; i < chunks.length; i++) {
            const chunkCmd = `TIMELINE|CHUNK|${i}|${chunks[i]}\n`;
            await serialWriter.write(encoder.encode(chunkCmd));
            
            // Progreso
            const progress = ((i + 1) / chunks.length) * 100;
            updateUploadProgressBar(progress);
            
            await new Promise(resolve => setTimeout(resolve, 50));
        }

        // Finalizar transferencia
        const endCmd = `TIMELINE|END|${chunks.length}\n`;
        await serialWriter.write(encoder.encode(endCmd));
        
        showNotification(`✅ Timeline grande enviado (${chunks.length} chunks)`, 'success');

    } catch (error) {
        console.error('Error en envío por chunks:', error);
        throw error;
    }
}

/**
 * Descarga la lista de timelines almacenados en el ESP32
 */
async function listDeviceTimelines() {
    if (!serialConnected || !serialWriter) {
        showNotification('❌ Dispositivo no conectado', 'error');
        return null;
    }

    try {
        console.log('📋 Solicitando lista de timelines...');
        
        const encoder = new TextEncoder();
        const command = `TIMELINE|LIST\n`;
        await serialWriter.write(encoder.encode(command));

        // Esperar respuesta
        const timelines = await waitForDeviceResponse('TIMELINE_LIST', 2000);
        
        if (timelines && timelines.length > 0) {
            showNotification(`✅ ${timelines.length} timeline(s) encontrado(s)`, 'success');
            return timelines;
        } else {
            showNotification('⚠️ No hay timelines en el dispositivo', 'warning');
            return [];
        }

    } catch (error) {
        showNotification(`❌ Error: ${error.message}`, 'error');
        console.error('Error listando timelines:', error);
        return null;
    }
}

/**
 * Ejecuta un timeline guardado en el ESP32
 * @param {string} timelineName - Nombre del timeline a ejecutar
 */
async function playDeviceTimeline(timelineName) {
    if (!serialConnected || !serialWriter) {
        showNotification('❌ Dispositivo no conectado', 'error');
        return false;
    }

    try {
        showNotification(`▶️ Ejecutando timeline "${timelineName}" en dispositivo...`, 'info');
        
        const encoder = new TextEncoder();
        const command = `TIMELINE|PLAY|${timelineName}\n`;
        await serialWriter.write(encoder.encode(command));

        console.log(`▶️ Reproduciendo: ${timelineName}`);
        showNotification(`✅ Timeline "${timelineName}" en reproducción`, 'success');
        return true;

    } catch (error) {
        showNotification(`❌ Error: ${error.message}`, 'error');
        return false;
    }
}

/**
 * Detiene la reproducción de timeline en el dispositivo
 */
async function stopDeviceTimeline() {
    if (!serialConnected || !serialWriter) {
        showNotification('❌ Dispositivo no conectado', 'error');
        return false;
    }

    try {
        const encoder = new TextEncoder();
        const command = `TIMELINE|STOP\n`;
        await serialWriter.write(encoder.encode(command));

        showNotification('⏹️ Timeline detenido', 'warning');
        return true;

    } catch (error) {
        showNotification(`❌ Error: ${error.message}`, 'error');
        return false;
    }
}

/**
 * Elimina un timeline del ESP32
 * @param {string} timelineName - Nombre del timeline a eliminar
 */
async function deleteDeviceTimeline(timelineName) {
    if (!serialConnected || !serialWriter) {
        showNotification('❌ Dispositivo no conectado', 'error');
        return false;
    }

    if (!confirm(`¿Eliminar timeline "${timelineName}"?`)) {
        return false;
    }

    try {
        const encoder = new TextEncoder();
        const command = `TIMELINE|DELETE|${timelineName}\n`;
        await serialWriter.write(encoder.encode(command));

        showNotification(`✅ Timeline "${timelineName}" eliminado`, 'success');
        console.log(`🗑️ Eliminado: ${timelineName}`);
        return true;

    } catch (error) {
        showNotification(`❌ Error: ${error.message}`, 'error');
        return false;
    }
}

/**
 * Descarga un timeline del ESP32 a la PC
 * @param {string} timelineName - Nombre del timeline a descargar
 */
async function downloadTimelineFromDevice(timelineName) {
    if (!serialConnected || !serialWriter) {
        showNotification('❌ Dispositivo no conectado', 'error');
        return false;
    }

    try {
        showNotification(`📥 Descargando timeline "${timelineName}"...`, 'info');
        
        const encoder = new TextEncoder();
        const command = `TIMELINE|DOWNLOAD|${timelineName}\n`;
        await serialWriter.write(encoder.encode(command));

        // Esperar respuesta con datos
        const timelineData = await waitForDeviceResponse('TIMELINE_DATA', 5000);
        
        if (timelineData) {
            // Descargar como JSON
            const json = JSON.stringify(timelineData, null, 2);
            const blob = new Blob([json], { type: 'application/json' });
            const url = URL.createObjectURL(blob);
            const a = document.createElement('a');
            a.href = url;
            a.download = `${timelineName}_backup.json`;
            a.click();

            showNotification(`✅ Timeline "${timelineName}" descargado`, 'success');
            return true;
        }

    } catch (error) {
        showNotification(`❌ Error: ${error.message}`, 'error');
        console.error('Error descargando timeline:', error);
        return false;
    }
}

/**
 * Utilidad: Espera respuesta del dispositivo con timeout
 * @param {string} responseType - Tipo de respuesta esperada
 * @param {number} timeoutMs - Timeout en ms
 */
function waitForDeviceResponse(responseType, timeoutMs = 2000) {
    return new Promise((resolve, reject) => {
        const timeoutId = setTimeout(() => {
            reject(new Error(`Timeout esperando respuesta: ${responseType}`));
        }, timeoutMs);

        // Crear listener temporal para respuestas
        const listener = (data) => {
            if (data && data.type === responseType) {
                clearTimeout(timeoutId);
                serialReader.releaseLock(); // Liberar reader
                resolve(data.payload);
            }
        };

        // TODO: Integrar con sistema de lectura serial existente
        // Por ahora, esto es un placeholder
    });
}

/**
 * Actualiza barra de progreso de subida
 * @param {number} percent - Porcentaje 0-100
 */
function updateUploadProgressBar(percent) {
    const progressBar = document.getElementById('progressFill');
    const progressContainer = document.getElementById('uploadProgress');
    
    if (progressBar && progressContainer) {
        progressContainer.classList.add('active');
        progressBar.style.width = percent + '%';
        progressBar.textContent = Math.round(percent) + '%';
    }
}

/**
 * Obtiene información de un timeline específico del dispositivo
 * @param {string} timelineName - Nombre del timeline
 */
async function getDeviceTimelineInfo(timelineName) {
    if (!serialConnected || !serialWriter) {
        return null;
    }

    try {
        const encoder = new TextEncoder();
        const command = `TIMELINE|INFO|${timelineName}\n`;
        await serialWriter.write(encoder.encode(command));

        const info = await waitForDeviceResponse('TIMELINE_INFO', 2000);
        return info;

    } catch (error) {
        console.error('Error obteniendo info:', error);
        return null;
    }
}

/**
 * Exporta timeline actual de la web como JSON descargable
 * @param {object} timeline - Timeline a exportar
 * @param {string} filename - Nombre del archivo
 */
function exportTimelineToFile(timeline, filename = 'timeline.json') {
    try {
        const json = JSON.stringify(timeline, null, 2);
        const blob = new Blob([json], { type: 'application/json' });
        const url = URL.createObjectURL(blob);
        const a = document.createElement('a');
        a.href = url;
        a.download = filename;
        a.click();
        URL.revokeObjectURL(url);
        
        showNotification(`💾 Timeline exportado: ${filename}`, 'success');
    } catch (error) {
        showNotification(`❌ Error exportando: ${error.message}`, 'error');
    }
}

/**
 * Importa timeline desde archivo JSON
 * @param {File} file - Archivo JSON
 */
async function importTimelineFromFile(file) {
    try {
        const text = await file.text();
        const timeline = JSON.parse(text);
        
        if (!timeline.events || !Array.isArray(timeline.events)) {
            throw new Error('Formato de timeline inválido');
        }

        showNotification(`📂 Timeline importado: ${timeline.name || 'sin nombre'}`, 'success');
        return timeline;

    } catch (error) {
        showNotification(`❌ Error importando: ${error.message}`, 'error');
        return null;
    }
}

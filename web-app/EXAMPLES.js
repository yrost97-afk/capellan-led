// ==================== QUICK START - EJEMPLOS DE USO ====================

/**
 * EJEMPLO 1: Reproducción simple en Web
 * Reproduce MP3 + timeline en el navegador
 */
async function ejemplo1_WebPlayback() {
    // 1. Cargar MP3
    const file = document.getElementById('mp3Input').files[0];
    const loaded = await loadMP3ForPlayback(file);
    
    if (!loaded) return;
    
    // 2. El timeline ya está creado (eventos arrastrando en UI)
    // timelineEvents = [
    //     {time: 0, type: 'color', r: 255, g: 0, b: 0},
    //     {time: 2000, type: 'effect', effectName: 'FIRE', ...}
    // ]
    
    // 3. Reproducir
    await playMP3WithTimeline();
    
    // 4. Controles
    // pauseMP3WithTimeline();
    // resumeMP3WithTimeline();
    // stopMP3WithTimeline();
}

/**
 * EJEMPLO 2: Enviar timeline al dispositivo
 * Guarda timeline en ESP32 para ejecutar sin web
 */
async function ejemplo2_SendToDevice() {
    if (!serialConnected) {
        showNotification('❌ Conecta el ESP32 primero', 'error');
        return;
    }
    
    // 1. Preparar datos
    const timeline = {
        name: 'mi-show-especial',
        duration: 15000,
        events: timelineEvents  // Del timeline en web
    };
    
    // 2. Subir al dispositivo
    const success = await uploadTimelineToDevice(timeline, 'mi-show-especial');
    
    if (success) {
        showNotification('✅ Timeline guardado en ESP32', 'success');
    }
}

/**
 * EJEMPLO 3: Listar y ejecutar timelines guardados
 * Gestionar shows guardados en el dispositivo
 */
async function ejemplo3_ManageDeviceTimelines() {
    if (!serialConnected) return;
    
    // 1. Listar todos los timelines
    const timelines = await listDeviceTimelines();
    
    if (!timelines || timelines.length === 0) {
        showNotification('⚠️ No hay timelines en el dispositivo', 'warning');
        return;
    }
    
    console.log('Timelines disponibles:');
    timelines.forEach(tl => {
        console.log(`  - ${tl.name}: ${(tl.duration/1000).toFixed(1)}s`);
    });
    
    // 2. Ejecutar uno
    const selectedTimeline = 'mi-show-especial';
    await playDeviceTimeline(selectedTimeline);
    showNotification(`▶️ Ejecutando: ${selectedTimeline}`, 'info');
    
    // 3. Después, detener
    // await stopDeviceTimeline();
    
    // 4. O eliminarlo
    // await deleteDeviceTimeline(selectedTimeline);
}

/**
 * EJEMPLO 4: Sincronización Web + Dispositivo
 * Lo más pro: reproduce en ambos simultáneamente
 */
async function ejemplo4_SyncPlayback() {
    if (!serialConnected) {
        showNotification('❌ Conecta el ESP32', 'error');
        return;
    }
    
    if (!currentMP3) {
        showNotification('❌ Carga un MP3 primero', 'error');
        return;
    }
    
    // 1. Un solo comando lo hace TODO:
    //    - Sube timeline al ESP32
    //    - Reproduce MP3 en navegador
    //    - Ejecuta timeline en dispositivo
    //    - SINCRONIZADO automáticamente
    
    await syncPlayOnDevice();
    
    // ¡Resultado: Luces + sonido en sync perfecto!
}

/**
 * EJEMPLO 5: Crear timeline programáticamente
 * En lugar de drag-and-drop, crear por código
 */
function ejemplo5_CreateTimelineByCode() {
    // Borrar timeline anterior
    timelineEvents = [];
    
    // Agregar eventos
    timelineEvents.push({
        time: 0,              // Inicio
        type: 'color',
        r: 255, g: 0, b: 0    // Rojo
    });
    
    timelineEvents.push({
        time: 2000,           // 2 segundos
        type: 'effect',
        effectName: 'STROBE',
        r: 255, g: 255, b: 255,
        speed: 100,
        duration: 2000
    });
    
    timelineEvents.push({
        time: 5000,           // 5 segundos
        type: 'effect',
        effectName: 'FIRE',
        speed: 30,
        duration: 3000
    });
    
    timelineEvents.push({
        time: 9000,           // 9 segundos
        type: 'brightness',
        level: 128
    });
    
    timelineEvents.push({
        time: 12000,          // 12 segundos
        type: 'color',
        r: 0, g: 100, b: 255  // Azul
    });
    
    console.log('✅ Timeline creado con 5 eventos');
}

/**
 * EJEMPLO 6: Exportar e importar timelines
 * Guardar/cargar desde archivos
 */
async function ejemplo6_ExportImport() {
    // EXPORTAR (descargar a PC)
    const timeline = {
        name: 'mi-show',
        duration: 15000,
        events: timelineEvents
    };
    
    exportTimelineToFile(timeline, 'mi-show.json');
    
    // IMPORTAR (cargar desde archivo)
    const fileInput = document.getElementById('timelineUpload');
    fileInput.onchange = async (e) => {
        const file = e.target.files[0];
        const imported = await importTimelineFromFile(file);
        
        if (imported) {
            timelineEvents = imported.events;
            showNotification('✅ Timeline importado', 'success');
        }
    };
}

/**
 * EJEMPLO 7: Callbacks en tiempo real
 * Hacer cosas mientras se reproduce
 */
function ejemplo7_RealTimeCallbacks() {
    // Registrar callback
    onTimelineEvent((event, currentTimeMs) => {
        const segundos = (currentTimeMs / 1000).toFixed(2);
        console.log(`[${segundos}s] Evento:`, event);
        
        // Ejemplo: Log por tipo
        if (event.type === 'color') {
            console.log(`  🎨 Color: RGB(${event.r},${event.g},${event.b})`);
        } else if (event.type === 'effect') {
            console.log(`  ✨ Efecto: ${event.effectName}`);
        }
        
        // Ejemplo: Enviar a servidor
        // fetch('/api/log-event', {
        //     method: 'POST',
        //     body: JSON.stringify({time: currentTimeMs, event})
        // });
    });
}

/**
 * EJEMPLO 8: Loop de reproducción continua
 * Repetir show indefinidamente
 */
async function ejemplo8_LoopPlayback() {
    let loopCount = 0;
    
    while (true) {
        loopCount++;
        console.log(`▶️ Loop ${loopCount} iniciado`);
        
        await playMP3WithTimeline();
        
        // Esperar a que termine
        const duration = audioBuffer ? audioBuffer.duration * 1000 : 15000;
        await new Promise(r => setTimeout(r, duration + 1000));
        
        // Repetir
    }
}

/**
 * EJEMPLO 9: Timeline dinámico (generado por música)
 * Crear eventos basados en BPM
 */
function ejemplo9_DynamicTimeline() {
    const BPM = 128;
    const beatDuration = (60 / BPM) * 1000;  // ms por beat
    
    timelineEvents = [];
    
    // Crear timeline con evento cada beat
    for (let i = 0; i < 8; i++) {  // 8 beats
        const time = i * beatDuration;
        
        // Alternar colores
        const color = i % 2 === 0 
            ? {r: 255, g: 0, b: 0}     // Rojo
            : {r: 0, g: 0, b: 255};    // Azul
        
        timelineEvents.push({
            time: Math.round(time),
            type: 'color',
            r: color.r,
            g: color.g,
            b: color.b
        });
    }
    
    console.log(`✅ Timeline dinámico: ${BPM} BPM, ${timelineEvents.length} eventos`);
}

/**
 * EJEMPLO 10: Depuración y monitoreo
 * Ver qué está pasando en tiempo real
 */
function ejemplo10_Debugging() {
    // Ver estado actual
    console.log('=== ESTADO ACTUAL ===');
    console.log('Audio cargado:', !!audioBuffer);
    console.log('Duración audio:', getAudioDuration(), 'segundos');
    console.log('Reproduciendo:', isAudioPlaying);
    console.log('Tiempo actual:', getCurrentPlaybackTime(), 'ms');
    console.log('Timeline eventos:', timelineEvents.length);
    console.log('Dispositivo conectado:', serialConnected);
    
    // Ver timeline
    console.log('\n=== TIMELINE ===');
    timelineEvents.forEach((event, i) => {
        console.log(`${i}: [${event.time}ms] ${event.type} -`, event);
    });
    
    // Ver audio stats
    if (audioBuffer) {
        console.log('\n=== AUDIO STATS ===');
        console.log('Canales:', audioBuffer.numberOfChannels);
        console.log('Sample rate:', audioBuffer.sampleRate, 'Hz');
        console.log('Duración:', audioBuffer.duration, 's');
    }
}

/**
 * EJEMPLO COMPLETO: Show profesional
 * Desde cero hasta reproducción con dispositivo
 */
async function ejemploCompleto_ProfessionalShow() {
    try {
        console.log('🎆 Iniciando show profesional...\n');
        
        // 1. Cargar MP3
        console.log('1️⃣ Cargando MP3...');
        const file = document.getElementById('mp3Input').files[0];
        await loadMP3ForPlayback(file);
        console.log('   ✅ MP3 cargado\n');
        
        // 2. Crear timeline (o usar el existente)
        console.log('2️⃣ Timeline con', timelineEvents.length, 'eventos\n');
        
        // 3. Preview en web
        console.log('3️⃣ Reproduciendo preview en web...');
        await playMP3WithTimeline();
        console.log('   ▶️ En reproducción (5 segundos de preview)\n');
        
        // Esperar 5 segundos
        await new Promise(r => setTimeout(r, 5000));
        
        // Parar preview
        pauseMP3WithTimeline();
        console.log('   ⏸️ Pausado\n');
        
        // 4. Conectar dispositivo
        console.log('4️⃣ Verificando conexión con ESP32...');
        if (!serialConnected) {
            console.log('   ❌ Conecta el ESP32 en la interfaz\n');
            return;
        }
        console.log('   ✅ Dispositivo conectado\n');
        
        // 5. Subir timeline
        console.log('5️⃣ Enviando timeline al dispositivo...');
        const success = await uploadTimelineToDevice(
            {name: 'show-profesional', duration: 15000, events: timelineEvents},
            'show-profesional'
        );
        console.log('   ✅ Timeline guardado\n');
        
        // 6. Reproducción sincronizada
        console.log('6️⃣ ¡INICIANDO SHOW SINCRONIZADO!\n');
        await syncPlayOnDevice();
        
        console.log('🎉 Show en reproducción - Luces + Sonido sincronizados!');
        
    } catch (error) {
        console.error('❌ Error:', error);
    }
}

// ==================== USO DESDE HTML ====================
/*

<!-- En index.html -->

<!-- Botones de ejemplo -->
<div style="padding: 20px; background: #f0f0f0; border-radius: 8px;">
    <h3>🧪 Ejemplos Rápidos</h3>
    <button onclick="ejemplo1_WebPlayback()">▶️ Reproducir en Web</button>
    <button onclick="ejemplo2_SendToDevice()">📤 Enviar al Dispositivo</button>
    <button onclick="ejemplo3_ManageDeviceTimelines()">📋 Listar Timelines</button>
    <button onclick="ejemplo4_SyncPlayback()">🔄 Sincronizar</button>
    <button onclick="ejemplo5_CreateTimelineByCode()">🎨 Crear Timeline</button>
    <button onclick="ejemplo6_ExportImport()">💾 Exportar/Importar</button>
    <button onclick="ejemplo7_RealTimeCallbacks()">📊 Callbacks</button>
    <button onclick="ejemplo8_LoopPlayback()">🔁 Loop</button>
    <button onclick="ejemplo9_DynamicTimeline()">🎵 Dinámico</button>
    <button onclick="ejemplo10_Debugging()">🐛 Debug</button>
    <button onclick="ejemploCompleto_ProfessionalShow()">🎆 Show Completo</button>
</div>

*/

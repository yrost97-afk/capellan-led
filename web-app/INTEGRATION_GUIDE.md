<!-- AGREGAR ESTA LÍNEA ANTES DEL CIERRE DE </head> -->
<!-- Cargar el módulo ejecutor de timeline -->
<script src="timeline-executor.js"></script>

<!-- MODIFICAR LA FUNCIÓN playTimeline() en el <script> existente: Línea 876 aproximadamente -->
<!-- CAMBIAR DE: -->
<!-- <button class="btn btn-primary" onclick="playTimeline()">▶️ Reproducir</button> -->
<!-- A: -->
<!-- <button class="btn btn-primary" onclick="playTimelineOnDevice(timelineEvents)">▶️ Reproducir</button> -->

<!-- CAMBIAR LA FUNCIÓN stopTimeline() DE: -->
<!-- <button class="btn btn-primary" onclick="stopTimeline()">⏹️ Detener</button> -->
<!-- A: -->
<!-- <button class="btn btn-primary" onclick="stopTimelineOnDevice()">⏹️ Detener</button> -->

<!-- ==================== FUNCIONES QUE DEBES REEMPLAZAR EN EL SCRIPT ====================

Busca y reemplaza estas funciones en el <script> del index.html:

// FUNCIÓN 1: Reemplaza playTimeline() existente
function playTimeline() {
    playTimelineOnDevice(timelineEvents);
}

// FUNCIÓN 2: Reemplaza stopTimeline() existente
function stopTimeline() {
    stopTimelineOnDevice();
}

// FUNCIÓN 3: Reemplaza dragColor() - Agregar evento de arrastre a timeline
function dragColor(event) {
    const color = document.getElementById('colorPicker').value;
    const rgb = hexToRgb(color);
    
    event.dataTransfer.effectAllowed = 'copy';
    event.dataTransfer.setData('application/json', JSON.stringify({
        type: 'color',
        r: rgb.r,
        g: rgb.g,
        b: rgb.b,
        time: getCurrentTimelinePosition(),
        duration: 1000
    }));
}

// FUNCIÓN 4: Reemplaza dragEffect() existente
function dragEffect(event, effectName, color) {
    event.dataTransfer.effectAllowed = 'copy';
    event.dataTransfer.setData('application/json', JSON.stringify({
        type: 'effect',
        effectName: effectName,
        r: color.r || 255,
        g: color.g || 255,
        b: color.b || 255,
        speed: 50,
        time: getCurrentTimelinePosition(),
        duration: 5000
    }));
}

// FUNCIÓN 5: Reemplaza dropEffect() existente
function dropEffect(event) {
    event.preventDefault();
    
    try {
        const data = JSON.parse(event.dataTransfer.getData('application/json'));
        
        // Calcular posición en el timeline
        const timelineTrack = document.getElementById('timelineTrack');
        const rect = timelineTrack.getBoundingClientRect();
        const x = event.clientX - rect.left;
        const percentPosition = x / rect.width;
        const timelineWidth = document.getElementById('timelineTrackInner').offsetWidth;
        const timePosition = (percentPosition * timelineWidth) / 10; // Convertir a milisegundos
        
        data.time = Math.max(0, timePosition);
        
        // Agregar evento al timeline
        timelineEvents.push(data);
        timelineEvents.sort((a, b) => a.time - b.time);
        
        renderTimeline();
        showNotification('✅ Evento agregado al timeline', 'success');
    } catch (error) {
        console.error('Error:', error);
        showNotification('❌ Error al agregar evento', 'error');
    }
}

// FUNCIÓN 6: Agregar función para renderizar timeline
function renderTimeline() {
    const timelineTrackInner = document.getElementById('timelineTrackInner');
    timelineTrackInner.innerHTML = '';
    
    timelineEvents.forEach((event, index) => {
        const eventEl = document.createElement('div');
        eventEl.className = 'timeline-event';
        eventEl.style.left = (event.time / 100) + 'px';
        eventEl.style.background = event.type === 'color' 
            ? `rgb(${event.r}, ${event.g}, ${event.b})`
            : 'linear-gradient(135deg, #667eea 0%, #764ba2 100%)';
        
        eventEl.innerHTML = `
            <div style="width: 100%; text-align: center;">
                ${event.effectName || 'Color'}<br/>
                <small>${Math.round(event.time)}ms</small>
            </div>
            <button class="timeline-event-delete" onclick="removeTimelineEvent(${index})">×</button>
        `;
        
        timelineTrackInner.appendChild(eventEl);
    });
}

// FUNCIÓN 7: Agregar función para eliminar eventos
function removeTimelineEvent(index) {
    timelineEvents.splice(index, 1);
    renderTimeline();
    showNotification('🗑️ Evento eliminado', 'warning');
}

// FUNCIÓN 8: Agregar función para obtener posición actual
function getCurrentTimelinePosition() {
    return timelineEvents.length > 0 
        ? Math.max(...timelineEvents.map(e => e.time)) + 2000 
        : 0;
}

// FUNCIÓN 9: Agregar hexToRgb si no existe
function hexToRgb(hex) {
    const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : { r: 255, g: 0, b: 0 };
}

// FUNCIÓN 10: Agregar showNotification si no existe
function showNotification(message, type = 'info') {
    const notification = document.createElement('div');
    notification.className = `notification ${type}`;
    notification.textContent = message;
    document.body.appendChild(notification);
    
    setTimeout(() => {
        notification.remove();
    }, 3000);
}

// FUNCIÓN 11: Agregar allowDrop
function allowDrop(event) {
    event.preventDefault();
    event.dataTransfer.dropEffect = 'copy';
}

// FUNCIÓN 12: Agregar applyColorFromPicker
function applyColorFromPicker() {
    const color = document.getElementById('colorPicker').value;
    const rgb = hexToRgb(color);
    
    const colorPreview = document.getElementById('colorPreview');
    colorPreview.style.background = color;
    
    const colorInfo = document.getElementById('colorInfo');
    colorInfo.textContent = `RGB(${rgb.r}, ${rgb.g}, ${rgb.b}) - ${color.toUpperCase()}`;
    
    showNotification('🎨 Color actualizado', 'info');
}

// FUNCIÓN 13: Agregar switchDevice
function switchDevice(device) {
    document.querySelectorAll('.device-tab').forEach(tab => tab.classList.remove('active'));
    event.target.classList.add('active');
    
    currentDeviceId = device;
    showNotification(`📱 Dispositivo: ${device.toUpperCase()}`, 'info');
}

// FUNCIÓN 14: Agregar downloadTimeline
function downloadTimeline() {
    if (timelineEvents.length === 0) {
        showNotification('⚠️ No hay eventos para descargar', 'warning');
        return;
    }
    
    const data = {
        version: '1.0',
        metadata: {
            title: 'Mi Timeline',
            device: currentDeviceId,
            created: new Date().toISOString()
        },
        events: timelineEvents
    };
    
    const json = JSON.stringify(data, null, 2);
    const blob = new Blob([json], { type: 'application/json' });
    const url = URL.createObjectURL(blob);
    const a = document.createElement('a');
    a.href = url;
    a.download = `timeline_${Date.now()}.json`;
    a.click();
    
    showNotification('💾 Timeline descargado', 'success');
}

// FUNCIÓN 15: Agregar uploadTimeline
function uploadTimeline(event) {
    const file = event.target.files[0];
    if (!file) return;
    
    const reader = new FileReader();
    reader.onload = (e) => {
        try {
            const data = JSON.parse(e.target.result);
            timelineEvents = data.events || [];
            renderTimeline();
            showNotification('📂 Timeline cargado correctamente', 'success');
        } catch (error) {
            showNotification('❌ Error al cargar archivo', 'error');
        }
    };
    reader.readAsText(file);
}

// FUNCIÓN 16: Agregar uploadTimelineToESP (placeholder)
function uploadTimelineToESP() {
    playTimelineOnDevice(timelineEvents);
}

// FUNCIÓN 17: Agregar listTimelinesESP (placeholder)
function listTimelinesESP() {
    showNotification('📋 Listando timelines...', 'info');
}

// FUNCIÓN 18: Agregar deleteTimelineESP (placeholder)
function deleteTimelineESP() {
    showNotification('🗑️ Función no implementada', 'warning');
}

// FUNCIÓN 19: Agregar runTimelineESP (placeholder)
function runTimelineESP() {
    playTimelineOnDevice(timelineEvents);
}

// FUNCIÓN 20: Agregar deleteSelectedTimeline (placeholder)
function deleteSelectedTimeline() {
    showNotification('🗑️ Función no implementada', 'warning');
}

==================== FIN DE LAS FUNCIONES ====================

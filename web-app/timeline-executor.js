// ==================== TIMELINE EXECUTOR - Envía efectos al ESP32 ====================

let serialWriter = null;
let serialConnected = false;

/**
 * Convierte los eventos del timeline a comandos del protocolo serial
 * Formato esperado por el firmware: COMANDO|PARAM1|PARAM2|...
 */
async function executeTimelineEvent(event) {
    if (!serialWriter) {
        showNotification('❌ Dispositivo no conectado', 'error');
        return false;
    }

    try {
        const encoder = new TextEncoder();
        let command = '';

        // Mapear tipos de eventos a comandos del firmware
        if (event.type === 'color') {
            command = `COLOR|${event.r}|${event.g}|${event.b}`;
        }
        else if (event.type === 'brightness') {
            command = `BRIGHTNESS|${event.level}`;
        }
        else if (event.type === 'effect') {
            // Mapear efectos a comandos específicos del firmware
            switch(event.effectName) {
                case 'STROBE':
                    command = `STROBE|${event.r}|${event.g}|${event.b}|${event.speed}|${event.duration}`;
                    break;
                case 'CHASE':
                    command = `CHASE|${event.r}|${event.g}|${event.b}|${event.speed}|${event.duration}`;
                    break;
                case 'FIRE':
                    command = `FIRE|${event.speed}|${event.duration}`;
                    break;
                case 'RAINBOW_CYCLE':
                    command = `RAINBOW_CYCLE|${event.speed}|${event.duration}`;
                    break;
                case 'BREATHE':
                    command = `BREATHE|${event.r}|${event.g}|${event.b}|${event.speed}|${event.duration}`;
                    break;
                case 'SPARKLE':
                    command = `SPARKLE|${event.r}|${event.g}|${event.b}|${event.speed}|${event.duration}`;
                    break;
                case 'OCEAN':
                    command = `COLOR|0|165|255`;
                    break;
                case 'FOREST':
                    command = `COLOR|34|139|34`;
                    break;
                case 'SUNSET':
                    command = `COLOR|255|140|0`;
                    break;
                case 'NEON':
                    command = `RAINBOW_CYCLE|100|5000`;
                    break;
                case 'GOLDEN':
                    command = `COLOR|255|215|0`;
                    break;
                default:
                    command = `COLOR|${event.r}|${event.g}|${event.b}`;
            }
        }
        else if (event.type === 'transition') {
            // Las transiciones se ejecutan como fade (cambio de color)
            command = `COLOR|${event.r}|${event.g}|${event.b}`;
        }

        if (command) {
            console.log('📤 Enviando:', command);
            await serialWriter.write(encoder.encode(command + '\n'));
            return true;
        }

    } catch (error) {
        console.error('Error enviando comando:', error);
        showNotification(`❌ Error: ${error.message}`, 'error');
        return false;
    }
}

/**
 * Reproduce el timeline completo en el ESP32
 */
async function playTimelineOnDevice(events) {
    if (!serialWriter) {
        showNotification('❌ Dispositivo no conectado', 'error');
        return;
    }

    if (!events || events.length === 0) {
        showNotification('⚠️ No hay eventos en el timeline', 'warning');
        return;
    }

    console.log('▶️ Iniciando reproducción del timeline...');
    showNotification('▶️ Reproduciendo timeline en dispositivo...', 'info');

    try {
        // Primero, apagar todos los LEDs
        const encoder = new TextEncoder();
        await serialWriter.write(encoder.encode('OFF\n'));
        await delay(500);

        // Reproducir cada evento con su retraso
        let currentTime = 0;

        for (let i = 0; i < events.length; i++) {
            const event = events[i];
            
            // Calcular tiempo de espera desde el evento anterior
            const waitTime = event.time - currentTime;
            
            if (waitTime > 0) {
                console.log(`⏱️ Esperando ${waitTime}ms...`);
                await delay(waitTime);
            }

            // Ejecutar el evento
            console.log(`🎬 Evento ${i + 1}/${events.length}:`, event);
            const success = await executeTimelineEvent(event);

            if (!success) {
                showNotification(`❌ Error en evento ${i + 1}`, 'error');
                break;
            }

            currentTime = event.time;
        }

        showNotification('✅ Timeline completado', 'success');
        console.log('✅ Timeline reproducido exitosamente');

    } catch (error) {
        console.error('Error reproduciendo timeline:', error);
        showNotification(`❌ Error: ${error.message}`, 'error');
    }
}

/**
 * Detiene la reproducción del timeline
 */
async function stopTimelineOnDevice() {
    if (!serialWriter) return;

    try {
        const encoder = new TextEncoder();
        await serialWriter.write(encoder.encode('OFF\n'));
        showNotification('⏹️ Timeline detenido', 'warning');
    } catch (error) {
        console.error('Error deteniendo timeline:', error);
    }
}

/**
 * Utility: espera un tiempo en ms
 */
function delay(ms) {
    return new Promise(resolve => setTimeout(resolve, ms));
}

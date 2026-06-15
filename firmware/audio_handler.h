// ==================== AUDIO HANDLER ====================
// Manejo de archivos MP3 y sincronización con timeline
// Requiere: FS.h, SD.h o LittleFS

#ifndef AUDIO_HANDLER_H
#define AUDIO_HANDLER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <vector>

// Estructura para keyframe de audio
struct AudioKeyframe {
    uint32_t timestamp_ms;  // Milisegundos desde el inicio del MP3
    uint8_t r, g, b;        // Color RGB
    uint8_t brightness;     // Brillo (0-255)
    String effect;          // Nombre del efecto
    uint32_t duration_ms;   // Duración del efecto
};

class AudioHandler {
private:
    File mp3File;
    File audioBuffer;
    uint32_t fileSize = 0;
    uint32_t currentPosition = 0;
    bool isPlaying = false;
    uint32_t startTime = 0;
    uint32_t pauseTime = 0;
    std::vector<AudioKeyframe> timeline;
    const int BUFFER_SIZE = 4096;  // 4KB buffer para lectura rápida
    const int CHUNK_SIZE = 1024;   // Enviar en chunks de 1KB
    
public:
    // ========== INICIALIZACIÓN ==========
    
    bool begin() {
        if (!LittleFS.begin()) {
            Serial.println("ERROR|LittleFS not available");
            return false;
        }
        return true;
    }
    
    // ========== CARGA DE MP3 ==========
    
    // Recibe MP3 en chunks desde serial
    bool receiveMP3Chunked(const char* filename, size_t totalSize) {
        File f = LittleFS.open(filename, "w");
        if (!f) {
            Serial.println("ERROR|Cannot create MP3 file");
            return false;
        }
        
        size_t received = 0;
        uint8_t buffer[256];
        unsigned long lastUpdate = millis();
        
        Serial.println(String("OK|Ready for MP3 upload: ") + filename);
        
        while (received < totalSize) {
            if (Serial.available()) {
                int bytesRead = Serial.readBytes(buffer, min(256, (int)(totalSize - received)));
                f.write(buffer, bytesRead);
                received += bytesRead;
                
                // Mostrar progreso cada 256KB
                if (millis() - lastUpdate > 1000) {
                    int progress = (received * 100) / totalSize;
                    Serial.println(String("PROGRESS|") + progress);
                    lastUpdate = millis();
                }
            }
            yield();  // Permite que el watchdog no resetee
        }
        
        f.close();
        fileSize = received;
        Serial.println("OK|MP3 upload complete");
        return true;
    }
    
    // Carga timeline JSON
    bool loadTimeline(const char* jsonPayload) {
        timeline.clear();
        
        // Parser JSON simple (optimizado para ESP)
        String json = jsonPayload;
        int keyframeCount = 0;
        
        // Buscar patrón: "keyframes":[...]
        int start = json.indexOf("[");
        int end = json.lastIndexOf("]");
        
        if (start == -1 || end == -1) {
            Serial.println("ERROR|Invalid JSON format");
            return false;
        }
        
        String frames = json.substring(start + 1, end);
        
        // Parsear cada keyframe
        int pos = 0;
        while (pos < frames.length()) {
            // Buscar objeto: {
            int objStart = frames.indexOf("{", pos);
            int objEnd = frames.indexOf("}", objStart);
            
            if (objStart == -1 || objEnd == -1) break;
            
            String obj = frames.substring(objStart, objEnd + 1);
            
            // Extraer valores
            uint32_t ts = extractJsonInt(obj, "timestamp");
            uint8_t r = extractJsonInt(obj, "r");
            uint8_t g = extractJsonInt(obj, "g");
            uint8_t b = extractJsonInt(obj, "b");
            uint8_t bright = extractJsonInt(obj, "brightness");
            String eff = extractJsonString(obj, "effect");
            uint32_t dur = extractJsonInt(obj, "duration");
            
            timeline.push_back({ts, r, g, b, bright, eff, dur});
            keyframeCount++;
            pos = objEnd + 1;
        }
        
        Serial.println(String("OK|Timeline loaded with ") + keyframeCount + " keyframes");
        return true;
    }
    
    // ========== REPRODUCCIÓN ==========
    
    void play() {
        startTime = millis();
        currentPosition = 0;
        isPlaying = true;
        Serial.println("OK|MP3 playback started");
    }
    
    void pause() {
        pauseTime = millis();
        isPlaying = false;
        Serial.println("OK|MP3 playback paused");
    }
    
    void resume() {
        startTime = millis() - (pauseTime - startTime);
        isPlaying = true;
        Serial.println("OK|MP3 playback resumed");
    }
    
    void stop() {
        isPlaying = false;
        currentPosition = 0;
        startTime = 0;
        Serial.println("OK|MP3 playback stopped");
    }
    
    uint32_t getCurrentTime() {
        if (!isPlaying) return pauseTime - startTime;
        return millis() - startTime;
    }
    
    // ========== SINCRONIZACIÓN CON TIMELINE ==========
    
    // Obtiene el keyframe actual según tiempo de reproducción
    AudioKeyframe* getCurrentKeyframe() {
        uint32_t currentTime = getCurrentTime();
        
        for (int i = timeline.size() - 1; i >= 0; i--) {
            if (timeline[i].timestamp_ms <= currentTime) {
                return &timeline[i];
            }
        }
        return nullptr;
    }
    
    // Próximo keyframe
    AudioKeyframe* getNextKeyframe() {
        uint32_t currentTime = getCurrentTime();
        
        for (int i = 0; i < timeline.size(); i++) {
            if (timeline[i].timestamp_ms > currentTime) {
                return &timeline[i];
            }
        }
        return nullptr;
    }
    
    // Tiempo hasta próximo keyframe (ms)
    uint32_t getTimeToNextKeyframe() {
        AudioKeyframe* next = getNextKeyframe();
        if (!next) return UINT32_MAX;
        
        uint32_t currentTime = getCurrentTime();
        return next->timestamp_ms - currentTime;
    }
    
    // ========== ENVÍO DE DATOS OPTIMIZADO ==========
    
    // Envía datos del MP3 en chunks para lectura rápida
    bool streamMP3Chunk(uint32_t position, uint32_t chunkSize) {
        File f = LittleFS.open("/mp3.bin", "r");
        if (!f) return false;
        
        f.seek(position);
        uint8_t buffer[CHUNK_SIZE];
        size_t bytesRead = f.readBytes((char*)buffer, min(chunkSize, (uint32_t)CHUNK_SIZE));
        
        // Enviar header binario (más rápido que texto)
        Serial.write(0xFF);  // Marcador
        Serial.write((bytesRead >> 8) & 0xFF);
        Serial.write(bytesRead & 0xFF);
        Serial.write(buffer, bytesRead);
        
        f.close();
        return bytesRead > 0;
    }
    
    // ========== ESTADO ==========
    
    String getStatus() {
        String status = "OK|";
        status += "Playing:" + String(isPlaying ? "1" : "0");
        status += "|Time:" + String(getCurrentTime());
        status += "|Size:" + String(fileSize);
        status += "|Keyframes:" + String(timeline.size());
        return status;
    }
    
    // ========== UTILIDADES INTERNAS ==========
    
private:
    uint32_t extractJsonInt(const String& json, const char* key) {
        String searchKey = String("\"") + key + "\":";
        int pos = json.indexOf(searchKey);
        if (pos == -1) return 0;
        
        pos += searchKey.length();
        String numStr = "";
        while (pos < json.length() && isdigit(json[pos])) {
            numStr += json[pos];
            pos++;
        }
        return numStr.toInt();
    }
    
    String extractJsonString(const String& json, const char* key) {
        String searchKey = String("\"") + key + "\":\"";
        int pos = json.indexOf(searchKey);
        if (pos == -1) return "";
        
        pos += searchKey.length();
        int endPos = json.indexOf("\"", pos);
        return json.substring(pos, endPos);
    }
};

#endif

import React, { useState, useEffect } from 'react';
import './App.css';

const { ipcRenderer } = window.require('electron');

function App() {
  const [currentTab, setCurrentTab] = useState('dashboard');
  const [appPath, setAppPath] = useState('');
  const [timelineData, setTimelineData] = useState(null);
  const [mp3File, setMp3File] = useState(null);

  useEffect(() => {
    ipcRenderer.invoke('get-app-path').then(path => setAppPath(path));

    // Listeners para menú
    ipcRenderer.on('new-project', handleNewProject);
    ipcRenderer.on('open-project', handleOpenProject);
    ipcRenderer.on('save-project', handleSaveProject);
    ipcRenderer.on('download-firmware', handleDownloadFirmware);
    ipcRenderer.on('open-serial', handleOpenSerial);

    return () => {
      ipcRenderer.removeAllListeners();
    };
  }, []);

  const handleNewProject = () => setCurrentTab('timeline');
  const handleOpenProject = () => {
    ipcRenderer.invoke('load-timeline').then(result => {
      if (result.success) {
        setTimelineData(result.data);
        setCurrentTab('timeline');
      }
    });
  };
  const handleSaveProject = () => {
    if (timelineData) {
      ipcRenderer.invoke('save-timeline', timelineData).then(result => {
        if (result.success) {
          alert(`✅ Proyecto guardado en:\n${result.path}`);
        }
      });
    }
  };
  const handleDownloadFirmware = () => {
    ipcRenderer.invoke('download-firmware', '3.0').then(result => {
      if (result.success) {
        alert(`✅ Firmware descargado`);
      }
    });
  };
  const handleOpenSerial = () => {
    ipcRenderer.invoke('open-serial-monitor', 'COM3').then(result => {
      if (result.success) {
        alert('✅ Monitor Serial abierto');
      }
    });
  };

  const handleUploadMp3 = () => {
    ipcRenderer.invoke('upload-mp3').then(result => {
      if (result.success) {
        setMp3File({ path: result.path, size: result.size });
        alert(`✅ MP3 seleccionado: ${Math.round(result.size / 1024 / 1024 * 100) / 100} MB`);
      }
    });
  };

  return (
    <div className="app">
      <header className="app-header">
        <h1>🎆 Capellán LED Pro Installer</h1>
        <p>Control profesional de LEDs con timeline sincronizado a MP3</p>
      </header>

      <nav className="app-nav">
        <button 
          className={`nav-btn ${currentTab === 'dashboard' ? 'active' : ''}`}
          onClick={() => setCurrentTab('dashboard')}
        >
          📊 Panel de Control
        </button>
        <button 
          className={`nav-btn ${currentTab === 'timeline' ? 'active' : ''}`}
          onClick={() => setCurrentTab('timeline')}
        >
          🎬 Timeline Editor
        </button>
        <button 
          className={`nav-btn ${currentTab === 'firmware' ? 'active' : ''}`}
          onClick={() => setCurrentTab('firmware')}
        >
          ⚙️ Firmware
        </button>
        <button 
          className={`nav-btn ${currentTab === 'help' ? 'active' : ''}`}
          onClick={() => setCurrentTab('help')}
        >
          ❓ Ayuda
        </button>
      </nav>

      <main className="app-content">
        {currentTab === 'dashboard' && <DashboardTab appPath={appPath} />}
        {currentTab === 'timeline' && <TimelineTab timelineData={timelineData} setTimelineData={setTimelineData} mp3File={mp3File} handleUploadMp3={handleUploadMp3} />}
        {currentTab === 'firmware' && <FirmwareTab />}
        {currentTab === 'help' && <HelpTab />}
      </main>
    </div>
  );
}

function DashboardTab({ appPath }) {
  return (
    <div className="tab-content">
      <h2>🎆 Bienvenido a Capellán LED</h2>
      <div className="dashboard-grid">
        <div className="card">
          <h3>📍 Ubicación de Instalación</h3>
          <code>{appPath}</code>
        </div>
        <div className="card">
          <h3>🚀 Inicio Rápido</h3>
          <ol>
            <li>Descarga el firmware</li>
            <li>Carga en tu ESP32/ESP8266</li>
            <li>Crea timelines con MP3</li>
            <li>Controla desde aquí</li>
          </ol>
        </div>
        <div className="card">
          <h3>✨ Características</h3>
          <ul>
            <li>✅ Soporte MP3 completo</li>
            <li>✅ 1000 eventos por timeline</li>
            <li>✅ Descarga rápida sin errores</li>
            <li>✅ 12+ efectos LED</li>
          </ul>
        </div>
      </div>
    </div>
  );
}

function TimelineTab({ timelineData, setTimelineData, mp3File, handleUploadMp3 }) {
  return (
    <div className="tab-content">
      <h2>🎬 Editor de Timeline</h2>
      <div className="timeline-editor">
        <div className="editor-panel">
          <h3>Archivo de Audio</h3>
          <button className="btn btn-primary" onClick={handleUploadMp3}>
            📁 Seleccionar MP3
          </button>
          {mp3File && (
            <div className="mp3-info">
              <p>✅ {mp3File.path}</p>
              <small>{Math.round(mp3File.size / 1024 / 1024 * 100) / 100} MB</small>
            </div>
          )}
        </div>

        <div className="editor-panel">
          <h3>Timeline de Eventos</h3>
          <div className="timeline-track">
            <p>💡 Arrastra efectos de colores aquí para sincronizar con la música</p>
            <div className="timeline-placeholder">
              [Timeline Visual aquí]
            </div>
          </div>
        </div>

        <div className="editor-controls">
          <button className="btn btn-success">▶️ Vista Previa</button>
          <button className="btn btn-primary">💾 Guardar Timeline</button>
          <button className="btn btn-warning">📤 Subir a ESP32</button>
        </div>
      </div>
    </div>
  );
}

function FirmwareTab() {
  return (
    <div className="tab-content">
      <h2>⚙️ Gestor de Firmware</h2>
      <div className="firmware-section">
        <div className="card">
          <h3>📥 Descargar Firmware</h3>
          <p>Versión actual: v3.0 (MP3 Enhanced)</p>
          <button className="btn btn-success" onClick={() => {
            window.require('electron').ipcRenderer.invoke('download-firmware', '3.0');
          }}>
            📥 Descargar v3.0
          </button>
        </div>

        <div className="card">
          <h3>💻 Monitor Serial</h3>
          <p>Conecta a 115200 baud</p>
          <button className="btn btn-primary" onClick={() => {
            window.require('electron').ipcRenderer.invoke('open-serial-monitor', 'COM3');
          }}>
            🖥️ Abrir Monitor
          </button>
        </div>

        <div className="card">
          <h3>📋 Dispositivos Soportados</h3>
          <ul>
            <li>✅ ESP32-C3</li>
            <li>✅ ESP32-WROOM-32</li>
            <li>✅ ESP32-S3</li>
            <li>✅ ESP8266 D1 Mini</li>
          </ul>
        </div>
      </div>
    </div>
  );
}

function HelpTab() {
  return (
    <div className="tab-content">
      <h2>❓ Ayuda y Documentación</h2>
      <div className="help-section">
        <h3>🔧 Instalación Rápida</h3>
        <ol>
          <li>Conecta tu ESP32/ESP8266 por USB</li>
          <li>Ve a Firmware → Descargar</li>
          <li>Abre el Monitor Serial</li>
          <li>Copia el firmware en Arduino IDE</li>
          <li>Carga en tu dispositivo</li>
        </ol>

        <h3>🎵 Usando MP3</h3>
        <ol>
          <li>Selecciona un archivo MP3</li>
          <li>Crea eventos de timeline</li>
          <li>Sincroniza con la música</li>
          <li>Sube a tu ESP32</li>
          <li>¡Disfruta el show!</li>
        </ol>

        <h3>📚 Documentación Completa</h3>
        <p>
          Visita{' '}
          <a href="https://github.com/yrost97-afk/capellan-led#readme" target="_blank" rel="noopener noreferrer">
            GitHub Capellán LED
          </a>
        </p>
      </div>
    </div>
  );
}

export default App;

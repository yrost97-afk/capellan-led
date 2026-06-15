const { contextBridge, ipcRenderer } = require('electron');

contextBridge.exposeInMainWorld('electron', {
  ipcRenderer: {
    invoke: (channel, ...args) => ipcRenderer.invoke(channel, ...args),
    on: (channel, listener) => ipcRenderer.on(channel, listener),
    send: (channel, ...args) => ipcRenderer.send(channel, ...args),
    removeAllListeners: () => ipcRenderer.removeAllListeners()
  }
});

contextBridge.exposeInMainWorld('api', {
  getAppPath: () => ipcRenderer.invoke('get-app-path'),
  getFirmwarePath: () => ipcRenderer.invoke('get-firmware-path'),
  getDocumentsPath: () => ipcRenderer.invoke('get-documents-path'),
  downloadFirmware: (version) => ipcRenderer.invoke('download-firmware', version),
  saveTimeline: (data) => ipcRenderer.invoke('save-timeline', data),
  loadTimeline: () => ipcRenderer.invoke('load-timeline'),
  uploadMP3: () => ipcRenderer.invoke('upload-mp3'),
  openSerialMonitor: (port) => ipcRenderer.invoke('open-serial-monitor', port)
});

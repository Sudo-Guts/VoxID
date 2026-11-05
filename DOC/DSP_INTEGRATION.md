# Integracion DSP en Ophyra

Los ocho modulos compartidos son `frame`, `window`, `fft`, `power`, `mel`, `log_mel`, `dct` y `mfcc`. STM32CubeIDE los compila como archivos fisicos dentro de `Core/Src` y `Core/Inc`, sin recursos enlazados.

`C0DE/voxid` contiene unicamente el proyecto de firmware. Las versiones anteriores quedan en el historial Git.

`voxid_voice.c` integra el adaptador MFCC, sus buffers estaticos, la conversion ADC a PCM y la media de frames seleccionados por energia. `voxid_voice.h` declara tanto las funciones de captura como las funciones existentes `voxid_dsp_*`; sus nombres y contratos se conservan. La energia se consulta inmediatamente despues de MFCC_READY y usa el mismo frame PCM. La fase 2 retira los archivos separados `voxid_dsp.c` y `voxid_dsp.h` del firmware principal.

`voxid_config.h` concentra 16 kHz, captura de 2.5 segundos, frame 400, avance 160, FFT 512, 26 filtros Mel, 13 MFCC y umbral RMS de seleccion.

El firmware integra estos caminos:

- LOCUTORES permite seleccionar un espacio, abrirlo con OK y recorrer sus tres referencias normalizadas C01..C12 con UP/DN, sin modificar el DSP ni la EEPROM.
- IDENTIFICAR adquiere audio, calcula MFCC y compara los perfiles con distancia normalizada y rechazo por margen.
- REGISTRAR acepta tres tomas por locutor y guarda sus prototipos mediante voxid_enroll y voxid_storage. El arranque recupera los perfiles EEPROM en RAM.

CubeIDE compila `Core` y `Drivers` tanto en Debug como en Release. La entrega no incluye infraestructura de pruebas de PC.

Importar `C0DE/voxid`, refrescar con F5 y ejecutar Clean/Build en STM32CubeIDE. La compilacion ARM y la captura fisica de la version de mantenimiento quedan pendientes.


La normalizacion y persistencia se describen en [etapa 4](STAGE4_STORAGE_IDENTIFY.md). I2C se utiliza al arrancar y al confirmar un perfil, siempre fuera de captura.

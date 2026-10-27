# Integración DSP en Ophyra

Los ocho módulos compartidos son `frame`, `window`, `fft`, `power`, `mel`, `log_mel`, `dct` y `mfcc`. STM32CubeIDE los enlaza desde `C0DE/src` y `C0DE/inc`, sin copiarlos.

`C0DE/src/voxid.c` contiene el punto de entrada de comprobación para PC y no se enlaza en el firmware.

El adaptador `voxid_dsp.c` reserva buffers estáticos y ofrece inicialización, alimentación PCM, consulta de coeficientes y energía del último frame. La energía se consulta inmediatamente después de MFCC_READY y usa el mismo frame PCM.

`voxid_config.h` concentra 16 kHz, captura de tres segundos, frame 400, avance 160, FFT 512, 26 filtros Mel, 13 MFCC y umbral RMS de selección.

El firmware integra estos caminos:

- LOCUTORES consulta el estado de cada espacio y grafica las tres referencias normalizadas C01..C12, sin modificar el DSP ni la EEPROM.
- IDENTIFICAR adquiere audio, calcula MFCC y compara los perfiles con distancia normalizada y rechazo por margen.
- REGISTRAR acepta tres tomas por locutor y guarda sus prototipos mediante voxid_enroll y voxid_storage. El arranque recupera los perfiles EEPROM en RAM.

La antigua autoprueba de la UI fue retirada con DIAGNOSTICO. Se conservan la prueba portable `test_mfcc.c` y el proyecto de benchmark `voxid_test`.

Mantener la estructura relativa del checkout, refrescar con F5 y ejecutar Clean/Build en STM32CubeIDE. La compilación ARM y la captura física de la versión de mantenimiento quedan pendientes.


La normalización y persistencia se describen en [etapa 4](STAGE4_STORAGE_IDENTIFY.md). I2C se utiliza al arrancar y al confirmar un perfil, siempre fuera de captura.

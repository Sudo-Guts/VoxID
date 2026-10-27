# Integración DSP en Ophyra

Los ocho módulos compartidos son `frame`, `window`, `fft`, `power`, `mel`, `log_mel`, `dct` y `mfcc`. STM32CubeIDE los enlaza desde `C0DE/src` y `C0DE/inc`, sin copiarlos.

`C0DE/src/voxid.c` contiene el punto de entrada de comprobación para PC y no se enlaza en el firmware.

El adaptador `voxid_dsp.c` reserva buffers estáticos y ofrece inicialización, alimentación PCM, consulta de coeficientes y energía del último frame. La energía se consulta inmediatamente después de MFCC_READY y usa el mismo frame PCM.

`voxid_config.h` concentra 16 kHz, captura de tres segundos, frame 400, avance 160, FFT 512, 26 filtros Mel, 13 MFCC y umbral RMS de selección.

El firmware integra dos caminos de comprobación:

- DIAGNOSTICO ejecuta la señal sintética de 440 Hz sin ADC.
- IDENTIFICAR y REGISTRAR permiten adquirir audio y calcular MFCC; no identifican ni almacenan perfiles todavía.

Mantener la estructura relativa del checkout, refrescar con F5 y ejecutar Clean/Build en STM32CubeIDE. La compilación ARM y la captura física de la versión de mantenimiento quedan pendientes.


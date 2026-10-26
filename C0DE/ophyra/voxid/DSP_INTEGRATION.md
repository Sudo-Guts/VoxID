# Integracion DSP en Ophyra (proyecto voxid)

Los modulos compartidos estan en `C0DE/src` y `C0DE/inc`.
La configuracion de Eclipse (`.project`) enlaza **ocho archivos .c** y **ocho archivos .h** hacia `Core/Src` y `Core/Inc`, sin copiarlos.

Los modulos son: `frame`, `window`, `fft`, `power`, `mel`, `log_mel`, `dct` y `mfcc`.

El archivo `C0DE/src/voxid.c` **no se enlaza** porque contiene otro `main()`.

## Adaptador STM32

`Core/Inc/voxid_dsp.h` y `Core/Src/voxid_dsp.c` reservan buffers estaticos para el pipeline MFCC y ofrecen:

- `voxid_dsp_init()`: configura el extractor al arrancar el firmware.
- `voxid_dsp_is_ready()`: estado de inicializacion consultado desde diagnostico.
- `voxid_dsp_push(sample)`: alimenta una muestra PCM de 16 bits y retorna `MFCC_WAITING`, `MFCC_READY` o `MFCC_ERROR`.
- `voxid_dsp_coefficients()`: vector de 13 coeficientes cuando el resultado es `MFCC_READY`.

Configuracion reutilizada de `voxid_test`:
16 kHz, frame 400, hop 160, FFT 512, 26 filtros Mel, 13 MFCC.

### STM32CubeIDE

1. Importar el proyecto desde `C0DE/ophyra/voxid` (en el checkout completo del repositorio).
2. La ruta de compilacion `${ProjDirPath}/../../inc` ya esta configurada tanto en **Debug** como **Release** dentro de `.cproject`. Actualizar los recursos con **F5**. Verificar que `Core/Src` y `Core/Inc` muestren los archivos DSP enlazados.
3. Ejecutar **Project > Clean** y **Build (Debug)**; confirmar que se compilan `frame.c`, `fft.c`, `mfcc.c`, etc.
4. Programar la Ophyra y abrir **DIAGNOSTICO**: debe mostrar `MFCC: LISTO` si la inicializacion termino bien.

Nota: este cambio **no activa ADC, TIM2 ni DMA**; solo prepara e inicializa el procesamiento. `MFCC: LISTO` no prueba todavia captura ni procesamiento en tiempo real.

**Importante:** mantener juntos `C0DE/src`, `C0DE/inc` y `C0DE/ophyra/voxid` con esta estructura relativa. Abrir una copia aislada de `voxid` rompera los enlaces.

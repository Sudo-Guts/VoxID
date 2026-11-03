# Validacion de etapas 1 y 2

Esta revision deja implementadas adquisicion y extraccion MFCC. El cierre fisico requiere compilar con el toolchain ARM y medir en Ophyra.

## Comprobaciones ya realizadas en PC

- Las once pruebas ejecutadas con `-Wall -Wextra -Wpedantic -Werror` pasan: Frame, ventana, FFT, MFCC, voz, audio, interfaz, GFX, registro, persistencia e identificacion.
- Las pruebas de voz procesan 40 000 muestras por toma y exigen 248 frames. Cubren silencio centrado, offset constante, senal debil, tono e intermitencia.
- La simulacion de HAL comprueba estadisticas, repeticion, cancelacion, sobreescritura, errores de callback y fallos de arranque.
- GFX se comprueba sobre memoria, incluido texto con fondo y coordenadas negativas.
- La UI real se ejercita con dibujo simulado: repetir desde resultado/error, OK inactivo, BACK y circulos fijos.
- La comprobacion de sintaxis con cabeceras STM32 del repositorio no produce un binario ARM ni valida enlazado o tiempos.

El criterio de energia selecciona candidatos a voz y no reconoce MARIPOSA. La comparacion de perfiles de la version actual se describe en [etapa 4](../C0DE/ophyra/voxid/STAGE4_STORAGE_IDENTIFY.md).

## Preparacion en STM32CubeIDE

1. Abrir el checkout completo e importar `C0DE/ophyra/voxid`.
2. F5, Clean y Build Debug. Confirmar que se compilan `voxid_audio.c`, `voxid_voice.c` y los ocho modulos DSP.
3. Programar la placa y comprobar el acceso a IDENTIFICAR, REGISTRAR y LOCUTORES. La integracion sintetica de tres frames se comprueba en PC con `make -C C0DE test test_mfcc.c`.

Si se regenera con CubeMX, revisar `MX_DMA_Init` antes de `MX_ADC1_Init`, DMA2 Stream0 Channel0 circular y el enlace `__HAL_LINKDMA`.

## Captura de nivel constante

Aplicar a PA2 un nivel conocido dentro de 0 a VDDA, por ejemplo cercano a 1.65 V con VDDA de 3.3 V, y compartir GND con la fuente.

| Comprobacion | Resultado esperado |
|---|---|
| Preparado | Circulos animados, OK y BACK disponibles. |
| Captura | Circulos fijos, cuenta de 3 a 1, OK inactivo y BACK disponible. |
| Resultado | En IDENTIFICAR, UP/DN recorre comparacion, grafica y estadisticas ADC. La tercera pagina muestra 248/248 frames, minimo, maximo y promedio. |
| Seleccion | CON VOZ cercano a cero en una entrada DC estable y con poco ruido. |
| Grafica | Si CON VOZ es cero, mensaje SIN VOZ DETECTADA. |
| Repetir | Nuevos circulos fijos y cuenta inicial de 2.5 segundos, sin tarjeta anterior. |

En la prueba sintetica de PC, una entrada DC ideal produce cero frames seleccionados. En la placa puede haber ruido, variacion de referencia o interferencia; registrar lo observado.

## Captura con la etapa analogica

1. Medir alimentacion, offset y excursion de salida antes de conectar el ADC.
2. Capturar pronunciaciones de MARIPOSA y registrar minimo, maximo, promedio y cantidad CON VOZ.
3. Repetir la toma y comprobar que no aparecen errores ni datos anteriores.
4. Cancelar con BACK y verificar que se puede iniciar una toma nueva.
5. Consultar LOCUTORES despues de registrar audio real y verificar que se muestran las referencias guardadas sin iniciar otra captura.

No se reserva un buffer de 2.5 segundos: se reciben mitades de 800 muestras y cada una dispone de 50 ms antes de que DMA necesite reutilizarla. La deteccion de sobreescritura rechaza una toma; medir el consumo real para comprobar el margen.

## Ajuste del criterio de energia

`Core/Inc/voxid_config.h` define `VOXID_VOICE_MIN_RMS = 0.01f`, normalizado a escala completa PCM. Comparar tomas de ruido ambiente y voz con el mismo ajuste de ganancia. Modificar el umbral solo con esas observaciones y repetir las pruebas.

Un nivel energetico suficiente no distingue voz de golpes o ruido fuerte. Una toma se usa como prototipo solo tras cumplir el criterio de registro y ser aceptada explicitamente. La comparacion requiere calibracion con voces reales.

## Cierre antes de etapa 3

Registrar version compilada, condiciones de entrada, capturas repetidas, comportamiento de cancelacion y ausencia de errores DMA. La etapa 3 ya tiene implementacion de registro y su [propia validacion](../C0DE/ophyra/voxid/STAGE3_ENROLLMENT.md); el cierre fisico de ambas comprobaciones sigue siendo necesario.

La persistencia, identificacion y sus comprobaciones fisicas pendientes se detallan en [etapa 4](../C0DE/ophyra/voxid/STAGE4_STORAGE_IDENTIFY.md).

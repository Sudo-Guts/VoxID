# Validación de etapas 1 y 2

Esta revisión deja implementadas adquisición y extracción MFCC. El cierre físico requiere compilar con el toolchain ARM y medir en Ophyra.

## Comprobaciones ya realizadas en PC

- Las once pruebas ejecutadas con `-Wall -Wextra -Wpedantic -Werror` pasan: Frame, ventana, FFT, MFCC, voz, audio, interfaz, GFX, registro, persistencia e identificación.
- Las pruebas de voz procesan 48 000 muestras por toma y exigen 298 frames. Cubren silencio centrado, offset constante, señal débil, tono e intermitencia.
- La simulación de HAL comprueba estadísticas, repetición, cancelación, sobreescritura, errores de callback y fallos de arranque.
- GFX se comprueba sobre memoria, incluido texto con fondo y coordenadas negativas.
- La UI real se ejercita con dibujo simulado: repetir desde resultado/error, OK inactivo, BACK y círculos fijos.
- La comprobación de sintaxis con cabeceras STM32 del repositorio no produce un binario ARM ni valida enlazado o tiempos.

El criterio de energía selecciona candidatos a voz y no reconoce MARIPOSA. La comparación de perfiles de la versión actual se describe en [etapa 4](../C0DE/ophyra/voxid/STAGE4_STORAGE_IDENTIFY.md).

## Preparación en STM32CubeIDE

1. Abrir el checkout completo e importar `C0DE/ophyra/voxid`.
2. F5, Clean y Build Debug. Confirmar que se compilan `voxid_audio.c`, `voxid_voice.c` y los ocho módulos DSP.
3. Programar la placa y comprobar el acceso a IDENTIFICAR, REGISTRAR y LOCUTORES. La integración sintética de tres frames se comprueba en PC con `make -C C0DE test test_mfcc.c`.

Si se regenera con CubeMX, revisar `MX_DMA_Init` antes de `MX_ADC1_Init`, DMA2 Stream0 Channel0 circular y el enlace `__HAL_LINKDMA`.

## Captura de nivel constante

Aplicar a PA2 un nivel conocido dentro de 0 a VDDA, por ejemplo cercano a 1.65 V con VDDA de 3.3 V, y compartir GND con la fuente.

| Comprobación | Resultado esperado |
|---|---|
| Preparado | Círculos animados, OK y BACK disponibles. |
| Captura | Círculos fijos, cuenta de 3 a 1, OK inactivo y BACK disponible. |
| Resultado | En IDENTIFICAR, UP/DN recorre comparación, gráfica y estadísticas ADC. La tercera página muestra 298/298 frames, mínimo, máximo y promedio. |
| Selección | CON VOZ cercano a cero en una entrada DC estable y con poco ruido. |
| Gráfica | Si CON VOZ es cero, mensaje SIN VOZ DETECTADA. |
| Repetir | Nuevos círculos fijos y cuenta inicial de 3 segundos, sin tarjeta anterior. |

En la prueba sintética de PC, una entrada DC ideal produce cero frames seleccionados. En la placa puede haber ruido, variación de referencia o interferencia; registrar lo observado.

## Captura con la etapa analógica

1. Medir alimentación, offset y excursión de salida antes de conectar el ADC.
2. Capturar pronunciaciones de MARIPOSA y registrar mínimo, máximo, promedio y cantidad CON VOZ.
3. Repetir la toma y comprobar que no aparecen errores ni datos anteriores.
4. Cancelar con BACK y verificar que se puede iniciar una toma nueva.
5. Consultar LOCUTORES después de registrar audio real y verificar que se muestran las referencias guardadas sin iniciar otra captura.

No se reserva un buffer de tres segundos: se reciben mitades de 640 muestras y cada una dispone de 40 ms antes de que DMA necesite reutilizarla. La detección de sobreescritura rechaza una toma; medir el consumo real para comprobar el margen.

## Ajuste del criterio de energía

`Core/Inc/voxid_config.h` define `VOXID_VOICE_MIN_RMS = 0.01f`, normalizado a escala completa PCM. Comparar tomas de ruido ambiente y voz con el mismo ajuste de ganancia. Modificar el umbral solo con esas observaciones y repetir las pruebas.

Un nivel energético suficiente no distingue voz de golpes o ruido fuerte. Una toma se usa como prototipo solo tras cumplir el criterio de registro y ser aceptada explícitamente. La comparación requiere calibración con voces reales.

## Cierre antes de etapa 3

Registrar versión compilada, condiciones de entrada, capturas repetidas, comportamiento de cancelación y ausencia de errores DMA. La etapa 3 ya tiene implementación de registro y su [propia validación](../C0DE/ophyra/voxid/STAGE3_ENROLLMENT.md); el cierre físico de ambas comprobaciones sigue siendo necesario.

La persistencia, identificación y sus comprobaciones físicas pendientes se detallan en [etapa 4](../C0DE/ophyra/voxid/STAGE4_STORAGE_IDENTIFY.md).

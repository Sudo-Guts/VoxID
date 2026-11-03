# Etapa 2: PCM y MFCC reales

Se implementa la conexion de ADC y DMA con el extractor MFCC compartido. El registro de prototipos se implementa por separado en voxid_enroll, descrito en STAGE3_ENROLLMENT.md. La persistencia y comparacion de la version actual se describen en [etapa 4](STAGE4_STORAGE_IDENTIFY.md); no se accede a EEPROM durante adquisicion o DSP.

## Flujo

1. ADC1_IN2 entrega cuentas de 12 bits sin signo.
2. DMA entrega mitades de 800 muestras al bucle principal.
3. `voxid_voice_process_block()` centra cada cuenta: `x = (adc - 2048) * 16`.
4. El filtro DC aplica `y[n] = x[n] - x[n-1] + (32604/32768) * y[n-1]` y satura a int16_t.
5. La primera muestra inicializa la entrada anterior; una entrada DC constante no introduce un escalon artificial.
6. El extractor produce frames de 400 muestras con avance de 160: Hamming, FFT 512, potencia, 26 Mel, logaritmo y 13 MFCC.
7. Se validan los coeficientes y se calcula la energia PCM del mismo frame, usando su buffer existente.
8. Solo los frames sobre el umbral RMS se acumulan en la media. Tambien se conserva el total para verificar continuidad.
9. La toma se acepta si termina con 40 000 muestras y 248 frames finitos.

No se almacena el audio completo. Los parametros compartidos estan en `Core/Inc/voxid_config.h`.

## Resultados y silencio

`VoxIDVoiceStats.frames` cuenta todos los frames y `voiced_frames` cuenta los que superan el criterio de energia. `mean_mfcc` promedia unicamente los seleccionados; si no hay ninguno, su contenido no representa voz y no debe usarse como perfil ni dibujarse como grafica de voz.

El umbral inicial es RMS normalizado 0.01. Es un criterio provisional que requiere calibracion fisica y tambien puede seleccionar ruido intenso.

En IDENTIFICAR, la UI muestra primero el resultado de comparacion; UP/DN permite consultar la grafica y una tercera pagina con los 248 frames, CON VOZ y estadisticas ADC. En la grafica dibuja la media seleccionada; con cero frames muestra SIN VOZ DETECTADA. MARIPOSA es una referencia de uso, no una palabra verificada por este codigo.

## Validacion

Las regresiones en PC cubren silencio, DC desplazada, tono, senal debil, intermitencia, reinicio y errores. El camino ADC y DMA se prueba con HAL simulada.

Quedan pendientes compilacion ARM, captura fisica, calibracion y consumo real frente a los 50 ms de cada mitad. Seguir [la validacion de etapas 1 y 2](../../../DOC/Validacion_etapas_1_2.md).


# Etapa 2: PCM y MFCC reales

Se implementa la conexión de ADC y DMA con el extractor MFCC compartido. No se registran prototipos ni se accede a EEPROM.

## Flujo

1. ADC1_IN2 entrega cuentas de 12 bits sin signo.
2. DMA entrega mitades de 640 muestras al bucle principal.
3. `voxid_voice_process_block()` centra cada cuenta: `x = (adc - 2048) * 16`.
4. El filtro DC aplica `y[n] = x[n] - x[n-1] + (32604/32768) * y[n-1]` y satura a int16_t.
5. La primera muestra inicializa la entrada anterior; una entrada DC constante no introduce un escalón artificial.
6. El extractor produce frames de 400 muestras con avance de 160: Hamming, FFT 512, potencia, 26 Mel, logaritmo y 13 MFCC.
7. Se validan los coeficientes y se calcula la energía PCM del mismo frame, usando su buffer existente.
8. Solo los frames sobre el umbral RMS se acumulan en la media. También se conserva el total para verificar continuidad.
9. La toma se acepta si termina con 48 000 muestras y 298 frames finitos.

No se almacena el audio completo. Los parámetros compartidos están en `Core/Inc/voxid_config.h`.

## Resultados y silencio

`VoxIDVoiceStats.frames` cuenta todos los frames y `voiced_frames` cuenta los que superan el criterio de energía. `mean_mfcc` promedia únicamente los seleccionados; si no hay ninguno, su contenido no representa voz y no debe usarse como perfil ni dibujarse como gráfica de voz.

El umbral inicial es RMS normalizado 0.01. Es un criterio provisional que requiere calibración física y también puede seleccionar ruido intenso.

La UI muestra los 298 frames, CON VOZ y estadísticas ADC. En la segunda página dibuja la media seleccionada; con cero frames muestra SIN VOZ DETECTADA. MARIPOSA es una referencia de uso, no una palabra verificada por este código.

## Validación

Las regresiones en PC cubren silencio, DC desplazada, tono, señal débil, intermitencia, reinicio y errores. El camino ADC y DMA se prueba con HAL simulada.

Quedan pendientes compilación ARM, captura física, calibración y consumo real frente a los 40 ms de cada mitad. Seguir [la validación de etapas 1 y 2](../../../DOC/Validacion_etapas_1_2.md).


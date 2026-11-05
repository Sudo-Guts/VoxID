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

## Validacion numerica

Se compararon las mismas fuentes C usadas por el firmware con una referencia independiente en float64, fuera del repositorio. La referencia usa NumPy 2.3.5 para FFT y SciPy 1.17.0 para DCT-II ortonormal. No se agregan bibliotecas, pruebas ni adaptadores de PC al proyecto STM32.

Las convenciones son PCM / 32768, Hamming simetrica de 400 muestras, FFT de 512 sin normalizacion directa, potencia sin duplicar bins, filtros triangulares HTK en Hz con altura unitaria, logaritmo natural con piso 1e-10 y los primeros 13 coeficientes DCT-II ortonormales.

- Se verificaron 24 frames individuales y 13 capturas sinteticas de 40000 muestras, tanto con -O0 como con -O2, sin fast-math y con contraccion de operaciones desactivada.
- Las capturas aportaron 6448 frames y 83824 coeficientes comparados. Se incluyeron silencio, DC, impulsos, tonos, ruido, intermitencia, barrido de frecuencia y recorte.
- Para cada coeficiente y para la media se exigio error absoluto <= 1e-3 + 1e-4 * abs(referencia). El maximo observado fue aproximadamente 2.06424e-4 por coeficiente y 1.91336e-4 en la media.
- Coincidieron exactamente el PCM, los conteos y la seleccion de frames por energia. Procesar por muestras o por bloques de 800 produjo las mismas estadisticas.

La comparacion encontro un error de extremo en mel_init: el limite solicitado de 8000 Hz se convertia en 8000.0009765625 Hz por redondeo. Una potencia unitaria solo en Nyquist daba 1.25966e-6 en el ultimo filtro en vez de cero. Los extremos ahora se fijan a min_hz y max_hz; ese aporte es exactamente cero y las comparaciones pasan con las tolerancias originales.

La revision DSP pasa a 2. Los perfiles de revision 1 se rechazan incluso con CRC correcto y deben registrarse de nuevo, sin migracion.

Referencias de las transformadas: [FFT real de NumPy](https://numpy.org/doc/stable/reference/generated/numpy.fft.rfft.html) y [DCT de SciPy](https://docs.scipy.org/doc/scipy/reference/generated/scipy.fft.dct.html).

## Validacion pendiente en Ophyra

La comparacion se ejecuto en Linux con GCC; no verifica el binario ARM, los tiempos de la placa ni la precision con personas. Quedan pendientes compilacion ARM, captura fisica, calibracion y consumo real frente a los 50 ms de cada mitad. Seguir [la validacion de etapas 1 y 2](Validacion_etapas_1_2.md).

